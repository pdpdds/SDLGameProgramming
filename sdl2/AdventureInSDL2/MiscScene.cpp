
/* MiscScene.cpp
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

#include "PagedTextDecor.h"
#include "ResStr.h"

#include "MiscScene.h"

namespace AISDL {

MiscScene::MiscScene(Director &director, Display &display) :
	SUPER(director, display, "Miscellaneous")
{
}

MiscScene::~MiscScene()
{
}

void MiscScene::OnAction()
{
	if (!miscTxt->NextPage(PagedTextDecor::Anim::FLING_UP)) {
		director.RequestNextScene();
	}
}

void MiscScene::OnCancel()
{
	miscTxt->PrevPage();
}

void MiscScene::Preload()
{
	SUPER::Preload();

	const std::string dir = display.res.resDir + "/text/misc/";
	miscTxt.reset(new PagedTextDecor(display, display.res.bodyFont,
		ResStr::Load(dir + "misc.txt"), 864));
}

void MiscScene::Reset()
{
	SUPER::Reset();

	miscTxt->FirstPage(PagedTextDecor::Anim::FLING_UP);
}

void MiscScene::Advance(Uint32 lastTick, Uint32 tick)
{
	miscTxt->Advance(tick);
}

void MiscScene::RenderContent()
{
	SDL_SetRenderDrawColor(display.renderer, 0x00, 0x00, 0x00, 0xff);
	SDL_RenderClear(display.renderer);

	display.SetHighRes();
	miscTxt->Render(80, 80);
}

}  // namespace AISDL
