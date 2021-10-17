//
// Created by efry on 22/11/2017.
//

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "audio.h"
#include "music.h"

#define AUDIO_DESIRED_SAMPLE_RATE 22050
#define AUDIO_DESIRED_NUM_CHANNELS 1

int audio_sample_rate = 0;
int audio_num_channels = 1;

void audio_init()
{
    if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
    {
        printf("ERROR: intialising audio!\n");
        return;
    }

    if( Mix_OpenAudio( AUDIO_DESIRED_SAMPLE_RATE, AUDIO_S16LSB, AUDIO_DESIRED_NUM_CHANNELS, 1024*2 ) == -1 )
    {
        printf("ERROR: Opening audio mixer!\n");
        return;
    }

    // get and print the audio format in use
    int numtimesopened;
    Uint16 format;
    numtimesopened=Mix_QuerySpec(&audio_sample_rate, &format, &audio_num_channels);
    if(!numtimesopened) {
        printf("Mix_QuerySpec: %s\n",Mix_GetError());
    }
    else {
        char *format_str="Unknown";
        switch(format) {
            case AUDIO_U8: format_str="U8"; break;
            case AUDIO_S8: format_str="S8"; break;
            case AUDIO_U16LSB: format_str="U16LSB"; break;
            case AUDIO_S16LSB: format_str="S16LSB"; break;
            case AUDIO_U16MSB: format_str="U16MSB"; break;
            case AUDIO_S16MSB: format_str="S16MSB"; break;
        }
        printf("audio_init(): opened=%d times  frequency=%dHz  format=%s  channels=%d\n",
               numtimesopened, audio_sample_rate, format_str, audio_num_channels);

        if(format != AUDIO_S16LSB)
        {
            printf("WARNING: AUDIO_S16LSB required.\n");
        }
    }

    music_init();
}

void audio_shutdown()
{
    Mix_CloseAudio();
}