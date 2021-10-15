
/* SpriteMap.cpp
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

#include <SDL2/SDL_image.h>

#include "Display.h"
#include "Exception.h"

#include "SpriteMap.h"

namespace AISDL {

/**
 * Constructor.
 *
 * @note This instance will take ownership of the texture and will be
 *       responsible for destroying it.
 *
 * @param display The target display.
 * @param texture The sprite texture.
 * @param w The width of each sprite in the texture.
 * @param h The height of each sprite in the texture.
 * @throws Exception The sprite width or height is too big for the texture.
 */
SpriteMap::SpriteMap(Display &display, SDL_Texture *texture, int w, int h) :
	display(display), texture(texture), w(w), h(h)
{
	int texW, texH;
	SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);

	perRow = texW / w;
	if (perRow == 0) {
		SDL_DestroyTexture(texture);
		texture = nullptr;
		std::ostringstream oss;
		oss << "Sprite width is " << w <<
			" but texture width is only " << texW;
		throw Exception(oss.str());
	}

	numSprites = texH * perRow / h;
	if (numSprites == 0) {
		SDL_DestroyTexture(texture);
		texture = nullptr;
		std::ostringstream oss;
		oss << "Sprite height is " << h <<
			" but texture height is only " << texH;
		throw Exception(oss.str());
	}
}

SpriteMap::~SpriteMap()
{
	if (texture) {
		SDL_DestroyTexture(texture);
	}
}

/**
 * Loads a sprite map from a file.
 * @param display The target display.
 * @param filename The filename, relative to the current directory.
 * @param w The width of each sprite in the texture.
 * @param h The height of each sprite in the texture.
 * @throws Exception The sprite width or height is too big for the texture.
 */
std::shared_ptr<SpriteMap> SpriteMap::Load(Display &display,
	const std::string &filename, int w, int h)
{
	SDL_Surface *surface = IMG_Load(filename.c_str());
	if (!surface) {
		throw Exception("Unable to load sprite map: " + filename,
			IMG_GetError());
	}

	SDL_Texture *texture = SDL_CreateTextureFromSurface(
		display.renderer, surface);
	if (!texture) {
		throw Exception("Failed to create texture for sprite map: " + filename,
			SDL_GetError());
	}

	return std::make_shared<SpriteMap>(display, texture, w, h);
}

/**
 * Render a single sprite at the specific location.
 * @param x The destination X coordinate.
 * @param y The destination Y coordinate.
 * @param index The index of sprite.
 */
void SpriteMap::Render(int x, int y, int index)
{
	if (index < 0 || index >= numSprites) {
		return;
	}

	SDL_Rect srcRect = {
		(index % perRow) * w, (index / perRow) * h,
		w, h
	};
	SDL_Rect destRect = { x, y, w, h };

	if (SDL_RenderCopy(display.renderer, texture, &srcRect, &destRect) < 0) {
		throw Exception(SDL_GetError());
	}
}

}  // namespace AISDL
