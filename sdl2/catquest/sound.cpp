#include "sound.h"

int Sound::channelCount = 0;

Sound::Sound(std::string url) {
	track = NULL;
	change(url);
}
Sound::~Sound() {
	free();
}
void Sound::free() {
	if(track != NULL) {
		Mix_FreeChunk(track);
		track = NULL;
	}
}
void Sound::change(std::string url) {
	free();
	track = Mix_LoadWAV(url.c_str());
	if(track == NULL) {
		printf("Failed to load sound effect (%s)! SDL_mixer Error: %s\n", url.c_str(), Mix_GetError());
		ok = false;
	} else {
		channel = channelCount++;
		ok = true;
	}
}
void Sound::play() {
	Mix_PlayChannel(channel, track, 0);
}
void Sound::stop() {
	Mix_HaltChannel(channel);
}

Music::Music(std::string url) {
	track = NULL;
	change(url);
}
Music::~Music() {
	free();
}
void Music::free() {
	if(track != NULL) {
		Mix_FreeMusic(track);
		track = NULL;
	}
}
void Music::play() {
	Mix_PlayMusic(track, -1);
}
void Music::change(std::string url) {
	free();
	track = Mix_LoadMUS(url.c_str());
	if(track == NULL) {
		printf("Failed to load music (%s)! SDL_mixer Error: %s\n", url.c_str(), Mix_GetError());
		ok = false;
	} else {
		ok = true;
	}
}
void Music::pause() {
	Mix_PauseMusic();
}
void Music::resume() {
	Mix_ResumeMusic();
}
void Music::stop() {
	Mix_HaltMusic();
}
bool Music::isPlaying() {
	return Mix_PlayingMusic() != 0;
}
bool Music::isPaused() {
	return Mix_PausedMusic() != 0;
}
