
/* FinalScene.cpp
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

#include "Exception.h"
#include "FmtTextDecor.h"
#include "ResStr.h"

#include "FinalScene.h"

namespace AISDL {

FinalScene::FinalScene(Director &director, Display &display) :
	SUPER(director, display, "Wrap-up and Credits"),
	qrCode(nullptr)
{
}

FinalScene::~FinalScene()
{
	if (qrCode) {
		SDL_DestroyTexture(qrCode);
	}
}

void FinalScene::OnAction()
{
	director.RequestShutdown();
}

void FinalScene::OnCancel()
{
	director.RequestShutdown();
}

void FinalScene::Preload()
{
	SUPER::Preload();

	std::string dir;

	dir = display.res.resDir + "/text/final/";
	finalTxt.reset(new FmtTextDecor(display, display.res.bodyFont,
		ResStr::Load(dir + "final.txt"), 640));

	dir = display.res.resDir + "/images/final/";
	SDL_Surface *surface = IMG_Load((dir + "url-qr.png").c_str());
	if (!surface) {
		throw Exception("Failed to load QR code", IMG_GetError());
	}
	qrCode = SDL_CreateTextureFromSurface(display.renderer, surface);
	if (!qrCode) {
		throw Exception("Failed to create texture", SDL_GetError());
	}

}

void FinalScene::Advance(Uint32 lastTick, Uint32 tick)
{
}

void FinalScene::RenderContent()
{
	SDL_SetRenderDrawColor(display.renderer, 0x3f, 0x3f, 0x3f, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(display.renderer);
	display.SetHighRes();

	finalTxt->Render(40, 40);

	display.RenderTexture(qrCode, 80, 200, 240, 240);
}

}  // namespace AISDL

