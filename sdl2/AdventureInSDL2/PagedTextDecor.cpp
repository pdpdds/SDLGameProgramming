
/* PagedTextDecor.cpp
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

#include "FmtTextDecor.h"
#include "ResStr.h"

#include "PagedTextDecor.h"

namespace AISDL {

namespace {
	const float FLING_DURATION = 500.0f;
}

/**
 * Constructor.
 * @param display The target display.
 * @param font The font to use to render the text.
 * @param text The text itself.
 * @param width The maximum width of the text area (currently unused).
 * @param cursor Display a cursor after the rendered text.
 */
PagedTextDecor::PagedTextDecor(Display &display, std::shared_ptr<Ttf> font,
	std::shared_ptr<ResStr> text, int width, bool cursor) :
	display(display), font(font), text(text), width(width),
	cursor(cursor), cursorVisible(false),
	pageNum(0),
	animating(Anim::NONE), animStart(0),
	animProgress(0), animOffsetY(0), animAlpha(0xff),
	animOutgoingPage(UINT_MAX)
{
	Rebuild();

	// Automatically rebuild the pages when text changes.
	this->text->SetOnReload([&]() {
		Rebuild();
	});
}

/**
 * Regenerate the renderables when the source text changes.
 */
void PagedTextDecor::Rebuild()
{
	const auto &tpages = text->pages;

	pages.clear();

	for (auto iter = tpages.cbegin(); iter != tpages.cend(); ++iter) {
		pages.emplace_back(std::make_shared<FmtTextDecor>(display, font,
			*iter, width));
	}

	numPages = tpages.size();
	if (pageNum >= numPages) {
		pageNum = numPages - 1;
	}
}

void PagedTextDecor::StartAnim(Anim::type animate, unsigned int outgoingPage)
{
	animating = animate;
	animStart = SDL_GetTicks();

	switch (animate) {
	case Anim::NONE:
		animProgress = UINT_MAX;
		animOffsetY = 0;
		animAlpha = 255;
		animOutgoingPage = UINT_MAX;
		break;
	case Anim::TYPEWRITER:
		animProgress = 0;
		animOffsetY = 0;
		animAlpha = 255;
		animOutgoingPage = UINT_MAX;
		break;
	case Anim::FLING_UP:
		animProgress = UINT_MAX;
		animOffsetY = 100;
		animAlpha = 0;
		animOutgoingPage = outgoingPage;
		break;
	}
}

/**
 * Retrieve the actual pixel width of the current page.
 * @return The size in pixels.
 */
int PagedTextDecor::MeasureWidth() const
{
	return pages[pageNum]->MeasureWidth();
}

/**
 * Retrieve the actual pixel height of the current page.
 * @return The size in pixels.
 */
int PagedTextDecor::MeasureHeight() const
{
	return pages[pageNum]->MeasureHeight();
}

/**
 * Jump to the first page.
 * @param animate The page transition animation.
 */
void PagedTextDecor::FirstPage(Anim::type animate)
{
	StartAnim(animate);
	pageNum = 0;
}

/**
 * Advance to the next page.
 * @param animate The page transition animation.
 * @return @c true if the page changed, @c false if already at the end.
 */
bool PagedTextDecor::NextPage(Anim::type animate)
{
	if (pageNum < numPages - 1) {
		StartAnim(animate, pageNum++);
		return true;
	}
	else {
		return false;
	}
}

/**
 * Return to the previous page.
 * @param animate The page transition animation.
 * @return @c true if the page changed, @c false if already at the first page.
 */
bool PagedTextDecor::PrevPage(Anim::type animate)
{
	if (pageNum > 0) {
		StartAnim(animate, pageNum--);
		return true;
	}
	else {
		return false;
	}
}

void PagedTextDecor::Advance(Uint32 tick)
{
	Uint32 timeDiff = tick - animStart;

	switch (animating) {
	case Anim::NONE:
		// Blink the cursor.
		cursorVisible = cursor && ((tick % 1000) < 500);
		break;

	case Anim::TYPEWRITER:
		cursorVisible = cursor;
		animProgress = timeDiff / 10;
		if (animProgress >= pages[pageNum]->GetNumRenderables()) {
			StartAnim(Anim::NONE);
		}
		break;

	case Anim::FLING_UP:
		cursorVisible = cursor;
		{
			float pos = static_cast<float>(timeDiff) / FLING_DURATION;
			pos = powf(1 - pos, 2);
			animOffsetY = static_cast<int>(100 * pos);
			animAlpha = 255 - static_cast<int>(255 * pos);
		}

		if (timeDiff > static_cast<unsigned>(FLING_DURATION)) {
			StartAnim(Anim::NONE);
		}
		break;
	}
}

/**
 * Render the current page.
 * @param x The X coordinate.
 * @param y The Y coordinate.
 * @param alpha The opacity (0 is fully transparent, 255 is fully opaque).
 */
void PagedTextDecor::Render(int x, int y, int alpha) const
{
	/* Disabled for now.
	   The persistence of vision makes the animation look messy.
	if (animOutgoingPage != UINT_MAX) {
		pages[animOutgoingPage]->Render(
			x, y + (100 - animOffsetY),
			(alpha - (animAlpha * alpha / 255)) / 2);
	}
	*/

	pages[pageNum]->Render(
		x, y + animOffsetY,
		animAlpha * alpha / 255,
		cursorVisible, animProgress);
}

}
