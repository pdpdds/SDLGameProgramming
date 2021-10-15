
/* IntroScene.cpp
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

#include "Conversation.h"
#include "Level.h"
#include "LevelDecor.h"
#include "PagedTextDecor.h"
#include "Player.h"
#include "ResStr.h"
#include "Sound.h"
#include "SpriteMap.h"

#include "IntroScene.h"

namespace AISDL {

IntroScene::IntroScene(Director &director, Display &display) :
	SUPER(director, display, "Introduction"),
	phase(0)
{
}

IntroScene::~IntroScene()
{
}

void IntroScene::OnWalkOffEdgeLeft(std::shared_ptr<Player> player)
{
	player->SetPos(511, player->GetPosY());
}

void IntroScene::OnWalkOffEdgeRight(std::shared_ptr<Player> player)
{
	player->SetPos(-31, player->GetPosY());
}

void IntroScene::OnInteract()
{
	convo->Next();
}

void IntroScene::OnAction()
{
	switch (phase) {
	case 0:
		if (!introTxt->NextPage(PagedTextDecor::Anim::TYPEWRITER)) {
			revealSound->Play();
			fadeTs = SDL_GetTicks();
			phase++;
		}
		break;
	case 1:
		break;
	case 2:
		if (!aboutTxt->NextPage(PagedTextDecor::Anim::FLING_UP)) {
			display.res.doorSound->Play();
			director.RequestNextScene();
		}
		break;
	}
}

void IntroScene::Preload()
{
	SUPER::Preload();

	std::string dir;

	dir = display.res.resDir + "/sounds/intro/";
	revealSound = Sound::Load(dir + "old-music-box-1.wav");

	level = Level::Load(display.res.resDir + "/levels/intro");
	levelDecor.reset(new LevelDecor(display, level, display.res.interiorTile));

	dir = display.res.resDir + "/text/intro/";
	introTxt.reset(new PagedTextDecor(display, display.res.pixelFont,
		ResStr::Load(dir + "intro.txt"), 432, true));
	aboutTxt.reset(new PagedTextDecor(display, display.res.bodyFont,
		ResStr::Load(dir + "about.txt"), 864));
	convo.reset(new Conversation(ResStr::Load(dir + "dialog.txt")));
}

void IntroScene::Reload()
{
	SUPER::Reload();

	level->Reload();
}

void IntroScene::Reset()
{
	SUPER::Reset();

	phase = 0;

	fadeAlpha = 255;
	introTxt->FirstPage(PagedTextDecor::Anim::TYPEWRITER);

	auto player = director.GetMainPlayer();
	player->SetPos(40, 320);
	convo->Start(player);
}

void IntroScene::Advance(Uint32 lastTick, Uint32 tick)
{
	SUPER::Advance(lastTick, tick);

	switch (phase) {
	case 0:
		introTxt->Advance(tick);
		break;

	case 1:
		{
			Uint32 timeDiff = tick - fadeTs;
			if (timeDiff > 1000) {
				aboutTxt->FirstPage(PagedTextDecor::Anim::FLING_UP);
				phase++;
			}
			else {
				fadeAlpha = 255 - (timeDiff * 255 / 1000);
			}
		}
		break;

	case 2:
		aboutTxt->Advance(tick);
		break;
	}
}

void IntroScene::RenderContent()
{
	SDL_SetRenderDrawColor(display.renderer, 0x00, 0x00, 0x00, 0xff);
	SDL_RenderClear(display.renderer);

	if (phase >= 1) {
		display.SetLowRes();
		levelDecor->Render();
	}

	SUPER::RenderContent();

	display.SetLowRes();

	if (phase <= 1) {
		SDL_SetRenderDrawBlendMode(display.renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(display.renderer, 0, 0, 0, fadeAlpha);
		SDL_Rect fillRect = { 0, 0, 512, 384 };
		SDL_RenderFillRect(display.renderer, &fillRect);
	}

	// Draw the background behind the text.
	/*
	SDL_SetRenderDrawColor(display.renderer, 0x00, 0xff, 0x00, 0x1f);
	SDL_Rect textBg = {
		35, 35,
		432 + 10, introTxt->MeasureHeight() + 10
	};
	SDL_RenderFillRect(display.renderer, &textBg);
	*/

	switch (phase) {
	case 0:
	case 1:
		display.SetLowRes();
		introTxt->Render(40, 40, fadeAlpha);
		break;
	case 2:
		display.SetHighRes();
		aboutTxt->Render(80, 80);
		break;
	}
}

}  // namespace AISDL

