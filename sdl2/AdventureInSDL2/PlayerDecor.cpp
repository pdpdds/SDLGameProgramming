
/* PlayerDecor.cpp
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

#include "Display.h"
#include "FmtTextDecor.h"
#include "Player.h"
#include "Sound.h"
#include "SpriteMap.h"

#include "PlayerDecor.h"

namespace AISDL {

namespace {
	/// Sprite indexes for each frame of animation.
	int WALK_ANIM[4][8] = {
		{  4,  5,  6,  7,  4,  5,  6,  7 },  // Up
		{ 16, 17, 18, 19, 20, 21, 22, 23 },  // Right
		{  0,  0,  0,  0,  0,  0,  0,  0 },  // Down
		{ 15, 14, 13, 12, 11, 10,  9,  8 },  // Left
	};
}

/**
 * Constructor.
 * @param display The target display.
 * @param player The player model.
 * @param sprite The spritemap to use to render the player.
 */
PlayerDecor::PlayerDecor(Display &display, std::shared_ptr<Player> player,
	std::shared_ptr<SpriteMap> sprite) :
	display(display), player(player), sprite(sprite),
	balloonVisible(false), balloonBounceY(0),
	balloon(new FmtTextDecor(display, display.res.talkFont, "", 512))
{
}

PlayerDecor::~PlayerDecor()
{
}

void PlayerDecor::Advance(Uint32 tick)
{
	auto p = player.lock();
	if (!p) return;

	//TODO: Animate walking.

	const std::string &balloonText = p->GetBalloonText();
	balloonVisible = !balloonText.empty();
	if (balloonVisible) {
		// Check if balloon text changed.
		if (prevBalloonText != balloonText) {
			prevBalloonText = balloonText;
			balloon->SetText("^0" + balloonText);

			display.res.talkSound->Play();
		}

		Uint32 balloonTsDiff = tick - p->GetBalloonTs();
		if (balloonTsDiff > 300) {
			balloonBounceY = 0;
		}
		else {
			balloonBounceY = ((balloonTsDiff % 100) < 50) ? 1 : 0;
		}
	}
	else {
		prevBalloonText.clear();
	}
}

void PlayerDecor::Render()
{
	auto p = player.lock();
	if (!p) return;

	int animIdx = (static_cast<int>(p->GetAnimDistance()) / 8) % 8;
	int spriteIdx = WALK_ANIM[p->GetDirection()][animIdx];

	//FIXME: We assume player sprite is 30x45.
	int px = static_cast<int>(p->GetPosX()) + 1;
	int py = static_cast<int>(p->GetPosY()) - 13;

	//TODO: Apply world->screen transform for pos.
	sprite->Render(px, py, spriteIdx);

	// Draw the dialog balloon centered above the player.
	if (balloonVisible) {
		int bw = balloon->MeasureWidth();
		int bh = balloon->MeasureHeight();
		int bx = (px + 15) - (bw / 2);
		int by = py - bh - balloonBounceY;

		SDL_Rect bgRect = { bx - 6, by, bw + 12, bh + 2 };
		SDL_SetRenderDrawColor(display.renderer, 0xff, 0xff, 0xff, 0xbf);
		SDL_RenderFillRect(display.renderer, &bgRect);
		SDL_SetRenderDrawColor(display.renderer, 0x00, 0x00, 0x00, 0xff);
		SDL_RenderDrawRect(display.renderer, &bgRect);

		balloon->Render(bx, by);
	}
}

}  // namespace AISDL

