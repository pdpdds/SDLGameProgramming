
/* Ttf.h
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

#include <SDL2/SDL_ttf.h>

namespace AISDL {

class Display;

/**
 * Wrapper and renderer for SDL_ttf fonts.
 *
 * This provides two ways to render text.
 *
 * Texture() renders UTF-8 text in the best possible quality, as a single
 * texture that the caller can render however they wish.  No colorization
 * is performed, but in exchange you get proper kerning.
 *
 * RenderText() renders UTF-8 text, but only code points covered by Latin-1.
 * The text is rendered directly to the current render target.  This
 * supports Quake-style color codes, but kerning isn't taken into account.
 *
 * @author Michael Imamura
 */
class Ttf : public std::enable_shared_from_this<Ttf> {
	friend TTF_Font *operator*(const Ttf &ttf);
	friend std::ostream &operator<<(std::ostream &os, const Ttf &ttf);
public:
	Ttf(Display &display, TTF_Font *font, int size);
	~Ttf();

public:
	static std::shared_ptr<Ttf> Load(Display &display,
		const std::string &filename, int size);

public:
	SDL_Texture *Texture(const Display &display, const std::string &s);
	void RenderText(Display &display, int x, int y, int width,
		const std::string &s, int alpha=0xff);

public:
	struct Glyph {
		Glyph() : avail(false) { }

		bool avail;
		SDL_Rect texRect;
		int layoutW;
	};
private:
	bool AddGlyph(SDL_Surface *surface, Uint16 ch, int &x, int y,
		Ttf::Glyph &glyph);
	void AddGlyphRange(SDL_Surface *surface, int &x, int &y, int lineHeight,
		Uint16 startCh, Uint16 endCh);
	void InitTypeCase();

private:
	TTF_Font *font;
	int size;
	Display &display;
public:
	SDL_Texture *typeCase;
	std::vector<Glyph> glyphs;
	int spaceLayoutWidth;
};

inline TTF_Font *operator*(const Ttf &ttf)
{
	return ttf.font;
}

inline std::ostream &operator<<(std::ostream &os, const Ttf &ttf)
{
	const char *fontName = TTF_FontFaceFamilyName(ttf.font);
	os << (fontName ? fontName : "Unnamed Font");

	const char *styleName = TTF_FontFaceStyleName(ttf.font);
	if (styleName) {
		os << ' ' << styleName;
	}

	os << ' ' << ttf.size;

	return os;
}

}  // namespace AISDL

