
/* ClockDecor.cpp
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

#include "Ttf.h"

#include "ClockDecor.h"

namespace AISDL {

namespace {
	const Uint32 DURATION = 2000;
}

ClockDecor::ClockDecor(Display &display) :
	display(display),
	lastUpdatedTs(0), timeStr("--:--:--"), visible(false),
	flashTs(0)
{
}

/**
 * Update the string representation of the time.
 * @param tick The time (in ms) since the start of the app.
 */
void ClockDecor::UpdateTimeStr(Uint32 tick)
{
	// We don't need to be exact; avoid updating if we probably don't need to.
	if (lastUpdatedTs > 0 && (tick - lastUpdatedTs) < 500) {
		return;
	}
	lastUpdatedTs = tick;

	int hours = tick / (1000 * 60 * 60);
	int minutes = (tick / (1000 * 60)) % 60;
	int seconds = (tick / 1000) % 60;

	std::ostringstream oss;
	oss << std::setfill('0') <<
		std::setw(2) << hours << ':' <<
		std::setw(2) << minutes << ':' <<
		std::setw(2) << seconds;
	timeStr = oss.str();
}

/**
 * Trigger the clock to be visible for a short period of time.
 */
void ClockDecor::Flash()
{
	flashTs = SDL_GetTicks();
	visible = true;

	UpdateTimeStr(flashTs);
	SDL_Log("Time: %s", timeStr.c_str());
}

void ClockDecor::Advance(Uint32 tick)
{
	if (visible) {
		Uint32 past = tick - flashTs;
		if (past > DURATION) {
			visible = false;
		}
		else {
			UpdateTimeStr(tick);

			double pos = static_cast<double>(past) / DURATION;
			alpha = 255 - static_cast<int>(255.0 * pow(pos, 4));
		}
	}
}

void ClockDecor::Render()
{
	display.SetLowRes();

	if (visible && display.res.clockFont) {
		display.res.clockFont->RenderText(display, 400, 0, 140, timeStr, alpha);
	}
}

}  // namespace AISDL
