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
#ifndef NJAM_FONT_H
#define NJAM_FONT_H
//---------------------------------------------------------------------------
class NjamFont
{
private:
    SDL_Surface *m_Surface;       // bitmap (raster font)
    int m_CharWidth, m_CharHeight;
public:
	int GetCharWidth();
	int GetCharHeight();

	bool WriteTextXY(SDL_Surface *Destination, int x, int y, const char *Text);
	bool WriteTextColRow(SDL_Surface *Destination, int Col, int Row, const char *Text);
	bool WriteTextCentered(SDL_Surface *Destination, int y, const char *Text);

    NjamFont(const char *Filename, int CharWidth, int CharHeight);
    ~NjamFont();
};
//---------------------------------------------------------------------------
#endif
