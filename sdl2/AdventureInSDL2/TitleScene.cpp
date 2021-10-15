
/* TitleScene.cpp
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

#include "Director.h"
#include "FmtTextDecor.h"

#include "TitleScene.h"

namespace AISDL {

TitleScene::TitleScene(Director &director, Display &display) :
	SUPER(director, display, "Title"),
	logo(nullptr)
{
}

TitleScene::~TitleScene()
{
	if (logo) SDL_DestroyTexture(logo);
}

void TitleScene::OnAction()
{
	director.RequestNextScene();
}

void TitleScene::Preload()
{
	//TODO: Detect if a controller is plugged in and change the
	//      text accordingly.
	//TODO: Support the "Start" button, for the sake of tradition.
	engageTxt.reset(new FmtTextDecor(display, display.res.bodyFont,
		"^7Press the ^2(A)^7 button to start", 1024));

	logo = display.LoadTexture(display.res.resDir + "/images/logo.png");
}

void TitleScene::Advance(Uint32 lastTick, Uint32 tick)
{
	//TODO: Animate the background.

	int alpha = (tick / 5) % 511;
	engageAlpha = (alpha < 256) ? (255 - alpha) : (alpha - 256);
}

void TitleScene::RenderContent()
{
	SDL_SetRenderDrawColor(display.renderer, 0x00, 0x00, 0x00, 0xff);
	SDL_RenderClear(display.renderer);
	display.SetHighRes();

	SDL_SetTextureAlphaMod(logo, 0x3f);
	display.RenderTexture(logo, 512 - 250, 200);

	int engageTxtWidth = engageTxt->MeasureWidth();
	engageTxt->Render(512 - (engageTxtWidth / 2), 512, engageAlpha);
}

}  // namespace AISDL

