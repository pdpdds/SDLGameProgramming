#include "Audio.h"
#include <iostream>

using namespace std;

Audio::~Audio() {
    Mix_CloseAudio();
}

void Audio::Init() {
	if (Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 1536) != 0) {
		cout << "- Nie mozna zainicjalizowac biblioteki SDL_Mixer\n";
		return;
	}

    Mix_Volume(0, MIX_MAX_VOLUME / 2);
    Mix_Volume(1, MIX_MAX_VOLUME / 2);
    Mix_Volume(2, MIX_MAX_VOLUME / 4);
    Mix_Volume(3, MIX_MAX_VOLUME / 2);
    Mix_Volume(4, MIX_MAX_VOLUME / 2);
    Mix_Volume(5, MIX_MAX_VOLUME / 2);
    Mix_Volume(6, MIX_MAX_VOLUME / 2);

    OnMove.sound = Mix_LoadWAV_RW(SDL_RWFromFile("background.ogg", "rb"), 1);
    Fire.sound = Mix_LoadWAV_RW(SDL_RWFromFile("data/sounds/fire.ogg", "rb"), 1);
    Explosion.sound = Mix_LoadWAV_RW(SDL_RWFromFile("data/sounds/explosion.ogg", "rb"), 1);
    Bonus.sound = Mix_LoadWAV_RW(SDL_RWFromFile("data/sounds/bonus.ogg", "rb"), 1);
    GameStart.sound = Mix_LoadWAV_RW(SDL_RWFromFile("data/sounds/gamestart.ogg", "rb"), 1);
    GameOver.sound = Mix_LoadWAV_RW(SDL_RWFromFile("data/sounds/gameover.ogg", "rb"), 1);

    if (OnMove.sound == NULL || Fire.sound == NULL || Explosion.sound == NULL || Bonus.sound == NULL || GameStart.sound == NULL || GameOver.sound == NULL) {
        cout << "- Unable to load music\n"; exit(1);
    }
    else {
        cout << "- Load music 'data/sounds/*.ogg'" << endl;
    }
}

void Audio::PlayChunk(SoundType type) {
    switch (type) {
    case SOUND_ONMOVE:
        if (OnMove.isPlayed == false) {
            Mix_PlayChannel(0, OnMove.sound, -1);
            OnMove.isPlayed = true;
        }
        break;
    case SOUND_FIRE:
        Mix_HaltChannel(1);
        Mix_PlayChannel(1, Fire.sound, 0);
        break;
    case SOUND_EXPLOSION:
        Mix_HaltChannel(2);
        Mix_PlayChannel(2, Explosion.sound, 0);
        break;
    case SOUND_BONUS:
        Mix_HaltChannel(3);
        Mix_PlayChannel(3, Bonus.sound, 0);
        break;
    case SOUND_GAMESTART:
        Mix_HaltChannel(4);
        Mix_PlayChannel(4, GameStart.sound, 0);
        break;
    case SOUND_GAMEOVER:
        Mix_HaltChannel(5);
        Mix_PlayChannel(5, GameOver.sound, 0);
        break;
    case SOUND_DIE:
        Mix_HaltChannel(6); 
        Mix_PlayChannel(6, Explosion.sound, 0);
    }
}

void Audio::StopAll() {
    for (int i = 0; i < 7; ++i)
        Mix_HaltChannel(i);
}

void Audio::StopChunk(SoundType type) {
    switch (type) {
    case SOUND_ONMOVE:
        Mix_HaltChannel(0);
        OnMove.isPlayed = false;
        break;
    }
}
