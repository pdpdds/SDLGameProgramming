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
#ifndef NJAM_MAP_H
#define NJAM_MAP_H
//---------------------------------------------------------------------------
#define MAPS 20
#define MAPW 28
#define MAPH 24
//---------------------------------------------------------------------------
typedef enum { ttWall=0,	ttEmpty,	ttGHouse,	ttDoor,			ttJuice,	ttCookie,
               ttFreezer,	ttTrap,		ttTeleport,	ttInvisible, 	ttPoints,	ttGHouseActive
} TileType;
//---------------------------------------------------------------------------
typedef struct
{
 	int x;
 	int y;
 	bool Invalid;
} MapPoint;
//---------------------------------------------------------------------------
class NjamMap
{
private:
    SDL_Surface *m_Images;
    int m_TileW, m_TileH;
    Uint8 m_Tiles[MAPS+1][MAPW][MAPH];	// MAPS+1 since last map is used for playing (it's a copy of actual map)

public:
    void Clear();
	void ClearCurrent();
	void AddPowerups(int HowMuch);
    int  CrossRoads(int x, int y);
    MapPoint FindSpecificTile(TileType type);
	int CountSpecificTiles(TileType type);
	MapPoint FindOtherTeleport(int x, int y);
	TileType GetTile(int x, int y);
    Uint8 *GetTiles();
    MapPoint GhostHouseXY();
	bool IsOk(int MapNumber);
    int  Load(const char *file);
	void RevertToMap(int map);
	void SwapMaps(int map1, int map2);
    void RenderMap(SDL_Surface *Destination, int x, int y);
    void RenderMap(SDL_Surface *Destination);
	void SetCurrentMap(int NewCurrentMap);
    void SetMapImages(SDL_Surface *, int TileW, int TileH);
    void SetTile(int x, int y, TileType Value);
    bool Save(const char *file);
	void TurnOnCrossRoads(int x, int y, int& dx, int& dy, int Choice);
	int SelectMap(char type, SDL_Surface *screen, NjamFont *font);
    NjamMap();
};
//---------------------------------------------------------------------------
#endif

