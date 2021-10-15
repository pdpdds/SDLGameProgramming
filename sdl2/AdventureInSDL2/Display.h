
/* Display.h
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

#include "Res.h"

namespace AISDL {

/**
 * Interface for scene manager.
 * @author Michael Imamura
 */
class Display {
public:
	Display();

public:
	SDL_Surface *NewAlphaSurface(int w, int h);
	SDL_Texture *LoadTexture(const std::string &filename);
	void RenderTexture(SDL_Texture *texture, int x, int y);
	void RenderTexture(SDL_Texture *texture, int x, int y, int w, int h);

public:
	void SetLowRes();
	void SetHighRes();
	void ToggleFullscreen();

public:
	// These are wrapped with deleters to ensure proper destruction order.
	// A more complete engine would wrap them in full classes.
	std::shared_ptr<SDL_Window> windowPtr;
	std::shared_ptr<SDL_Renderer> rendererPtr;
public:
	Res res;
	// Convenience pointers.
	SDL_Window *window;
	SDL_Renderer *renderer;
};

}  // namespace AISDL

