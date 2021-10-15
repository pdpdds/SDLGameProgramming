
/* ImageScene.h
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

#pragma once

#include "SideScrollScene.h"

namespace AISDL {

class Conversation;
class Level;
class LevelDecor;
class PagedTextDecor;

/**
 * The SDL_image art gallery.
 * @author Michael Imamura
 */
class ImageScene : public SideScrollScene {
	typedef SideScrollScene SUPER;
public:
	ImageScene(Director &director, Display &display);
	virtual ~ImageScene();

private:
	SDL_Texture *LoadPainting(const std::string &filename);

public:
	// SideScrollScene
	virtual void OnWalkOffEdgeLeft(std::shared_ptr<Player> player);
	virtual void OnWalkOffEdgeRight(std::shared_ptr<Player> player);

	// Scene
	virtual void OnInteract();
	virtual void OnAction();

	virtual void Preload();
	virtual void Reload();
	virtual void Reset();
	virtual void Advance(Uint32 lastTick, Uint32 tick);
	virtual void RenderContent();
private:
	void DrawPainting(SDL_Texture *texture, int x, int y, int w, int h);

private:
	std::shared_ptr<Level> level;
	std::unique_ptr<LevelDecor> levelDecor;
	std::unique_ptr<Conversation> convo;
	SDL_Texture *cavePainting;
	SDL_Texture *animePainting;
	SDL_Texture *sunsetPainting;
	bool showPaintings;
	Uint32 flashTs;
	Uint8 flashAlpha;
};

}  // namespace AISDL
