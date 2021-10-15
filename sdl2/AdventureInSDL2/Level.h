
/* Level.h
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

#include "Layer.h"

namespace AISDL {

class Level {
public:
	Level(const std::string &dirname);

public:
	static std::shared_ptr<Level> Load(const std::string &filename);
	void Reload();

public:
	size_t GetWidth() const { return width; }
	size_t GetHeight() const { return height; }

	template<typename Fn>
	void ForEachLayer(Fn fn) const
	{
		std::for_each(layers.cbegin(), layers.cend(), fn);
	}
	size_t GetNumLayers() const { return layers.size(); }
	const Layer &GetLayer(size_t i) const { return layers[i]; }

	void ShowAllLayers();
	void HideAllLayers();
	void ShowLayer(size_t i) { layers[i].Show(); }
	void HideLayer(size_t i) { layers[i].Hide(); }

private:
	const std::string dirname;
	std::vector<Layer> layers;
	size_t width;
	size_t height;
};

}  // namespace AISDL
