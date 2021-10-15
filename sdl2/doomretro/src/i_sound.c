/*
========================================================================

                           D O O M  R e t r o
         The classic, refined DOOM source port. For Windows PC.

========================================================================

  Copyright © 1993-2012 id Software LLC, a ZeniMax Media company.
  Copyright © 2013-2018 Brad Harding.

  DOOM Retro is a fork of Chocolate DOOM. For a list of credits, see
  <https://github.com/bradharding/doomretro/wiki/CREDITS>.

  This file is part of DOOM Retro.

  DOOM Retro is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation, either version 3 of the License, or (at your
  option) any later version.

  DOOM Retro is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with DOOM Retro. If not, see <https://www.gnu.org/licenses/>.

  DOOM is a registered trademark of id Software LLC, a ZeniMax Media
  company, in the US and/or other countries and is used without
  permission. All other trademarks are the property of their respective
  holders. DOOM Retro is in no way affiliated with nor endorsed by
  id Software.

========================================================================
*/

#include "c_console.h"
#include "i_system.h"
#include "m_config.h"
#include "m_misc.h"
#include "s_sound.h"
#include "SDL2/SDL_mixer.h"
#include "version.h"
#include "w_wad.h"
#include "z_zone.h"

#define CACHESIZE   (64 * 1024 * 1024)

typedef struct allocated_sound_s allocated_sound_t;

struct allocated_sound_s
{
    sfxinfo_t               *sfxinfo;
    Mix_Chunk               chunk;
    int                     use_count;
    int                     pitch;
    allocated_sound_t       *prev;
    allocated_sound_t       *next;
};

static dboolean             sound_initialized;

static allocated_sound_t    *channels_playing[s_channels_max];

static int                  mixer_freq;
static Uint16               mixer_format;
static int                  mixer_channels;

// Doubly-linked list of allocated sounds.
// When a sound is played, it is moved to the head, so that the oldest sounds not used recently are at the tail.
static allocated_sound_t    *allocated_sounds_head;
static allocated_sound_t    *allocated_sounds_tail;
static int                  allocated_sounds_size;

// Hook a sound into the linked list at the head.
static void AllocatedSoundLink(allocated_sound_t *snd)
{
    snd->prev = NULL;

    snd->next = allocated_sounds_head;
    allocated_sounds_head = snd;

    if (!allocated_sounds_tail)
        allocated_sounds_tail = snd;
    else
        snd->next->prev = snd;
}

// Unlink a sound from the linked list.
static void AllocatedSoundUnlink(allocated_sound_t *snd)
{
    if (!snd->prev)
        allocated_sounds_head = snd->next;
    else
        snd->prev->next = snd->next;

    if (!snd->next)
        allocated_sounds_tail = snd->prev;
    else
        snd->next->prev = snd->prev;
}

static void FreeAllocatedSound(allocated_sound_t *snd)
{
    // Unlink from linked list.
    AllocatedSoundUnlink(snd);

    // Keep track of the amount of allocated sound data:
    allocated_sounds_size -= snd->chunk.alen;

    free(snd);
}

// Search from the tail backwards along the allocated sounds list, find and free a sound that is
// not in use, to free up memory. Return true for success.
static dboolean FindAndFreeSound(void)
{
    allocated_sound_t   *snd = allocated_sounds_tail;

    while (snd)
    {
        if (!snd->use_count)
        {
            FreeAllocatedSound(snd);
            return true;
        }

        snd = snd->prev;
    }

    // No available sounds to free...
    return false;
}

// Enforce SFX cache size limit. We are just about to allocate "len" bytes on the heap for a new
// sound effect, so free up some space so that we keep allocated_sounds_size < snd_cachesize
static void ReserveCacheSpace(int len)
{
    // Keep freeing sound effects that aren't currently being played, until there is enough space
    // for the new sound.
    while (allocated_sounds_size + len > CACHESIZE)
        // Free a sound. If there is nothing more to free, stop.
        if (!FindAndFreeSound())
            break;
}

// Allocate a block for a new sound effect.
static allocated_sound_t *AllocateSound(sfxinfo_t *sfxinfo, int len)
{
    allocated_sound_t   *snd;

    // Keep allocated sounds within the cache size.
    ReserveCacheSpace(len);

    // Allocate the sound structure and data. The data will immediately follow the structure, which
    // acts as a header.
    do
    {
        // Out of memory? Try to free an old sound, then loop round and try again.
        if (!(snd = malloc(sizeof(allocated_sound_t) + len)) && !FindAndFreeSound())
            return NULL;
    } while (!snd);

    // Skip past the chunk structure for the audio buffer
    snd->chunk.abuf = (byte *)(snd + 1);
    snd->chunk.alen = len;
    snd->chunk.allocated = 1;
    snd->chunk.volume = MIX_MAX_VOLUME;
    snd->pitch = NORM_PITCH;
    snd->sfxinfo = sfxinfo;
    snd->use_count = 0;

    // Keep track of how much memory all these cached sounds are using...
    allocated_sounds_size += len;

    AllocatedSoundLink(snd);
    return snd;
}

// Lock a sound, to indicate that it may not be freed.
static void LockAllocatedSound(allocated_sound_t *snd)
{
    // Increase use count, to stop the sound being freed.
    snd->use_count++;

    // When we use a sound, re-link it into the list at the head, so that the oldest sounds fall to
    // the end of the list for freeing.
    AllocatedSoundUnlink(snd);
    AllocatedSoundLink(snd);
}

// Unlock a sound to indicate that it may now be freed.
static void UnlockAllocatedSound(allocated_sound_t *snd)
{
    snd->use_count--;
}

static allocated_sound_t *GetAllocatedSoundBySfxInfoAndPitch(sfxinfo_t *sfxinfo, int pitch)
{
    allocated_sound_t   *p = allocated_sounds_head;

    while (p)
    {
        if (p->sfxinfo == sfxinfo && p->pitch == pitch)
            return p;

        p = p->next;
    }

    return NULL;
}

// Allocate a new sound chunk and pitch-shift an existing sound up-or-down into it.
static allocated_sound_t *PitchShift(allocated_sound_t *insnd, int pitch)
{
    allocated_sound_t   *outsnd;
    Sint16              *srcbuf = (Sint16 *)insnd->chunk.abuf;
    Uint32              srclen = insnd->chunk.alen;
    Sint16              *dstbuf;

    // determine ratio pitch:NORM_PITCH and apply to srclen, then invert.
    // This is an approximation of vanilla behavior based on measurements
    Uint32              dstlen = (int)((1 + (1 - (float)pitch / NORM_PITCH)) * srclen);

    // ensure that the new buffer is an even length
    if (!(dstlen % 2))
        dstlen++;

    if (!(outsnd = AllocateSound(insnd->sfxinfo, dstlen)))
        return NULL;

    outsnd->pitch = pitch;
    dstbuf = (Sint16 *)outsnd->chunk.abuf;

    // loop over output buffer. find corresponding input cell, copy over
    for (Sint16 *inp, *outp = dstbuf; outp < dstbuf + dstlen / 2; outp++)
    {
        inp = srcbuf + (int)((float)(outp - dstbuf) / dstlen * srclen);
        *outp = *inp;
    }

    return outsnd;
}

// When a sound stops, check if it is still playing. If it is not, we can mark the sound data as
// CACHE to be freed back for other means.
static void ReleaseSoundOnChannel(int channel)
{
    allocated_sound_t   *snd = channels_playing[channel];

    Mix_HaltChannel(channel);

    if (!snd)
        return;

    channels_playing[channel] = NULL;
    UnlockAllocatedSound(snd);

    // if the sound is a pitch-shift and it's not in use, immediately free it
    if (snd->pitch != NORM_PITCH && snd->use_count <= 0)
        FreeAllocatedSound(snd);
}
#define M_PI 3.14159265358979323846
// Generic sound expansion function for any sample rate.
static dboolean ExpandSoundData(sfxinfo_t *sfxinfo, byte *data, int samplerate, int length)
{
    unsigned int        expanded_length = (unsigned int)(((uint64_t)length * mixer_freq) / samplerate);
    allocated_sound_t   *snd = AllocateSound(sfxinfo, expanded_length * 4);
    Sint16              *expanded;
    int                 expand_ratio;
    double              dt;
    double              alpha;

    if (!snd)
        return false;

    expanded = (Sint16 *)(&snd->chunk)->abuf;
    expand_ratio = (length << 8) / expanded_length;

    for (unsigned int i = 0; i < expanded_length; i++)
    {
        int src = (i * expand_ratio) >> 8;

        expanded[i * 2] = expanded[i * 2 + 1] = (data[src] | (data[src] << 8)) - 32768;
    }

    dt = 1.0 / mixer_freq;
    alpha = dt / (1.0 / (M_PI * samplerate) + dt);

    for (unsigned int i = 2; i < expanded_length * 2; ++i)
        expanded[i] = (Sint16)(alpha * expanded[i] + (1 - alpha) * expanded[i - 2]);

    return true;
}

// Load and convert a sound effect
// Returns true if successful
static dboolean CacheSFX(sfxinfo_t *sfxinfo)
{

    // need to load the sound
    int             lumpnum = sfxinfo->lumpnum;
    byte            *data = W_CacheLumpNum(lumpnum);
    unsigned int    lumplen = W_LumpLength(lumpnum);
    unsigned int    length;

    // Check the header, and ensure this is a valid sound
    if (lumplen < 8 || data[0] != 0x03 || data[1] != 0x00)
        return false;   // Invalid sound

    length = ((data[7] << 24) | (data[6] << 16) | (data[5] << 8) | data[4]);

    // If the header specifies that the length of the sound is greater than the length of the lump
    // itself, this is an invalid sound lump

    // We also discard sound lumps that are less than 49 samples long, as this is how DMX behaves -
    // although the actual cut-off length seems to vary slightly depending on the sample rate. This
    // needs further investigation to better understand the correct behavior.
    if (length > lumplen - 8 || length <= 48)
        return false;

    // The DMX sound library seems to skip the first 16 and last 16 bytes of the lump - reason unknown.

    // Sample rate conversion
    if (!ExpandSoundData(sfxinfo, data + 24, ((data[3] << 8) | data[2]), length - 32))
        return false;

    // don't need the original lump any more
    W_UnlockLumpNum(lumpnum);

    return true;
}

// Load a SFX chunk into memory and ensure that it is locked.
static dboolean LockSound(sfxinfo_t *sfxinfo)
{
    // If the sound isn't loaded, load it now
    if (!GetAllocatedSoundBySfxInfoAndPitch(sfxinfo, NORM_PITCH) && !CacheSFX(sfxinfo))
        return false;

    LockAllocatedSound(GetAllocatedSoundBySfxInfoAndPitch(sfxinfo, NORM_PITCH));
    return true;
}

// Retrieve the raw data lump index for a given SFX name.
int I_GetSfxLumpNum(sfxinfo_t *sfx)
{
    char    namebuf[9];

    if (sfx->link)
        sfx = sfx->link;

    M_snprintf(namebuf, sizeof(namebuf), "ds%s", sfx->name);
    return W_GetNumForName(namebuf);
}

void I_UpdateSoundParams(int handle, int vol, int sep)
{
    if (!sound_initialized || handle < 0 || handle >= s_channels_max)
        return;

    Mix_SetPanning(handle, BETWEEN(0, (254 - sep) * vol / MAX_SFX_VOLUME, 255),
        BETWEEN(0, sep * vol / MAX_SFX_VOLUME, 255));
}

//
// Starting a sound means adding it to the current list of active sounds in the internal channels.
// As the SFX info struct contains e.g. a pointer to the raw data, it is ignored.
// As our sound handling does not handle priority, it is ignored.
// Pitching (that is, increased speed of playback) is set, but currently not used by mixing.
//
int I_StartSound(sfxinfo_t *sfxinfo, int channel, int vol, int sep, int pitch)
{
    allocated_sound_t   *snd;

    if (!sound_initialized || channel < 0 || channel >= s_channels_max)
        return -1;

    // Release a sound effect if there is already one playing on this channel
    ReleaseSoundOnChannel(channel);

    // Get the sound data
    if (!LockSound(sfxinfo))
        return -1;

    if (!(snd = GetAllocatedSoundBySfxInfoAndPitch(sfxinfo, pitch)))
    {
        allocated_sound_t   *newsnd;

        // fetch the base sound effect, un-pitch-shifted
        if (!(snd = GetAllocatedSoundBySfxInfoAndPitch(sfxinfo, NORM_PITCH)))
            return -1;

        if (s_randompitch && pitch != NORM_PITCH)
            if ((newsnd = PitchShift(snd, pitch)))
            {
                LockAllocatedSound(newsnd);
                UnlockAllocatedSound(snd);
                snd = newsnd;
            }
    }
    else
        LockAllocatedSound(snd);

    // play sound
    Mix_PlayChannel(channel, &snd->chunk, 0);

    channels_playing[channel] = snd;

    // set separation, etc.
    I_UpdateSoundParams(channel, vol, sep);

    return channel;
}

void I_StopSound(int handle)
{
    if (!sound_initialized || handle < 0 || handle >= s_channels_max)
        return;

    // Sound data is no longer needed; release the sound data being used for this channel
    ReleaseSoundOnChannel(handle);
}

dboolean I_SoundIsPlaying(int handle)
{
    if (!sound_initialized || handle < 0 || handle >= s_channels_max)
        return false;

    return Mix_Playing(handle);
}

// Periodically called to update the sound system
void I_UpdateSound(void)
{
    // Check all channels to see if a sound has finished
    for (int i = 0; i < s_channels_max; i++)
        if (channels_playing[i] && !I_SoundIsPlaying(i))
            // Sound has finished playing on this channel, but sound data has not been released to cache
            ReleaseSoundOnChannel(i);
}

dboolean I_AnySoundStillPlaying(void)
{
    dboolean    result = false;

    for (int i = 0; i < s_channels_max; i++)
        result |= Mix_Playing(i);

    return result;
}

void I_ShutdownSound(void)
{
    if (!sound_initialized)
        return;

    Mix_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    sound_initialized = false;
}

dboolean I_InitSound(void)
{
    const SDL_version   *linked = Mix_Linked_Version();

    // No sounds yet
    for (int i = 0; i < s_channels_max; i++)
        channels_playing[i] = NULL;

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
        return false;

    if (linked->major != SDL_MIXER_MAJOR_VERSION || linked->minor != SDL_MIXER_MINOR_VERSION)
        I_Error("The wrong version of %s was found. %s requires v%i.%i.%i.",
            SDL_MIXER_FILENAME, PACKAGE_NAME, SDL_MIXER_MAJOR_VERSION, SDL_MIXER_MINOR_VERSION, SDL_MIXER_PATCHLEVEL);

    if (linked->patch != SDL_MIXER_PATCHLEVEL)
        C_Warning("The wrong version of <b>%s</b> was found. <i>%s</i> requires v%i.%i.%i.",
            SDL_MIXER_FILENAME, PACKAGE_NAME, SDL_MIXER_MAJOR_VERSION, SDL_MIXER_MINOR_VERSION, SDL_MIXER_PATCHLEVEL);

    if (Mix_OpenAudio(SAMPLERATE, MIX_DEFAULT_FORMAT, CHANNELS, CHUNKSIZE) < 0)
        return false;

    if (!Mix_QuerySpec(&mixer_freq, &mixer_format, &mixer_channels))
        return false;

    Mix_AllocateChannels(s_channels_max);
    SDL_PauseAudio(0);
    sound_initialized = true;
    return true;
}
