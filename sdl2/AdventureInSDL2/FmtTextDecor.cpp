
/* FmtTextDecor.cpp
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

#include "utf8/utf8.h"

#include "Exception.h"
#include "ResStr.h"

#include "FmtTextDecor.h"

namespace AISDL {

namespace {
	const SDL_Color FMT_COLORS[] = {
		{ 0x00, 0x00, 0x00, 0xff },  // 0   ^0  black
		{ 0xff, 0x00, 0x00, 0xff },  // 1   ^1  red
		{ 0x00, 0xff, 0x00, 0xff },  // 2   ^2  green
		{ 0xff, 0xff, 0x00, 0xff },  // 3   ^3  yellow
		{ 0x00, 0x00, 0xff, 0xff },  // 4   ^4  blue
		{ 0x00, 0xff, 0xff, 0xff },  // 5   ^5  cyan
		{ 0xff, 0x00, 0xff, 0xff },  // 6   ^6  magenta
		{ 0xff, 0xff, 0xff, 0xff },  // 7   ^7  [default] white
		{ 0xff, 0x7f, 0x00, 0xff },  // 8   ^8  orange
		{ 0x7f, 0x7f, 0x7f, 0xff },  // 9   ^9  dark gray
		{ 0xbf, 0xbf, 0xbf, 0xff },  // 10  ^:  light gray
		{ 0xbf, 0xbf, 0xbf, 0xff },  // 11  ^;  light gray
		{ 0x00, 0x7f, 0x00, 0xff },  // 12  ^<  dark green
		{ 0x7f, 0x7f, 0x00, 0xff },  // 13  ^=  dark yellow
		{ 0x00, 0x00, 0x7f, 0xff },  // 14  ^>  dark blue
		{ 0x7f, 0x00, 0x00, 0xff },  // 15  ^?  dark red
		{ 0x7f, 0x3f, 0x00, 0xff },  // 16  ^@  brown
		{ 0xff, 0x99, 0x19, 0xff },  // 17  ^a  pumpkin
		{ 0x00, 0x7f, 0x7f, 0xff },  // 18  ^b  dark cyan
		{ 0x7f, 0x00, 0x7f, 0xff },  // 19  ^c  dark magenta
		{ 0x00, 0x7f, 0xff, 0xff },  // 20  ^d  azure
		{ 0x7f, 0x00, 0xff, 0xff },  // 21  ^e  violet
		{ 0x33, 0x99, 0xcc, 0xff },  // 22  ^f  [external link]
		{ 0xcc, 0xff, 0xcc, 0xff },  // 23  ^g  mint green
		{ 0x00, 0x66, 0x33, 0xff },  // 24  ^h  forest green
		{ 0xff, 0x00, 0x33, 0xff },  // 25  ^i
		{ 0xb2, 0x19, 0x19, 0xff },  // 26  ^j
		{ 0x99, 0x33, 0x00, 0xff },  // 27  ^k
		{ 0xcc, 0x99, 0x33, 0xff },  // 28  ^l
		{ 0x99, 0x99, 0x33, 0xff },  // 29  ^m
		{ 0xff, 0xff, 0xbf, 0xff },  // 30  ^n
		{ 0xff, 0xff, 0x7f, 0xff },  // 31  ^o  [keyword link]
	};
}

/**
 * Constructor when using static string.
 * @param display The target display.
 * @param font The font to use to render the text.
 * @param s The text itself.
 * @param width The maximum width of the text area (currently unused).
 */
FmtTextDecor::FmtTextDecor(Display &display, std::shared_ptr<Ttf> font,
	const std::string &s, int width) :
	display(display), font(std::move(font)), s(s),
	width(width),
	sizeWidth(0), sizeHeight(0)
{
	Reformat();
}

/**
 * Constructor when using resource string.
 * @param display The target display.
 * @param font The font to use to render the text.
 * @param text The text itself.
 * @param width The maximum width of the text area (currently unused).
 */
FmtTextDecor::FmtTextDecor(Display &display, std::shared_ptr<Ttf> font,
	std::shared_ptr<ResStr> text, int width) :
	display(display), font(std::move(font)), text(std::move(text)),
	width(width),
	sizeWidth(0), sizeHeight(0)
{
	Reformat();

	// Automatically trigger Reformat() when the text changes.
	this->text->SetOnReload([&]() {
		Reformat();
	});
}

/**
 * Process the text and generate the list of formatted renderables.
 */
void FmtTextDecor::Reformat()
{
	int lineHeight = TTF_FontLineSkip(**font);
	int spaceWidth = font->spaceLayoutWidth;
	if (text) s = **text;
	int x = 0, y = 0;
	int fmtColor = 7;

	sizeWidth = 0;
	sizeHeight = 0;

	rends.clear();
	rends.reserve(s.length());

	uint32_t ch = 0;
	for (auto iter = s.cbegin(), iend = s.cend(); iter != iend; ) {
		ch = utf8::next(iter, iend);

		// Only handle characters which we have included in the type case.
		if (ch > font->glyphs.size()) {
			SDL_Log("Unrenderable code point: %d", ch);
			continue;
		}

		switch (ch) {
			case '\n':  // Newlines.
				x = 0;
				y += lineHeight;
				break;

			case '^':  // Color codes.
				if (iter == iend) {
					return;
				}
				else {
					uint32_t idx = utf8::next(iter, iend);

					// Allow "^^" as an escape sequence for "^".
					if (idx == '^') {
						const Ttf::Glyph &glyph = font->glyphs['^'];
						if (glyph.avail) {
							rends.emplace_back(Rend(&glyph, x, y, fmtColor));
							x += glyph.layoutW;
						}
						break;
					}

					fmtColor = (idx + 16) & 31;
				}
				break;

			case '\r':  // Ignore CRs.
				break;

			case ' ':
				x += spaceWidth;
				break;

			default: {
				const Ttf::Glyph &glyph = font->glyphs[ch];
				if (!glyph.avail) continue;

				rends.emplace_back(Rend(&glyph, x, y, fmtColor));

				x += glyph.layoutW;
			}
		}

		if (x >= sizeWidth) {
			sizeWidth = x;
		}
	}

	// If the last char wasn't a newline, adjust the height otherwise we'll
	// cut off the bottom line of text.
	if (ch != '\n') {
		y += lineHeight;
	}
	sizeHeight = y;
}

/**
 * Change the text to a static string.
 * @param s The static string.
 */
void FmtTextDecor::SetText(const std::string &s)
{
	if (text) {
		text.reset();
	}
	if (this->s != s) {
		this->s = s;
		Reformat();
	}
}

/**
 * Render the text at the specified coordinates.
 * @param x The X coordinate.
 * @param y The Y coordinate.
 * @param alpha The opacity (0 is fully transparent, 255 is fully opaque).
 * @param cursor @c true to draw a cursor at the end of the text.
 * @param limit The maximum number of characters to render.
 */
void FmtTextDecor::Render(int x, int y, int alpha, bool cursor,
	unsigned int limit) const
{
	// If completely transparent, do nothing.
	if (alpha == 0) return;

	int fmtColor = 7;

	SDL_SetTextureAlphaMod(font->typeCase, alpha);

	// Reset default color to white.
	SDL_SetTextureColorMod(font->typeCase, 0xff, 0xff, 0xff);

	unsigned int count = 0;
	int cursorX = x, cursorY = y;
	for (auto iter = rends.cbegin(); iter != rends.cend(); ++iter) {
		if (fmtColor != iter->fmtColor) {
			fmtColor = iter->fmtColor;
			const SDL_Color &color = FMT_COLORS[fmtColor];
			SDL_SetTextureColorMod(font->typeCase, color.r, color.g, color.b);
		}

		SDL_Rect destRect = {
			iter->x + x,
			iter->y + y,
			iter->glyph->texRect.w,
			iter->glyph->texRect.h,
		};
		if (SDL_RenderCopy(display.renderer, font->typeCase,
			&iter->glyph->texRect, &destRect) < 0)
		{
			throw Exception(SDL_GetError());
		}
		cursorX = destRect.x + iter->glyph->layoutW;
		cursorY = destRect.y;

		count++;
		if (count >= limit) {
			break;
		}
	}

	if (cursor) {
		const Ttf::Glyph &glyph = font->glyphs['_'];
		if (glyph.avail) {
			SDL_Rect destRect = {
				cursorX, cursorY,
				glyph.texRect.w, glyph.texRect.h
			};
			if (SDL_RenderCopy(display.renderer, font->typeCase,
				&glyph.texRect, &destRect) < 0)
			{
				throw Exception(SDL_GetError());
			}
		}
	}
}

}
