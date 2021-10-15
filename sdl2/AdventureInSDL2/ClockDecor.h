
/* ClockDecor.h
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

namespace AISDL {

/**
 * Displays the time since the app started in the corner of the screen.
 * @author Michael Imamura
 */
class ClockDecor {
public:
	ClockDecor(Display &display);

private:
	void UpdateTimeStr(Uint32 tick);

public:
	void Flash();

public:
	void Advance(Uint32 tick);
	void Render();

private:
	Display &display;
	Uint32 lastUpdatedTs;
	std::string timeStr;
	bool visible;
	int alpha;
	Uint32 flashTs;
};

}  // namespace AISDL
