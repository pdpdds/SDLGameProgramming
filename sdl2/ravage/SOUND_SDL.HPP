#ifndef SOUND_H
#define SOUND_H

#include <SDL2/SDL_mixer.h>
#include "LIST.HPP"

enum SOUND_INSERT_STATES
{
    SND_SINGLE = 0,
    SND_DUPLICATE = 1,
    SND_DUPLICATE_SOFT = 2,
    SND_APPEND = 3
};

typedef struct sound_Info
{
    Mix_Chunk *sound;

    int ID;

    int loops;
    int channel;
    int volume;

    int playType;
    int finished;

} Sound_Info;

typedef struct sound_Manager
{
    struct list *soundInfoList; /*List of every sound information created*/
    struct list *soundList; /*List of every sound loaded that needs to be released once the program has finished*/
    int soundsLoaded; /*Total amount of sounds that have been loaded*/
    int soundInfoCreated; /*Total amount of sound information structures that have been created*/

    int totalChannels; /*Total mixing channels that are avaliable*/
    int maxSoundsPerFrame; /*Maximum amount of sounds to play at once*/

    Sound_Info *insertSounds; /*Array of sounds to start playing each frame*/
    Sound_Info *playingSounds; /*Array of sounds that are being played*/

    int totalSoundsInserted; /*Current amount of sounds that have been inserted so far in the frame*/
    int totalSoundsPlaying; /*Current amount of sounds that are playing*/

    int volume;
	 int musicVolume;
	 int realVolume;
	 int realMusicVolume;
    int mute;

    int enabled; /*If 0 then any sounds called will be rejected*/

} Sound_Manager;

Sound_Info *soundInfo_Create(Mix_Chunk *sound, int ID, int playType, int volume, int channel, int loops);

void soundInfo_Setup(Sound_Info *soundInfo, Mix_Chunk *sound, int ID, int playType, int volume, int channel, int loops);

void soundInfo_Delete(Sound_Info *soundInfo);


void sound_ReportChannelFinished(int channel);


int sound_Init(int maxSounds, int maxChannels, int volume, int enabled);

void sound_Clean(void);

int sound_NumID(int ID);

Sound_Info *sound_CheckCollision(int ID, Mix_Chunk *sound);

int sound_ToggleMute(void);

void sound_Mute(void);

void sound_Unmute(void);

void sound_Stop(int channel);

void sound_Pause(int channel);

void sound_Resume(int channel);

int sound_ChangeVolume(int changePercent);

int sound_SetVolume(int volumePercent);

int sound_SetMusicVolume(int volumePercent);

void sound_AssignVolume(void);

void sound_Report(void);

int sound_Call(Mix_Chunk *sound, int ID, int insertType, int volume, int loops);

void sound_Play(void);

Mix_Chunk *sound_Load(const char *soundName, int autoFree);

int sound_Free(Mix_Chunk *sound);

void sound_ReleaseAll(void);

#endif
