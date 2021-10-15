
/* PlayerDecor.h
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

namespace AISDL {

class Display;
class FmtTextDecor;
class Player;
class SpriteMap;

/**
 * The main character view for the player.
 * @author Michael Imamura
 */
class PlayerDecor {
public:
	PlayerDecor(Display &display, std::shared_ptr<Player> player,
		std::shared_ptr<SpriteMap> sprite);
	~PlayerDecor();

public:
	void Advance(Uint32 tick);
	void Render();

private:
	Display &display;
	std::weak_ptr<Player> player;
	std::shared_ptr<SpriteMap> sprite;
	std::string prevBalloonText;
	bool balloonVisible;
	int balloonBounceY;
	std::unique_ptr<FmtTextDecor> balloon;
};

}  // namespace AISDL

