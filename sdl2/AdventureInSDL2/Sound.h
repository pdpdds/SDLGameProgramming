
/* Sound.h
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

#pragma once

#include <SDL2/SDL_mixer.h>

namespace AISDL {

class Display;

/**
 * Wrapper for SDL_mixer sound chunks.
 * @author Michael Imamura
 */
class Sound : public std::enable_shared_from_this<Sound> {
	friend Mix_Chunk *operator*(const Sound &sound);
public:
	Sound(Mix_Chunk *chunk);
	~Sound();

public:
	static std::shared_ptr<Sound> Load(const std::string &filename);

public:
	void Play();

private:
	Mix_Chunk *chunk;
};

inline Mix_Chunk *operator*(const Sound &sound)
{
	return sound.chunk;
}

}  // namespace AISDL
