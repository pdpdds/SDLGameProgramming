
/* FmtTextDecor.h
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

#include "Display.h"
#include "Ttf.h"

namespace AISDL {

class ResStr;

/**
 * A formatted text renderable.
 *
 * This supports basic formatting using newlines and Quake-style color
 * codes (e.g. "This is ^1red ^7and ^3yellow"), based on the following
 * chart: http://wolfwiki.anime.net/index.php/Color_Codes
 *
 * @author Michael Imamura
 */
class FmtTextDecor {
public:
	FmtTextDecor(Display &display, std::shared_ptr<Ttf> font,
		const std::string &s, int width);
	FmtTextDecor(Display &display, std::shared_ptr<Ttf> font,
		std::shared_ptr<ResStr> text, int width);

private:
	struct Rend {
		Rend(const Ttf::Glyph *glyph, int x, int y, int fmtColor) :
			glyph(glyph), x(x), y(y), fmtColor(fmtColor) { }

		const Ttf::Glyph *glyph;
		int x, y;
		int fmtColor;
	};
	void Reformat();

public:
	void SetText(const std::string &s);

public:
	size_t GetNumRenderables() const { return rends.size(); }

	int MeasureHeight() const { return sizeHeight; }
	int MeasureWidth() const { return sizeWidth; }

public:
	void Render(int x, int y, int alpha=0xff, bool cursor=false,
		unsigned int limit=UINT_MAX) const;

private:
	Display &display;
	std::shared_ptr<Ttf> font;
	std::shared_ptr<ResStr> text;
	std::string s;
	int width;
	int sizeWidth, sizeHeight;

	std::vector<Rend> rends;
};

}
