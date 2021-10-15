
/* SpriteMap.h
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

/**
 * A collection of sprites on a single texture.
 * @author Michael Imamura
 */
class SpriteMap {
public:
	SpriteMap(Display &display, SDL_Texture *texture, int w, int h);
	~SpriteMap();

public:
	static std::shared_ptr<SpriteMap> Load(Display &display,
		const std::string &filename, int w, int h);

public:
	void Render(int x, int y, int index);

private:
	Display &display;
	std::string filename;
	SDL_Texture *texture;
	int w, h;
	int perRow;
	int numSprites;
};

}  // namespace AISDL
