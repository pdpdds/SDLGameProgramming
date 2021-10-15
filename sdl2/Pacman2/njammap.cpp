/*-----------------------------------------------------------------------------
Njammap.cpp

Handling all that concerns game maps (worlds):
Loading, saving, searching for tiles, etc...


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
#include "njammap.h"
//---------------------------------------------------------------------------
NjamMap::NjamMap()
{
 	Clear();
}
//---------------------------------------------------------------------------
// returns number of maps loaded
int NjamMap::Load(const char *file)
{
	if (file == NULL)
	{
		printf("Warning! NjamMap::Load(), file == NULL\n");
		return -1;
	}

    FILE *fp = fopen(file, "rb");
    if (fp == NULL)
    {
    	printf("ERROR: Map file: \"%s\" could not be opened.\n", file);
        return -1;
    }

    Clear();

    // reads maps in order
    int i, loaded;
	bool ok = true;
    for (i=0; i<MAPS; i++)
    {
        if (fread(m_Tiles[i], MAPW, MAPH, fp) != MAPH)
		{
			printf("Error reading map: %d\n", i);
			loaded = -1;
            break;
		}

	    if (ok && m_Tiles[i][1][1] == ttWall)		// incomplete map found. Loader concludes that
		{
			ok = false;
	    	loaded = i;										// any subsequent map isn't finished
		}
    }
    fclose(fp);

	// output loaded maps, just for debugging
	/*
	for (int j=0; j<i; j++)
	{
		int count = 0;
		for (int k=0; k<MAPW; k++)
			for (int l=0; l<MAPH; l++)
				if (m_Tiles[j][k][l] == ttCookie)
					count++;
		printf("MAP %02d, cookies = %3d\n", j, count);
	}*/


    return (ok ? i : loaded);
}
//---------------------------------------------------------------------------
Uint8 *NjamMap::GetTiles()
{
 	return &(m_Tiles[MAPS][0][0]);
}
//---------------------------------------------------------------------------
bool NjamMap::Save(const char *file)
{
    FILE *fp = fopen(file, "w+b");
    if (fp == NULL)
        return false;

    // save all maps
    if (fwrite(m_Tiles, MAPW*MAPH, MAPS, fp) != MAPS)
    {
		printf("Cannot save maps file");
        fclose(fp);
        return false;
    }

    fclose(fp);
    return true;
}
//---------------------------------------------------------------------------
// dummy test to see if map has been edited at all
bool NjamMap::IsOk(int MapNumber)
{
	return (m_Tiles[MapNumber][1][1] != ttWall);
}
//---------------------------------------------------------------------------
void NjamMap::SwapMaps(int map1, int map2)
{
	if (map1 < 0 || map1 >= MAPS || map2 < 0 || map2 >= MAPS || map1 == map2)
		return;

	Uint8 temp;
	for (int i=0; i<MAPW; i++)
	{
		for (int j=0; j<MAPH; j++)
		{
			temp = m_Tiles[map1][i][j];
			m_Tiles[map1][i][j] = m_Tiles[map2][i][j];
			m_Tiles[map2][i][j] = temp;
		}
	}
}
//---------------------------------------------------------------------------
void NjamMap::SetCurrentMap(int NewCurrentMap)
{
 	// copy data from current map to last map (which is used for gameplay)
 	if (NewCurrentMap >= 0 && NewCurrentMap < MAPS)
 	{
	    for (int i=0; i<MAPW; i++)
	        for (int j=0; j<MAPH; j++)
	            m_Tiles[MAPS][i][j] = m_Tiles[NewCurrentMap][i][j];

		// set corners to blank
		//m_Tiles[MAPS][1]     [1]      = ttEmpty;	don't touch
		m_Tiles[MAPS][1]     [MAPH-2] = ttEmpty;
		m_Tiles[MAPS][MAPW-2][1]      = ttEmpty;
		m_Tiles[MAPS][MAPW-2][MAPH-2] = ttEmpty;
	}
}
//---------------------------------------------------------------------------
void NjamMap::ClearCurrent()
{
	for (int i=0; i<MAPW; i++)
		for (int j=0; j<MAPH; j++)
			m_Tiles[MAPS][i][j] = ttWall;

	m_Tiles[MAPS][1]     [MAPH-2] = ttEmpty;
	m_Tiles[MAPS][MAPW-2][1]      = ttEmpty;
	m_Tiles[MAPS][MAPW-2][MAPH-2] = ttEmpty;
}
//---------------------------------------------------------------------------
void NjamMap::RenderMap(SDL_Surface *Destination)
{
    RenderMap(Destination, 0, 0);
}
//---------------------------------------------------------------------------
void NjamMap::RenderMap(SDL_Surface *Destination, int x, int y)
{
    SDL_Rect src, dest;
    src.y = 0;
    src.h = m_TileH;
    src.w = m_TileW;

    for (int i=0; i<MAPW; i++)
    {
        for (int j=0; j<MAPH; j++)
        {
            src.x = m_Tiles[MAPS][i][j] * m_TileW;
            dest.x = x + i*m_TileW;
            dest.y = y + j*m_TileH;
            SDL_BlitSurface(m_Images, &src, Destination, &dest);
        }
    }
}
//---------------------------------------------------------------------------
void NjamMap::RevertToMap(int map)
{
    for (int i=0; i<MAPW; i++)
        for (int j=0; j<MAPH; j++)
            m_Tiles[map][i][j] = m_Tiles[MAPS][i][j];
}
//---------------------------------------------------------------------------
TileType NjamMap::GetTile(int x, int y)
{
    if (x < 0 || x>=MAPW || y<0 || y>=MAPH)
        return ttWall;

    return (TileType)(m_Tiles[MAPS][x][y]);
}
//---------------------------------------------------------------------------
void NjamMap::SetTile(int x, int y, TileType Value)
{
    if (x < 0 || x>=MAPW || y<0 || y>=MAPH)
        return;

    m_Tiles[MAPS][x][y] = (char)Value;
}
//---------------------------------------------------------------------------
void NjamMap::SetMapImages(SDL_Surface *Surface, int TileW, int TileH)
{
    m_Images = Surface;
    m_TileW = TileW;
    m_TileH = TileH;
}
//---------------------------------------------------------------------------
// insert random powerups
void NjamMap::AddPowerups(int HowMuch)
{
    TileType allowed[] = { ttJuice, ttFreezer, ttTrap, ttInvisible, ttCookie };
    for (int i=0; i<HowMuch; i++)
    {
        TileType t;
        int x, y;
        do
        {
            x = NjamRandom(MAPW);
            y = NjamRandom(MAPH);
            t = GetTile(x, y);
        }
        while (ttCookie != t);

        SetTile(x, y,  allowed[NjamRandom(5)]);
    }
}
//---------------------------------------------------------------------------
void NjamMap::TurnOnCrossRoads(int x, int y, int& dx, int& dy, int Choice)
{
    int r = 0;

    // if !moving_left
    if (dx != 1 && GetTile(x-1, y) != ttWall)		// check left
    {
        if (r == Choice)
        {
        	dx = -1;
        	dy = 0;
        	return;
        }
        r++;
	}

    if (dx != -1 && GetTile(x+1, y) != ttWall)
    {
        if (r == Choice)
        {
        	dx = 1;
        	dy = 0;
        	return;
        }
        r++;
	}

    if (dy != 1 && GetTile(x, y-1) != ttWall)
    {
        if (r == Choice)
        {
        	dx = 0;
        	dy = -1;
        	return;
        }
        r++;
	}

    if (dy != -1 && GetTile(x, y+1) != ttWall)
    {
        if (r == Choice)
        {
        	dx = 0;
        	dy = 1;
        }
	}
}
//---------------------------------------------------------------------------
// returns number of adjacent non-wall tiles
int NjamMap::CrossRoads(int x, int y)
{
    int r = 0;
    if (GetTile(x-1, y) != ttWall)        // returns wall even if error
        r++;
    if (GetTile(x+1, y) != ttWall)
        r++;
    if (GetTile(x, y-1) != ttWall)
        r++;
    if (GetTile(x, y+1) != ttWall)
        r++;
    return r;
}
//---------------------------------------------------------------------------
void NjamMap::Clear()
{
    for (int i=0; i<MAPS; i++)
        for (int j=0; j<MAPW; j++)
            for (int k=0; k<MAPH; k++)
                m_Tiles[i][j][k] = ttWall;
}
//---------------------------------------------------------------------------
MapPoint NjamMap::GhostHouseXY()
{
    MapPoint tempp;
    for (int i=0; i<MAPW; i++)
    {
        for (int j=0; j<MAPH; j++)
        {
            if (m_Tiles[MAPS][i][j] == ttGHouse || m_Tiles[MAPS][i][j] == ttGHouseActive)
            {
                tempp.x = i;
                tempp.y = j;
                tempp.Invalid = false;
                return tempp;
            }
        }
    }

    tempp.x = 0;
    tempp.y = 0;
    tempp.Invalid = true;
    return tempp;
}
//---------------------------------------------------------------------------
int NjamMap::CountSpecificTiles(TileType type)
{
	int count = 0;
    for (int i=0; i<MAPW; i++)
        for (int j=0; j<MAPH; j++)
            if (m_Tiles[MAPS][i][j] == type)
				count++;
	return count;
}
//---------------------------------------------------------------------------
MapPoint NjamMap::FindOtherTeleport(int x, int y)
{
	MapPoint tempp;
	tempp.x = x;
	tempp.y = y;
	tempp.Invalid = false;

	int count = CountSpecificTiles(ttTeleport);
	if (count < 2)
		return tempp;

	int selected = NjamRandom(count-1);
    for (int i=0; i<MAPW; i++)
	{
        for (int j=0; j<MAPH; j++)
		{
			if (i == x && j == y)
				continue;

            if (m_Tiles[MAPS][i][j] == ttTeleport)
				selected--;

			if (selected < 0)
			{
				tempp.x = i;
				tempp.y = j;
				return tempp;
			}
		}
	}

	tempp.Invalid = true;
	return tempp;
}
//---------------------------------------------------------------------------
MapPoint NjamMap::FindSpecificTile(TileType type)
{
    MapPoint tempp;
    for (int i=0; i<MAPW; i++)
    {
        for (int j=0; j<MAPH; j++)
        {
            if (m_Tiles[MAPS][i][j] == type)
            {
                tempp.x = i;
                tempp.y = j;
                tempp.Invalid = false;
                return tempp;
            }
        }
    }

    tempp.x = 0;
    tempp.y = 0;
    tempp.Invalid = true;
    return tempp;
}
//---------------------------------------------------------------------------
