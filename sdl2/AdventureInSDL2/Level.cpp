
/* Level.cpp
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

#include "Level.h"

namespace AISDL {

namespace {
	const int MAX_LAYERS = 99;
}

Level::Level(const std::string &dirname) :
	dirname(dirname), width(0), height(0)
{
	Reload();
}

std::shared_ptr<Level> Level::Load(const std::string &filename)
{
	return std::make_shared<Level>(filename);
}

void Level::Reload()
{
	layers.clear();

	// Look for "00.txt", "01.txt", etc.
	for (int layerNum = 0; layerNum < MAX_LAYERS; layerNum++) {
		std::ostringstream oss;
		oss << dirname << '/' <<
			std::setw(2) << std::setfill('0') << layerNum <<
			".txt";
		std::string filename = oss.str();

		// Check if the file exists.
		// If it doesn't exist, assume we've reached the end of the list.
		struct stat st;
		if (::stat(filename.c_str(), &st) != 0) break;

		layers.emplace_back(filename);
	}

	// Normalize all layers to be the same size.
	size_t maxHeight = 0, maxWidth = 0;
	for (auto iter = layers.begin(); iter != layers.end(); ++iter) {
		size_t layerW = iter->GetWidth();
		size_t layerH = iter->GetHeight();

		if (layerW > maxWidth) {
			maxWidth = layerW;
		}
		if (layerH > maxHeight) {
			maxHeight = layerH;
		}
	}
	for (auto iter = layers.begin(); iter != layers.end(); ++iter) {
		iter->Resize(maxWidth, maxHeight);
	}

	width = maxWidth;
	height = maxHeight;
}

void Level::ShowAllLayers()
{
	std::for_each(layers.begin(), layers.end(), std::mem_fn(&Layer::Show));
}

void Level::HideAllLayers()
{
	std::for_each(layers.begin(), layers.end(), std::mem_fn(&Layer::Hide));
}

}
