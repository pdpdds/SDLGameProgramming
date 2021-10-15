/*-----------------------------------------------------------------------------
Njamfont.cpp

Simple font class that loads font resource (fixed width/height bitmap)
and provides functions to render it to screen


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
#include <stdio.h>
#include <SDL2/SDL.h>
#include "njamutils.h"
#include "njamfont.h"
//---------------------------------------------------------------------------
NjamFont::NjamFont(const char *Filename, int CharWidth, int CharHeight)
{
 	m_Surface = NULL;

	LogFile("Loading font...");
 	SDL_Surface *temp = SDL_LoadBMP(Filename);
 	if (!temp)
 	{
 		LogFile("FAILED.\n");
		LogFile((const char *)SDL_GetError());
 		return;
 	}
 	LogFile("OK.\n");

 	// setting color key for font: black is transparent
 	Uint32 black = SDL_MapRGB(temp->format, 0, 0, 0);
 	SDL_SetColorKey(temp, SDL_TRUE, black);

	/* Convert image to video format */
	m_Surface = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_RGBA8888, 0);
	SDL_FreeSurface(temp);
	if ( m_Surface == NULL )
		fprintf(stderr, "Couldn't convert font image: %s\n", SDL_GetError());

	m_CharWidth = CharWidth;
	m_CharHeight = CharHeight;
}
//---------------------------------------------------------------------------
NjamFont::~NjamFont()
{
 	if (m_Surface)
		SDL_FreeSurface(m_Surface);
}
//---------------------------------------------------------------------------
int NjamFont::GetCharWidth()
{
 	return m_CharWidth;
}
//---------------------------------------------------------------------------
int NjamFont::GetCharHeight()
{
 	return m_CharHeight;
}
//---------------------------------------------------------------------------
// Writes text centered at screen
bool NjamFont::WriteTextCentered(SDL_Surface *Destination, int y, const char *Text)
{
 	int len=0;
	while (Text[len++]);	// get string length
 	int xpos = (Destination->w - len * m_CharWidth) / 2;
	return WriteTextXY(Destination, xpos, y, Text);
}
//---------------------------------------------------------------------------
// Uses CharWidth and Height to split screen into Columns and Rows
bool NjamFont::WriteTextColRow(SDL_Surface *Destination, int Col, int Row, const char *Text)
{
	return WriteTextXY(Destination, Col * m_CharWidth, Row * m_CharHeight, Text);
}
//---------------------------------------------------------------------------
// Outputs Text into Destionation surface at coordinate (x, y)
bool NjamFont::WriteTextXY(SDL_Surface *Destination, int x, int y, const char *Text)
{
 	const char FontMap[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,;:!@*()%/";
    if (!m_Surface || !Destination)
		return false;

	SDL_Rect src, dest;
	src.y = 0;
	src.w = m_CharWidth;
	src.h = m_CharHeight;

	if (y < -m_CharHeight || y > Destination->h)	// out of screen
		return true;

    for (int i=0; Text[i]; i++)
    {
		dest.y = y;
		dest.x = x + i * m_CharWidth;

		if (dest.x < -m_CharWidth)			// not yet in screen
			continue;

    	if (dest.x > Destination->w)		// outside
    		break;

		if (Text[i] == ' ' || Text[i] == 10 || Text[i] == 13)			// SPACE, NL, CR
			continue;

		bool Found = false;
        for (int j=0; j < (sizeof(FontMap)/sizeof(char)); j++)
        {
            if (Text[i] == FontMap[j])
            {
            	Found = true;
            	src.x = j * m_CharWidth;

			    if (0 != SDL_BlitSurface(m_Surface, &src, Destination, &dest))
			    {
			    	LogFile("Failed to blit font character image.\n");
					LogFile((const char *)SDL_GetError());
					return false;
				}
                break;
            }
        }

        if (!Found)
   			printf("TEXT OUTPUT WARNING: Character: %c (code: %d) was not found.\n", Text[i], Text[i]);
	}
    return true;
}
//---------------------------------------------------------------------------

