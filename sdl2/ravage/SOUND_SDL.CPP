#include <stdio.h>
#include "SOUND_SDL.HPP"

static Sound_Manager soundManager;

/*
    Function: soundInfo_Create

    Description -
    Create a sound information structure that can is used by the sound manager to play
    Mix_Chunk sounds.

    5 arguments:
    Mix_Chunk *sound - The sound loaded.
    int ID - The ID of the sound, so it can be tracked to not allow duplicates. (-1 if no ID)
    int playType - The playing behaviour of the sound.
    int volume - The volume of the sound.
    int channel - The channel of the sound (-1 for any).
    int loops - The amound of times the sound will loop before stopping.
*/
Sound_Info *soundInfo_Create(Mix_Chunk *sound, int ID, int playType, int volume, int channel, int loops)
{
    Sound_Info *newSoundInfo = (Sound_Info *)malloc(sizeof(Sound_Info));

    soundInfo_Setup(newSoundInfo, sound, ID, playType, volume, channel, loops);

    soundManager.soundInfoCreated ++;

    return newSoundInfo;
}

void soundInfo_Setup(Sound_Info *soundInfo, Mix_Chunk *sound, int ID, int playType, int volume, int channel, int loops)
{
    soundInfo->sound = sound;
    soundInfo->channel = channel;
    soundInfo->loops = loops;
    soundInfo->volume = volume;
    soundInfo->ID = ID;

    soundInfo->playType = playType;
    soundInfo->finished = 0;

    return;
}

/*
    Function: soundInfo_Delete

    Description -
    Deletes the Sound_Info structure.

    1 argument:
    Sound_Info *soundInfo - The Sound_Info structure to delete.
*/
void soundInfo_Delete(Sound_Info *soundInfo)
{
    free(soundInfo);

    soundManager.soundInfoCreated --;

    return;
}

/*
    Function: sound_Init

    Description -
    Initialises the sound manager.

    4 arguments:
    int maxSounds - The maximum amount of sounds per frame allowed.
    int maxChannels - The maximum amount of channels allowed for mixing.
    int volume - The starting volume %.
    int enabled - If 0 then no sounds will be played.
*/
int sound_Init(int maxSounds, int maxChannels, int volume, int enabled)
{
    int x = 0;
    soundManager.soundList = NULL;
    soundManager.soundsLoaded = 0;

    if(maxChannels > 0)
    {
        soundManager.totalChannels = maxChannels;

        Mix_AllocateChannels(maxChannels);
        Mix_ChannelFinished(&sound_ReportChannelFinished);

        soundManager.playingSounds = (Sound_Info *)malloc(maxChannels * sizeof(Sound_Info));

        for(x = 0; x < maxChannels; x++)
        {
            soundInfo_Setup(&soundManager.playingSounds[x], NULL, -1, -1, -1, -1, 0);
        }
    }

    soundManager.maxSoundsPerFrame = maxSounds;
    if(maxSounds > 0)
    {
        soundManager.insertSounds = (Sound_Info *)malloc(maxSounds * sizeof(Sound_Info));

        for(x = 0; x < maxSounds; x++)
        {
            soundInfo_Setup(&soundManager.insertSounds[x], NULL, -1, -1, -1, -1, 0);
        }
    }
    else
    {
        soundManager.insertSounds = NULL;
    }

    soundManager.totalSoundsInserted = 0;
    soundManager.totalSoundsPlaying = 0;

    soundManager.enabled = enabled;
    soundManager.musicVolume = soundManager.volume = volume;
    soundManager.mute = 0;

    sound_Unmute();
    sound_SetVolume(volume);

    return 0;
}

/*
    Function: sound_Clean

    Description -
    Cleans up the sound manager and deletes any sounds that it is tracking.

    0 arguments:
*/
void sound_Clean(void)
{
    if(soundManager.insertSounds != NULL)
    {
        free(soundManager.insertSounds);
    }

    if(soundManager.playingSounds != NULL)
    {
        free(soundManager.playingSounds);
    }

    sound_ReleaseAll();

    return;
}

/*
    Function: sound_NumID

    Description -
    Returns the number of sounds currently being played with a specific ID.

    1 arguments:
    int ID - The ID of the sound.
*/
int sound_NumID(int ID)
{
    int numID = 0;

    int x = 0;

    for(x = 0; x < soundManager.totalChannels; x++)
    {
        if(soundManager.playingSounds[x].finished == 0 && soundManager.playingSounds[x].ID == ID)
        {
            numID ++;
        }
    }

    return numID;
}


/*
    Function: sound_CheckCollision

    Description -
    Returns NULL if the sound is not being played, otherwise
    returns the record of the sound that is playing.

    2 arguments:
    int ID - The ID of the sound.
    Mix_Chunk *sound - The sound data.
*/
Sound_Info *sound_CheckCollision(int ID, Mix_Chunk *sound)
{
    int x = 0;

    for(x = 0; x < soundManager.totalChannels; x++)
    {
        if(soundManager.playingSounds[x].finished == 0 && soundManager.playingSounds[x].ID == ID && soundManager.playingSounds[x].sound == sound)
        {
            return &soundManager.playingSounds[x];
        }
    }

    return NULL;

    #ifdef DONTDEF
    for(x = 0; x < soundManager.totalSoundsPlaying; x++)
    {
        if(soundManager.playSounds[x]->sound == sound && soundManager.playSounds[x]->channel == channel)
        {
            /*If the sound to be checked has a higher volume tham the duplicate, make sure
            the sound is at least that higher volume.*/
            if(soundManager.playSounds[x]->volume < volume)
            {
                soundManager.playSounds[x]->volume = volume;
            }

            return 1;
        }
    }
    #endif

    return 0;
}

int sound_ToggleMute(void)
{
    if(soundManager.mute == 0)
    {
        sound_Mute();
    }
    else
    {
        sound_Unmute();
    }

    return soundManager.mute;
}

void sound_Mute(void)
{
    soundManager.mute = 1;

    sound_AssignVolume();

    return;
}

void sound_Unmute(void)
{
    soundManager.mute = 0;

    sound_AssignVolume();

    return;
}

void sound_Stop(int channel)
{
    if(soundManager.enabled == 0)
    {
        return;
    }

    Mix_HaltChannel(channel);

    return;
}

void sound_Pause(int channel)
{
    if(soundManager.enabled == 0)
    {
        return;
    }

    Mix_Pause(channel);

    return;
}

void sound_Resume(int channel)
{
    if(soundManager.enabled == 0)
    {
        return;
    }

    Mix_Resume(channel);

    return;
}

int sound_ChangeVolume(int changePercent)
{
   return sound_SetVolume(soundManager.volume + changePercent);
}

int sound_SetVolume(int volumePercent)
{
    if(volumePercent > 100)
    {
        volumePercent = 100;
    }
    else if(volumePercent < 0)
    {
        volumePercent = 0;
    }

    soundManager.volume = volumePercent;

    sound_AssignVolume();

    return soundManager.volume;
}

int sound_SetMusicVolume(int volumePercent)
{
    if(volumePercent > 100)
    {
        volumePercent = 100;
    }
    else if(volumePercent < 0)
    {
        volumePercent = 0;
    }

	 soundManager.musicVolume = volumePercent;
	 sound_AssignVolume();

    return soundManager.musicVolume;
}

/*
    Function: sound_AssignVolume

    Description -
    Converts the volume of the sound manager into the correct value for SDL_Mixer.
    Then uses SDL_Mixer to set the volume.

    0 arguments:
*/
void sound_AssignVolume(void)
{
    float real_Volume = (soundManager.volume/100.0f) * (float)MIX_MAX_VOLUME;
	 float real_MusicVolume = (soundManager.musicVolume/100.0f) * (float)MIX_MAX_VOLUME;
    soundManager.realVolume = (int)real_Volume;

    if(soundManager.enabled == 0)
    {
        return;
    }

    if(soundManager.mute == 0)
    {
        Mix_VolumeMusic((int)real_MusicVolume);
        //Mix_Volume(-1, soundManager.realVolume);
    }
    else
    {
        Mix_VolumeMusic(0);
        Mix_Volume(-1, 0);
    }

    return;
}

void sound_Report(void)
{
    printf("Sound report:\n");

    printf("loaded(%d)(%d) maxPerFrame(%d) volume (%d) mute(%d)\ntotalSoundsInserted(%d) totalSoundsPlaying(%d)\n",
           soundManager.soundsLoaded,
           soundManager.soundInfoCreated,
           soundManager.maxSoundsPerFrame,
           soundManager.volume,
           soundManager.mute,
           soundManager.totalSoundsInserted,
           soundManager.totalSoundsPlaying
    );

    return;
}

void sound_ReportChannelFinished(int channel)
{
    Sound_Info *soundRecord = &soundManager.playingSounds[channel];

    if(soundRecord->loops > 0)
    {
        sound_Call(soundRecord->sound, soundRecord->ID, SND_APPEND, soundRecord->volume, soundRecord->loops);
    }

    soundRecord->finished ++;
    soundManager.totalSoundsPlaying --;

    return;
}

/*
    Function: sound_Call

    Description -
    Places a sound into the sound manager so that is will be played on the next/current frame.
	 Returns 1 if sound has been added, 0 otherwise.

    5 arguments:
    Mix_Chunk *sound - Sound that is to be played
    int ID - The ID of given to the sound that is to play.
    int insertType - (SND_SINGLE) - Only allow one sound of the given ID to be playing,
                     (SND_DUPLICATE) - Allow many versions of the given ID to play.
                     (SND_DUPLICATE_SOFT) - Allow many versions of the given ID to play, but reduce sound if over a certain amount.
                     (SND_APPEND) - If the sound of given ID is playing, then append the number of loops (loops + 1) to this sound.

    int volume - The volume of the sound. 0 to MIX_MAX_VOLUME(128). -1 if unchanged.
    int loops - number of times the sound will be played.
*/
int sound_Call(Mix_Chunk *sound, int ID, int insertType, int volume, int loops)
{
    Sound_Info *soundRecord = NULL;

    if(soundManager.enabled == 0 || sound == NULL)
    {
        return 0;
    }

    switch(insertType)
    {
        default:
        case SND_DUPLICATE:

        break;

        case SND_DUPLICATE_SOFT:

        break;

        case SND_SINGLE:

        if(sound_CheckCollision(ID, sound) != NULL)
        {
            /*Exit early due to duplicate*/
            return 0;
        }

        break;

        case SND_APPEND:
        soundRecord = sound_CheckCollision(ID, sound);

        /*If the sound is already playing, update the amount of loops it should play*/
        if(soundRecord != NULL)
        {
            soundRecord->loops = 1 + loops;
            return 0;
        }

        break;
    }

    /*Check if there is space for the sound*/
    if(soundManager.totalSoundsInserted < soundManager.maxSoundsPerFrame)
    {
        /*Add in the sound to be played for the next frame*/
        soundInfo_Setup(&soundManager.insertSounds[soundManager.totalSoundsInserted], sound, ID, insertType, volume, -1, loops);

        soundManager.totalSoundsInserted ++;

        /*Set the global volume to the sound*/
        Mix_VolumeChunk(sound, soundManager.realVolume);

        return 1;

    }

    /*Exit due to max sounds reached*/
    return 0;
}

/*
    Function: sound_Play

    Description -
    Plays any sounds tracked by the sound manager.

    0 arguments:
*/
void sound_Play(void)
{
    int x = 0;
    int channel = 0;
    int numID = 0;

    Sound_Info *playingSound = NULL;

    if(soundManager.totalSoundsInserted <= 0 || soundManager.enabled == 0)
    {
        return;
    }

    /*Push all the sounds to be played in this frame into the mixer*/
    for(x = (soundManager.totalSoundsInserted- 1); x >= 0; x--)
    {
        channel = Mix_PlayChannel(soundManager.insertSounds[x].channel, soundManager.insertSounds[x].sound, soundManager.insertSounds[x].loops);

        if(soundManager.totalSoundsPlaying < soundManager.totalChannels && channel >= 0)
        {
            /*Track this sound*/
            playingSound = &soundManager.playingSounds[channel];
            soundInfo_Setup(playingSound, soundManager.insertSounds[x].sound, soundManager.insertSounds[x].ID, soundManager.insertSounds[x].playType, soundManager.insertSounds[x].volume, channel, 0);

            if(soundManager.mute == 1)
            {
            }
            else if(playingSound->volume > -1)
            {
                if(playingSound->playType == SND_DUPLICATE_SOFT)
                {
                    numID = sound_NumID(playingSound->ID);

                    if(numID > 2)
                    {
                        //printf("[%d]Reducing volume from %d to %d\n", numID, playingSound->volume, (int)(playingSound->volume * (2.5f/(numID))));
                        playingSound->volume =(int)(playingSound->volume * (2.5f/(numID)));
                    }
                }

                /*Make sure the channel has the same volume as the sound*/
                Mix_Volume(channel, playingSound->volume);
            }
            else
            {
                Mix_Volume(channel, MIX_MAX_VOLUME);
            }

            soundManager.totalSoundsPlaying ++;
        }
        else
        {
            //printf("Sound error: %s\n", Mix_GetError());
        }

        soundManager.totalSoundsInserted --;

        /*Mark the inserted sound as being clear, maybe not needed*/
        soundManager.insertSounds[x].finished = 1;
    }

    return;
}

/*
    Function: sound_Load

    Description -
    Returns a sound that has been loaded from file.

    If autoFree == A_FREE then the sound will be tracked by the sound manager which will
    release it when the program finishes.

    2 arguments:
    const char *soundName - The file name of the sound to load.
    int autoFree - If A_FREE then track the sound, otherwise set to M_FREE.
*/
Mix_Chunk *sound_Load(const char *soundName, int autoFree)
{
    Mix_Chunk *s_New = NULL;

    s_New = Mix_LoadWAV(soundName);

    if(s_New == NULL)
    {
        printf("Warning: Could not open sound (%s)\n", soundName);

        return NULL;
    }

    soundManager.soundsLoaded ++;

    if(autoFree == 1)
    {
        list_Push(&soundManager.soundList, s_New, 1);
    }

    return s_New;
}

/*
    Function: sound_Free

    Description -
    Releases the memory created for a sound. This
    sound should have been created by sound_Load.
    Updates the sound managers count of sounds.

    1 argument:
    Mix_Chunk *sound - The sound to free.
*/
int sound_Free(Mix_Chunk *sound)
{
    Mix_FreeChunk(sound);

    soundManager.soundsLoaded --;

    return 0;
}

/*
    Function: sound_ReleaseAll

    Description -
    Removes all the sounds that are tracked in the sound manager.

    0 arguments:
*/
void sound_ReleaseAll(void)
{
    while(soundManager.soundList != NULL)
    {
        sound_Free((Mix_Chunk *)soundManager.soundList->data);

        list_Pop(&soundManager.soundList);
    }

    while(soundManager.soundInfoList != NULL)
    {
        soundInfo_Delete((Sound_Info *)soundManager.soundInfoList->data);

        list_Pop(&soundManager.soundInfoList);
    }

    return;
}
