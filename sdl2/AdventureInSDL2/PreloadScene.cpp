
/* PreloadScene.cpp
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

#include "App.h"

#include "PreloadScene.h"

namespace AISDL {

namespace {
	const int BAR_WIDTH = 200;
	const int BAR_HEIGHT = 20;
}

PreloadScene::PreloadScene(App &director, Display &display) :
	SUPER(director, display, "Preloader"),
	app(director), loading(false), progress(0)
{
}

PreloadScene::~PreloadScene()
{
}

void PreloadScene::Advance(Uint32 lastTick, Uint32 tick)
{
	// Wait for the first frame to be rendered before preloading.
	if (loading) {
		double count = 0;
		double total = app.GetNumScenes() + 1;

		// Preload global assets.
		display.res.Preload(display);
		RenderProgress(++count, total);

		// Preload each of the scenes.
		app.ForEachScene([&](std::shared_ptr<Scene> &scene) {
			scene->Preload();
			RenderProgress(++count, total);
		});

		director.RequestNextScene();
	}
	else {
		loading = true;
		SDL_Log("Starting preload.");
	}
}

void PreloadScene::RenderContent()
{
	auto rend = display.renderer;

	display.SetLowRes();

	// Clear the screen.
	SDL_SetRenderDrawColor(rend, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(rend);

	SDL_Rect barRect = {
		256 - (BAR_WIDTH / 2), 192 - (BAR_HEIGHT / 2),
		static_cast<int>(BAR_WIDTH * progress), BAR_HEIGHT };

	// Draw the progress bar fill.
	SDL_SetRenderDrawColor(rend, 0xbf, 0xbf, 0xbf, 0xff);
	SDL_RenderFillRect(rend, &barRect);

	// Draw the progress bar frame.
	barRect.w = BAR_WIDTH;
	SDL_SetRenderDrawColor(rend, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderDrawRect(rend, &barRect);
}

/**
 * Render a progress update frame.
 *
 * This is called to display the preload progress from Advance().
 * This makes it easier to preload the list of our assets without going
 * through the main loop (since we don't care about events during this
 * time anyway).
 *
 * @param count The current progress within the total.
 * @param total The total.
 */
void PreloadScene::RenderProgress(double count, double total)
{
	progress = count / total;
	Render();
	SDL_RenderPresent(display.renderer);
}

}  // namespace AISDL

