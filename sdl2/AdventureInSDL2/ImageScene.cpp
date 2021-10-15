
/* ImageScene.cpp
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

#include "Conversation.h"
#include "Exception.h"
#include "Level.h"
#include "LevelDecor.h"
#include "PagedTextDecor.h"
#include "Player.h"
#include "ResStr.h"
#include "Sound.h"
#include "SpriteMap.h"

#include "ImageScene.h"

namespace AISDL {

ImageScene::ImageScene(Director &director, Display &display) :
	SUPER(director, display, "SDL_image"),
	cavePainting(nullptr)
{
}

ImageScene::~ImageScene()
{
	if (cavePainting) {
		SDL_DestroyTexture(cavePainting);
	}
}

SDL_Texture *ImageScene::LoadPainting(const std::string &filename)
{
	std::string fn = display.res.resDir;
	fn += "/images/";
	fn += filename;

	SDL_Surface *surface = IMG_Load(fn.c_str());
	if (!surface) {
		throw Exception("Failed to load image: " + filename, IMG_GetError());
	}

	SDL_Texture *retv = SDL_CreateTextureFromSurface(display.renderer, surface);
	if (!retv) {
		throw Exception("Failed to create texture", SDL_GetError());
	}

	return retv;
}

void ImageScene::OnWalkOffEdgeLeft(std::shared_ptr<Player> player)
{
	player->SetPos(511, player->GetPosY());
}

void ImageScene::OnWalkOffEdgeRight(std::shared_ptr<Player> player)
{
	player->SetPos(-31, player->GetPosY());
}

void ImageScene::OnInteract()
{
	convo->Next();
}

void ImageScene::OnAction()
{
	if (showPaintings) {
		display.res.doorSound->Play();
		director.RequestNextScene();
	}
	else {
		showPaintings = true;
		flashTs = SDL_GetTicks();
		flashAlpha = 255;
	}
}

void ImageScene::Preload()
{
	SUPER::Preload();

	cavePainting = LoadPainting("cave_bluelarge.png");
	animePainting = LoadPainting("banneer.png");
	sunsetPainting = LoadPainting("sunsetintheswamp.png");

	level = Level::Load(display.res.resDir + "/levels/image");
	levelDecor.reset(new LevelDecor(display, level, display.res.interiorTile));

	const std::string dir = display.res.resDir + "/text/image/";
	convo.reset(new Conversation(ResStr::Load(dir + "dialog.txt")));
}

void ImageScene::Reload()
{
	SUPER::Reload();

	level->Reload();
}

void ImageScene::Reset()
{
	SUPER::Reset();

	showPaintings = false;

	auto player = director.GetMainPlayer();
	player->SetPos(40, 320);
	convo->Start(player);
}

void ImageScene::Advance(Uint32 lastTick, Uint32 tick)
{
	SUPER::Advance(lastTick, tick);

	if (showPaintings) {
		Uint32 past = tick - flashTs;
		if (past < 500) {
			float pos = static_cast<float>(past) / 500;
			flashAlpha = 255 - static_cast<int>(255.0f * powf(pos, 2));
		}
		else {
			flashAlpha = 0;
		}
	}
}

void ImageScene::RenderContent()
{
	SDL_SetRenderDrawColor(display.renderer, 0x00, 0x00, 0x00, 0xff);
	SDL_RenderClear(display.renderer);

	// Draw the background layer.
	display.SetLowRes();
	levelDecor->RenderLayer(0);

	// Draw the paintings.
	if (showPaintings) {
		display.SetHighRes();
		DrawPainting(cavePainting, 2, 1, 3, 3);
		DrawPainting(animePainting, 3, 5, 4, 3);
		DrawPainting(sunsetPainting, 9, 2, 5, 5);
	}

	// Draw the foreground layer.
	display.SetLowRes();
	levelDecor->RenderLayer(1);

	SUPER::RenderContent();
}

void ImageScene::DrawPainting(SDL_Texture *texture, int x, int y, int w, int h)
{
	SDL_Rect destRect = { x * 64, y * 64, w * 64, h * 64 };
	SDL_RenderCopy(display.renderer, texture, nullptr, &destRect);

	if (flashAlpha > 0) {
		SDL_SetRenderDrawColor(display.renderer, 0xff, 0xff, 0xff, flashAlpha);
		SDL_SetRenderDrawBlendMode(display.renderer, SDL_BLENDMODE_BLEND);
		SDL_RenderFillRect(display.renderer, &destRect);
	}
}

}  // namespace AISDL
