
#ifndef AH_SOUND
#define AH_SOUND

#include "SOUND_SDL.HPP"

#define MAXVOICES       8     // Number of SFX voices supported.
#define MINPRIORITY     3     // Least possible WAV priority.

// How does AlphaHelix defines the samples ?
// Well here's the structure:
#pragma pack (1)
struct Sample {
   Mix_Chunk *sample;          // Definition of sample.
   long     volume;           // Volume of sample (max = 64).
   long     priority;         // Sample priority.
};

#pragma pack (4)
//Position of data in WavePool(WP)
// Array must be of type long.
#define WP_NWAVES             0
#define WP_STARTOFINDEX       1
class WavePool {
public:
	long     nsamples;         // Number of samples loaded.
   Sample   *sample;          // Pointer to sample array.
   void     load(char *f);
   void     unload(void);
   void     play(int n);
};


void s_loadmod(char *f);
void s_unloadmod(void);
void s_startmod(void);
void s_stopmod(void);
char *s_modname(void);
void s_setmodvol(int volume);
int s_getmodvol(void);
void s_setwavvol(int volume);
int s_getmixfreq(void);
char *s_drivername(void);
void s_update(void);
void initsound();
void shutsound(void);

#endif


