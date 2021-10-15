
/* Layer.cpp
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

#include <fstream>

#include "Layer.h"

namespace AISDL {

Layer::Layer(const std::string &filename) :
	visible(true)
{
	std::ifstream ifs(filename);
	if (!ifs) {
		SDL_Log("Could not open layer: %s", filename.c_str());
		return;
	}

	std::string ris;
	size_t maxWidth = 16;
	while (std::getline(ifs, ris)) {
		// Ignore blank lines and comments.
		if (ris.empty() || ris[0] == '#') {
			continue;
		}

		size_t width = ParseRow(ris, maxWidth);
		if (width > maxWidth) {
			maxWidth = width;
		}
	}

	// Normalize all rows to be the same width.
	for (auto iter = tiles.begin(); iter != tiles.end(); ++iter) {
		iter->resize(maxWidth);
	}

	SDL_Log("Loaded layer (%ux%u): %s", maxWidth, tiles.size(), filename.c_str());
}

size_t Layer::ParseRow(const std::string &line, size_t initWidth)
{
	std::istringstream iss(line);
	iss >> std::hex;

	tiles.emplace_back(row_t());
	row_t &row = tiles.back();
	row.reserve(initWidth);

	int tile;
	while (iss >> tile) {
		row.push_back(tile);
	}

	return row.size();
}

/**
 * Resize the tile grid to be an exact size.
 * @param width The desired width.
 * @param height The desired height.
 */
void Layer::Resize(size_t width, size_t height)
{
	tiles.resize(height);
	for (auto iter = tiles.begin(); iter != tiles.end(); ++iter) {
		iter->resize(width);
	}
}

}
