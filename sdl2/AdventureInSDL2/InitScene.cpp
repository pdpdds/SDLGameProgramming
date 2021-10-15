
/* InitScene.cpp
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

#include "InitScene.h"

namespace AISDL {

InitScene::InitScene(Director &director, Display &display) :
	SUPER(director, display, "First steps: Init and create window")
{
}

InitScene::~InitScene()
{
}

void InitScene::OnAction()
{
	if (!initTxt->NextPage(PagedTextDecor::Anim::FLING_UP)) {
		director.RequestNextScene();
	}
}

void InitScene::OnCancel()
{
	initTxt->PrevPage();
}

void InitScene::Preload()
{
	SUPER::Preload();

	const std::string dir = display.res.resDir + "/text/init/";
	initTxt.reset(new PagedTextDecor(display, display.res.bodyFont,
		ResStr::Load(dir + "init.txt"), 864));
}

void InitScene::Reset()
{
	SUPER::Reset();

	initTxt->FirstPage(PagedTextDecor::Anim::FLING_UP);
}

void InitScene::Advance(Uint32 lastTick, Uint32 tick)
{
	initTxt->Advance(tick);
}

void InitScene::RenderContent()
{
	SDL_SetRenderDrawColor(display.renderer, 0x00, 0x00, 0x00, 0xff);
	SDL_RenderClear(display.renderer);

	display.SetHighRes();
	initTxt->Render(80, 80);
}

}  // namespace AISDL
