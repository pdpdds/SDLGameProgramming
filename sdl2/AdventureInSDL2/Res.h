
/* Res.h
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
class Sound;
class SpriteMap;
class Ttf;

/**
 * Global resource library.
 * @author Michael Imamura
 */
class Res {
public:
	Res();
	~Res();

private:
	static bool CheckResDir(const std::string &path);

public:
	void Preload(Display &display);

public:
	std::string resDir;

	std::shared_ptr<Ttf> clockFont;
	std::shared_ptr<Ttf> pixelFont;
	std::shared_ptr<Ttf> bodyFont;
	std::shared_ptr<Ttf> talkFont;

	std::shared_ptr<SpriteMap> playerSprite;

	std::shared_ptr<SpriteMap> interiorTile;

	std::shared_ptr<Sound> doorSound;
	std::shared_ptr<Sound> talkSound;
};

}  // namespace AISDL

