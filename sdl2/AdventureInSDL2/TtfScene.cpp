
/* TtfScene.cpp
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

#include "TtfScene.h"

namespace AISDL {

TtfScene::TtfScene(Director &director, Display &display) :
	SUPER(director, display, "SDL_ttf")
{
}

TtfScene::~TtfScene()
{
}

void TtfScene::OnWalkOffEdgeLeft(std::shared_ptr<Player> player)
{
	player->SetPos(511, player->GetPosY());
}

void TtfScene::OnWalkOffEdgeRight(std::shared_ptr<Player> player)
{
	player->SetPos(-31, player->GetPosY());
}

void TtfScene::OnInteract()
{
	convo->Next();
}

void TtfScene::OnAction()
{
	if (!ttfTxt->NextPage(PagedTextDecor::Anim::FLING_UP)) {
		display.res.doorSound->Play();
		director.RequestNextScene();
	}
}

void TtfScene::OnCancel()
{
	ttfTxt->PrevPage();
}

void TtfScene::Preload()
{
	SUPER::Preload();

	level = Level::Load(display.res.resDir + "/levels/ttf");
	levelDecor.reset(new LevelDecor(display, level, display.res.interiorTile));

	const std::string dir = display.res.resDir + "/text/ttf/";
	ttfTxt.reset(new PagedTextDecor(display, display.res.bodyFont,
		ResStr::Load(dir + "ttf.txt"), 864));
	convo.reset(new Conversation(ResStr::Load(dir + "dialog.txt")));
}

void TtfScene::Reload()
{
	SUPER::Reload();

	level->Reload();
}

void TtfScene::Reset()
{
	SUPER::Reset();

	ttfTxt->FirstPage(PagedTextDecor::Anim::FLING_UP);

	auto player = director.GetMainPlayer();
	player->SetPos(40, 320);
	convo->Start(player);
}

void TtfScene::Advance(Uint32 lastTick, Uint32 tick)
{
	SUPER::Advance(lastTick, tick);

	ttfTxt->Advance(tick);
}

void TtfScene::RenderContent()
{
	SDL_SetRenderDrawColor(display.renderer, 0x00, 0x00, 0x00, 0xff);
	SDL_RenderClear(display.renderer);

	display.SetLowRes();
	levelDecor->Render();

	SUPER::RenderContent();

	display.SetHighRes();
	ttfTxt->Render(80, 40);
}

}  // namespace AISDL
