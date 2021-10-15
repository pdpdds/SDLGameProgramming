
/* MainLoopScene.h
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
 * All about the main event loop.
 * @author Michael Imamura
 */
class MainLoopScene : public SideScrollScene {
	typedef SideScrollScene SUPER;
public:
	MainLoopScene(Director &director, Display &display);
	virtual ~MainLoopScene();

public:
	// SideScrollScene
	virtual void OnWalkOffEdgeLeft(std::shared_ptr<Player> player);
	virtual void OnWalkOffEdgeRight(std::shared_ptr<Player> player);

	// Scene
	virtual void OnAction();
	virtual void OnCancel();

	virtual void Preload();
	virtual void Reload();
	virtual void Reset();
	virtual void Advance(Uint32 lastTick, Uint32 tick);
	virtual void RenderContent();

private:
	int levelLoop;
	std::shared_ptr<Level> level;
	std::unique_ptr<LevelDecor> levelDecor;
	std::unique_ptr<PagedTextDecor> eventTxt;
	std::unique_ptr<Conversation> loopSayings;
};

}  // namespace AISDL
