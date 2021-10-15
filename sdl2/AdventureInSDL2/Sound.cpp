
/* Ttf.cpp
 *
 * Copyright (C) 2013 Michael Imamura
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

#include "StdAfx.h"

#include "Exception.h"

#include "Sound.h"

namespace AISDL {

namespace {
	bool mixerInitialized = false;
	std::vector<std::shared_ptr<Sound>> playingSounds(8);

	void OnMixChannelFinished(int channel)
	{
		playingSounds[channel].reset();
		//SDL_Log("Channel %d has stopped playing.", channel);
	}
}

/**
 * Constructor.  This should never be called directly; use Load() instead.
 * @param chunk The loaded chunk (may not be @c nullptr).
 */
Sound::Sound(Mix_Chunk *chunk) :
	std::enable_shared_from_this<Sound>(),
	chunk(chunk)
{
	if (!mixerInitialized) {
		Mix_ChannelFinished(OnMixChannelFinished);
		mixerInitialized = true;
	}
}

Sound::~Sound()
{
	// playingSounds will hold references to this sound as long as they're
	// still playing, so this will only get called when the sound is not
	// being played on any channel.
	Mix_FreeChunk(chunk);
}

/**
 * Load a sound.
 * @param filename The sound file (only .wav files are supported).
 */
std::shared_ptr<Sound> Sound::Load(const std::string &filename)
{
	//FIXME: Don't assume the file is a .wav file.
	Mix_Chunk *chunk = Mix_LoadWAV(filename.c_str());
	if (!chunk) {
		throw Exception("Failed to load: " + filename, Mix_GetError());
	}
	return std::make_shared<Sound>(chunk);
}

void Sound::Play()
{
	SDL_LockAudio();
	int channel = Mix_PlayChannel(-1, chunk, 0);
	if (channel == -1) {
		SDL_Log("No available channel for sound.");
	}
	else {
		playingSounds.reserve(channel + 1);
		playingSounds[channel] = shared_from_this();
		//SDL_Log("Sound is playing on channel %d", channel);
	}
	SDL_UnlockAudio();
}

}  // namespace AISDL
