
/* LevelDecor.cpp
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

#include "Layer.h"
#include "Level.h"
#include "SpriteMap.h"

#include "LevelDecor.h"

#ifdef min
#	undef min
#endif

namespace AISDL {

LevelDecor::LevelDecor(Display &display, std::shared_ptr<Level> level,
	std::shared_ptr<SpriteMap> defaultSprite) :
	display(display),
	level(std::move(level)), defaultSprite(std::move(defaultSprite))
{
}

void LevelDecor::RenderLayer(const Layer &layer, size_t width, size_t height)
{
	if (!layer.IsVisible()) return;

	for (size_t y = 0; y < height; y++) {
		auto iter = layer.GetRow(y);
		for (size_t x = 0; x < width; x++) {
			int tileIdx = *iter;
			if (tileIdx) {
				defaultSprite->Render(x * 32, y * 32, tileIdx);
			}
			++iter;
		}
	}
}

/**
 * Draw all layers.
 */
void LevelDecor::Render()
{
	//TODO: Handle offset and viewport.

	size_t width = std::min<size_t>(16, level->GetWidth());
	size_t height = std::min<size_t>(12, level->GetHeight());

	level->ForEachLayer([&](const Layer &layer) {
		RenderLayer(layer, width, height);
	});
}

/**
 * Draw a single layer.
 * @param index The layer index.
 */
void LevelDecor::RenderLayer(size_t index)
{
	//TODO: Handle offset and viewport.

	size_t width = std::min<size_t>(16, level->GetWidth());
	size_t height = std::min<size_t>(12, level->GetHeight());

	RenderLayer(level->GetLayer(index), width, height);
}

}
