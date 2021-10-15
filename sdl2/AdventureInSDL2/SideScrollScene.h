
/* SideScrollScene.h
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

#include "Scene.h"

namespace AISDL {

class Player;
class PlayerDecor;

/**
 * Base class for side-scroller scenes.
 * @author Michael Imamura
 */
class SideScrollScene : public Scene {
	typedef Scene SUPER;
public:
	SideScrollScene(Director &director, Display &display,
		const std::string &title);
	virtual ~SideScrollScene();

public:
	virtual void OnWalkOffEdgeLeft(std::shared_ptr<Player> player) { };
	virtual void OnWalkOffEdgeRight(std::shared_ptr<Player> player) { };

private:
	void ApplyPlayerMovement(float timeDiff);

public:
	// Scene
	virtual void Reset();
	virtual void Advance(Uint32 lastTick, Uint32 tick);
	virtual void RenderContent();

protected:
	std::unique_ptr<PlayerDecor> playerDecor;
};

}  // namespace AISDL
