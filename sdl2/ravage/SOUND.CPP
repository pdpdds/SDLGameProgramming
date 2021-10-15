
/*-------------------------------------------------------*/
/* High Level Sound Routines.                            */
/* [c]copyright 1995 by AlphaHelix                       */
/*                                                       */
/*                                                       */
/*                                                       */
/*-------------------------------------------------------*/

// Master SOUND ENABLE SWITCH (debugging reasons)
//#define NO_SOUND

#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>


#include "ERROR.HPP"
#include "MEMORY.HPP"
#include "DISKIO.HPP"
#include "SOUND.HPP"
#include "RAVAGE.HPP"

#include "SOUND_SDL.HPP"

// Mod player variables.
static Mix_Music *mf = NULL;          // Music structure for Module file.
static int     mod_play;         // MOD is playing(1) or stopped(0).
static int     mod_vol;          // Volume of MOD voices.

// Wave variables.
static int     wav_vol;          // WAVE volume.

// All other variables.
static int     initialized = 0;


//-------------------------------------------------------------------
// WavePool members.
//-------------------------------------------------------------------
void WavePool::load(char *f)
{
   myFile   myfile;
   int      i;
   SDL_RWops *memSample = NULL;

// read in header
   if (!myfile.open(f)) error("WavPool not found: ", f);

   myfile.read(&nsamples, sizeof(long));

// Just quit if empty library.
   if (nsamples == 0) {
      myfile.close();
      return;
   }

   sample = new Sample[nsamples];
   myfile.read(sample, nsamples * sizeof(Sample));

   fseek(myfile.getFILE(), myfile.getbase(), SEEK_SET);
   memSample = SDL_RWFromMem(myfile.getMemory(), myfile.getsize());

// Load WAVES.
   for (i = 0; i < nsamples; i++) {
#ifndef NO_SOUND
      memSample->seek(memSample, (long)sample[i].sample, RW_SEEK_SET);

      if((sample[i].sample = Mix_LoadWAV_RW(memSample, SDL_FALSE)) == NULL) error("Error loading sample. ", (char *)Mix_GetError());
#endif
   }

   SDL_RWclose(memSample);
   myfile.close();
}

void WavePool::unload(void)
{
   int   i;

   if (nsamples == 0) return;

   for (i = 0; i < nsamples; i++) {
#ifndef NO_SOUND
      Mix_FreeChunk(sample[i].sample);
#endif
   }
   delete []sample;
}

void WavePool::play(int n)
{
   Mix_Chunk *s = sample[n].sample;

   //printf("Sound vol %d wave_vol %d\n", sample[n].volume*wav_vol, wav_vol);
   if (!wav_vol) return;

#ifndef NO_SOUND
   if(!sound_Call(s, n, SND_DUPLICATE, sample[n].volume, 0)) printf("Sound not added\n");
#endif

}



//-------------------------------------------------------------------
// MOD functions.
//-------------------------------------------------------------------


/*---------------------------------------------------------
 Function:

 Description:
 Load a MOD or UNIMOD file into memory and prepare it to
 be played.
---------------------------------------------------------*/
void s_loadmod(char *f)
{
#ifndef NO_SOUND
   myFile   myfile;
   SDL_RWops *mod = NULL;

   if (mf) s_unloadmod();
   if (!myfile.open(f)) error("(s_loadmod): not found: ", f);

   mod = SDL_RWFromMem(myfile.getMemory(), myfile.getsize());

   if(mod == NULL) error("SDL_RWFromMem: ", SDL_GetError());

   #ifdef PANDORA
   mf = Mix_LoadMUS_RW(mod); SDL_FreeRW(mod);
   #else
   mf = Mix_LoadMUS_RW(mod, SDL_TRUE);
   #endif

   if(!mf) {
      printf("Mix_LoadMUS_RW: %s\n", Mix_GetError());
   }

   myfile.close();
#endif
}

/*---------------------------------------------------------
 Function:

 Description:
 Unload a previously loaded MOD file from memory.
---------------------------------------------------------*/
void s_unloadmod(void)
{
#ifndef NO_SOUND
   s_stopmod();
   if (mf) Mix_FreeMusic(mf);
   mf = NULL;
#endif
}

/*---------------------------------------------------------
 Function: startmod & stopmod

 Description:
 Start and stop playing a previous loaded MUSIC file
 respectivly.
---------------------------------------------------------*/
void s_startmod(void)
{
   #ifndef NO_SOUND
   if (mf) mod_play = 1;
   if(Mix_PlayMusic(mf, settings.loop_music ? -1 : 1) == -1) printf("Mix_PlayMusic(): %s\n", Mix_GetError());
   #endif
}

void s_stopmod(void)
{
#ifndef NO_SOUND
   Mix_HaltMusic();
   mod_play = 0;
#endif
}

char *s_modname(void)
{
#ifdef NO_SOUND
   return "debug mode";
#else
   return "Music Temp";
#endif
}

/*---------------------------------------------------------
 Function:

 Description:
 Set MUSIC volume. Must be in the range (0 to 64).
 If 0 is given, all the voices used by the MOD won't be
 mixed anymore. So this saves a lot of power on a SoundBlaster.
---------------------------------------------------------*/
void s_setmodvol(int volume)
{
   int percentage = (volume * 100)/64;

#ifndef NO_SOUND
   sound_SetMusicVolume(percentage);
   mod_vol = volume;
#endif
}

int s_getmodvol(void)
{
   return mod_vol;
}

/*---------------------------------------------------------
 Function:

 Description:
 Set the volume to play any WAV files.
 Must be in the range 0 to 64.
 If 0 is given, all the WAV voices are releases saving
 a lot of cpu power with a non hardware-mixing SoundCard.
---------------------------------------------------------*/
void s_setwavvol(int volume)
{
   int percentage = (volume * 100)/64;
   wav_vol = volume;
#ifndef NO_SOUND
   sound_SetVolume(percentage);
#endif
}


/*---------------------------------------------------------
 Function:

 Description:
 Returns Sound Card Driver Name.
---------------------------------------------------------*/
char *s_drivername(void)
{
#ifdef NO_SOUND
   return "Debug Mode";
#else
   return "SDL Audio";
#endif
}

/*---------------------------------------------------------
 Function:

 Description:
 Call this function once in a while to update the
 player.
---------------------------------------------------------*/
void s_update(void)
{
#ifndef NO_SOUND
   sound_Play();
#endif
}

/*---------------------------------------------------------
 Function:

 Description:
 Initialize sound system.
---------------------------------------------------------*/
void initsound()
{

   if(Mix_OpenAudio(8000, AUDIO_S16SYS, 2, 512) == -1)
	{
      error("Unable to open audio device: ", Mix_GetError());
   }

   if(!Mix_Init(MIX_INIT_MOD))
	{
      printf("Unable to support MOD files: %s\n", Mix_GetError());
   }

#ifndef NO_SOUND
   if (initialized) return;

// Initialize several more or less cool variables.
   mf = 0;                 // No MOD loaded.
   mod_play = 0;           // No MOD has been loaded so don't play it.

// initialize soundcard
   if (sound_Init(128, 8, 100, 1)) {
      error("Sound Error: ", Mix_GetError());
   } else {
      initialized = 1;
   }
#endif
}

void shutsound(void)
{
   if (!initialized) return;
#ifndef NO_SOUND
   s_stopmod();
   sound_Clean();

#endif
   Mix_Quit();
   initialized = 0;
}

