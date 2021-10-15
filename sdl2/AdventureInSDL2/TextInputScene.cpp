
/* TextInputScene.cpp
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

#include "utf8/utf8.h"

#include "Conversation.h"
#include "FmtTextDecor.h"
#include "Level.h"
#include "LevelDecor.h"
#include "PagedTextDecor.h"
#include "Player.h"
#include "ResStr.h"
#include "SpriteMap.h"

#include "TextInputScene.h"

namespace AISDL {

TextInputScene::TextInputScene(Director &director, Display &display) :
	SUPER(director, display, "Introduction")
{
}

TextInputScene::~TextInputScene()
{
}

void TextInputScene::UpdatePlayerName()
{
	playerNameTxt->SetText(playerName);

	// Note when the last input event occurred so we can keep the cursor
	// visible.
	lastInputTs = SDL_GetTicks();
}

void TextInputScene::OnKeyDown(SDL_KeyboardEvent &evt)
{
	if (evt.keysym.sym == SDLK_BACKSPACE) {
		if (!playerName.empty()) {
			// Find the last UTF-8 character sequence and remove it.
			auto iter = playerName.end();
			utf8::prior(iter, playerName.begin());
			playerName.erase(iter, playerName.end());

			UpdatePlayerName();
		}
	}
}

/**
 * Handle when the OS-native text input appends text.
 * @param evt The event.
 */
void TextInputScene::OnTextInput(SDL_TextInputEvent &evt)
{
	playerName += evt.text;
	UpdatePlayerName();
}

/**
 * Handle when the OS-native composition system changes.
 * @param evt The event.
 */
void TextInputScene::OnTextEditing(SDL_TextEditingEvent &evt)
{
	//TODO
	// This can get a bit... interesting, so we're leaving it out for now.
	// See: http://wiki.libsdl.org/Tutorials/TextInput
}

void TextInputScene::OnWalkOffEdgeLeft(std::shared_ptr<Player> player)
{
	player->SetPos(511, player->GetPosY());
}

void TextInputScene::OnWalkOffEdgeRight(std::shared_ptr<Player> player)
{
	player->SetPos(-31, player->GetPosY());
}

void TextInputScene::OnInteract()
{
	if (phase == 0) {
		convo->Next();
	}
}

void TextInputScene::OnAction()
{
	switch (phase) {
	case 0:
		if (!inputTxt->NextPage(PagedTextDecor::Anim::FLING_UP)) {
			phase++;

			fadeTs = SDL_GetTicks();

			introTxt->FirstPage(PagedTextDecor::Anim::TYPEWRITER);

			auto player = director.GetMainPlayer();
			player->Silence();
			player->SetDirection(Player::Direction::DOWN);
			player->SetPos(256 - 16, 384 / 3);  // Centered on the screen.

			SDL_Log("Starting Unicode text input.");
			SDL_StartTextInput();
		}
		break;
	case 1:
		director.RequestNextScene();
		break;
	}
}

void TextInputScene::Preload()
{
	SUPER::Preload();

	level = Level::Load(display.res.resDir + "/levels/intro");
	levelDecor.reset(new LevelDecor(display, level, display.res.interiorTile));

	const std::string dir = display.res.resDir + "/text/textinput/";
	convo.reset(new Conversation(ResStr::Load(dir + "dialog.txt")));
	inputTxt.reset(new PagedTextDecor(display, display.res.bodyFont,
		ResStr::Load(dir + "input.txt"), 864));
	introTxt.reset(new PagedTextDecor(display, display.res.pixelFont,
		ResStr::Load(dir + "intro.txt"), 432));
}

void TextInputScene::Reload()
{
	SUPER::Reload();

	level->Reload();
}

void TextInputScene::Reset()
{
	SUPER::Reset();

	phase = 0;

	//FIXME: The current pixelFont does not have some of the Unicode characters
	//       we'd like to have.
	playerName = "Ulric of Cogito";
	playerNameTxt.reset(new FmtTextDecor(display, display.res.bodyFont,
		playerName, 512));
	UpdatePlayerName();

	inputTxt->FirstPage(PagedTextDecor::Anim::FLING_UP);

	auto player = director.GetMainPlayer();
	player->SetPos(40, 320);
	convo->Start(player);
}

void TextInputScene::Cleanup()
{
	playerNameTxt.reset();

	SDL_Log("Stopping Unicode text input.");
	SDL_StopTextInput();
}

void TextInputScene::HandleEvent(SDL_Event &evt)
{
	switch (evt.type) {
	case SDL_KEYDOWN:
		OnKeyDown(evt.key);
		break;
	case SDL_TEXTINPUT:
		OnTextInput(evt.text);
		break;
	case SDL_TEXTEDITING:
		OnTextEditing(evt.edit);
		break;
	}
}

void TextInputScene::Advance(Uint32 lastTick, Uint32 tick)
{
	SUPER::Advance(lastTick, tick);

	switch (phase) {
	case 0:
		inputTxt->Advance(tick);
		break;
	case 1:
		{
			Uint32 timeDiff = tick - fadeTs;
			if (timeDiff <= 1000) {
				fadeAlpha = timeDiff * 127 / 1000;
			}
		}
		introTxt->Advance(tick);
		playerNameCursor = ((lastInputTs - tick) % 1000) < 500;
		break;
	}
}

void TextInputScene::RenderContent()
{
	SDL_SetRenderDrawColor(display.renderer, 0x00, 0x00, 0x00, 0xff);
	SDL_RenderClear(display.renderer);

	display.SetLowRes();
	levelDecor->Render();

	if (phase == 1) {
		SDL_SetRenderDrawBlendMode(display.renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(display.renderer, 0, 0, 0, fadeAlpha);
		SDL_Rect fillRect = { 0, 0, 512, 384 };
		SDL_RenderFillRect(display.renderer, &fillRect);
	}

	SUPER::RenderContent();

	display.SetLowRes();

	switch (phase) {
	case 0:
		display.SetHighRes();
		inputTxt->Render(80, 40);
		break;
	case 1:
		{
			int introTxtWidth = introTxt->MeasureWidth();
			introTxt->Render(256 - (introTxtWidth / 2), 40);

			// Draw the text input area.
			display.SetHighRes();
			int playerNameTxtWidth = playerNameTxt->MeasureWidth();
			playerNameTxt->Render(512 - (playerNameTxtWidth / 2), 160, 255,
				playerNameCursor);
		}
		break;
	}

}

}  // namespace AISDL

