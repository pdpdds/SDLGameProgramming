#ifndef __SOUND_H__
#define __SOUND_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string>
#include "gameobject.h"

class Sound : public GameObject {
	Mix_Chunk* track;
	int channel;
	static int channelCount;
public:
	Sound(std::string);
	~Sound();
	void change(std::string);
	void free();

	void play();
	void stop();
};

class Music : public GameObject {
	Mix_Music* track;
public:
	Music(std::string);
	~Music();
	void change(std::string);
	void free();

	void play();
	static void pause();
	static void resume();
	static void stop();
	static bool isPlaying();
	static bool isPaused();
};

#endif
