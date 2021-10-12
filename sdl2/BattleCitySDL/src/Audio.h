#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL_mixer.h>
#include "Types.h"

using namespace std;

struct Sound {
    Sound() : isPlayed(false) { }
    bool isPlayed;
    Mix_Chunk *sound;
};

class Audio {
public:
    ~Audio();
    void Init();

    void PlayChunk(SoundType type);
    void StopChunk(SoundType type);
    void StopAll();

private:
    Sound OnMove;
    Sound Fire;
    Sound Explosion;
    Sound Bonus;
    Sound GameStart;
    Sound GameOver;
};

#endif // AUDIO_H
