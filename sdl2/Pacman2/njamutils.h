/*-----------------------------------------------------------------------------
Copyright 2003 Milan Babuskov

This file is part of Njam (http://njam.sourceforge.net).

Njam is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Njam is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Njam in file COPYING; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
-----------------------------------------------------------------------------*/
#ifndef NJAM_UTILS_H
#define NJAM_UTILS_H
#include <SDL2/SDL.h>
#include "njamfont.h"
//-----------------------------------------------------------------------------
void NjamSetRect(SDL_Rect& rect, int x, int y, int w=0, int h=0);
SDL_Keycode NjamGetch(bool Wait);
int NjamRandom(int MaxValue);
#ifdef __linux__
char *NjamGetHomeDir();
int NjamCheckAndCreateDir(const char *name);
#endif

typedef enum { fxBlackWhite, fxDarken } tEffect;
bool SurfaceEffect(SDL_Surface *surface, SDL_Rect& r, tEffect Effect = fxBlackWhite);
//-----------------------------------------------------------------------------
class LogFile	// writes string to a file, used for logging
{
public:
	LogFile(const char *Text, bool Create = false);
};
//-----------------------------------------------------------------------------
#endif
