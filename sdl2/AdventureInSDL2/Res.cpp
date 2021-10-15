
/* Res.cpp
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

#include <sys/stat.h>

#include "Exception.h"
#include "Sound.h"
#include "SpriteMap.h"
#include "Ttf.h"

#include "Res.h"

namespace AISDL {

Res::Res()
{
	// Search for our resource directory.
	// With CMake, the preference is to put the build files in a subdirectory
	// of the project, but there's nothing wrong with putting them at the
	// toplevel either, so we check the possible locations.
	//TODO: Handle install dir from CMake.
	const char *dirs[] = { "share", "../share", "../../share" };
	for (unsigned i = 0; i < (sizeof(dirs) / sizeof(dirs[0])); i++) {
		if (CheckResDir(dirs[i])) {
			resDir = dirs[i];
			break;
		}
	}

	if (resDir.empty()) {
		throw Exception("Unable to determine location of resources.");
	}
	else {
		SDL_Log("Found resource directory: %s", resDir.c_str());
	}
}

Res::~Res()
{
}

/**
 * Determine if the given directory probably contains our resources.
 * @param path The path to check.
 * @return @c true if successful, @c false otherwise.
 */
bool Res::CheckResDir(const std::string &path)
{
	struct stat st;
	std::string check = path + "/version.txt";
	return ::stat(check.c_str(), &st) == 0;
}

/**
 * Preload all global resources.
 * @param display The target display.
 */
void Res::Preload(Display &display)
{
	const std::string fontDir = resDir + "/fonts/";
	pixelFont = Ttf::Load(display, fontDir + "FifteenNarrow.ttf", 16);
	clockFont = pixelFont;
	bodyFont = Ttf::Load(display, fontDir + "Tuffy.ttf", 25);
	talkFont = Ttf::Load(display, fontDir + "Minecraftia.ttf", 14);

	const std::string spriteDir = resDir + "/sprites/";
	playerSprite = SpriteMap::Load(display, spriteDir + "wizard.png", 30, 45);

	const std::string tileDir = resDir + "/tiles/";
	interiorTile = SpriteMap::Load(display, tileDir + "inq-xp-mi.png", 32, 32);

	const std::string soundDir = resDir + "/sounds/";
	doorSound = Sound::Load(soundDir + "door14.wav");
	talkSound = Sound::Load(soundDir + "ploep1.wav");
}

}  // namespace AISDL

