
/* Layer.h
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

/**
 * A single layer of the level.
 * @author Michael Imamura
 */
class Layer {
public:
	Layer(const std::string &filename);

private:
	size_t ParseRow(const std::string &line, size_t initWidth);

public:
	bool IsVisible() const { return visible; }
	void Show() { visible = true; }
	void Hide() { visible = false; }

	typedef std::vector<int> row_t;
	typedef std::vector<row_t> tiles_t;

	size_t GetHeight() const { return tiles.size(); }
	size_t GetWidth() const { return tiles.empty() ? 0 : tiles[0].size(); }
	void Resize(size_t width, size_t height);

	row_t::const_iterator GetRow(size_t row) const
	{
		return tiles[row].cbegin();
	}

private:
	bool visible;
	tiles_t tiles;
};

}  // namespace AISDL
