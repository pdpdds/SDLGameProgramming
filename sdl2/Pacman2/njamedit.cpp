//-----------------------------------------------------------------------------
//	Filename:	njamedit.cpp
//	Created:	22. Sep 2003. by Milan Babuskov
//
//  Purpose:	Handle everything about level editor
//
//  This file implements methods of NjamEngine class
//  which are defined in njam.h
//
//  Tabs should be at 4 spaces.
//  Each section is separated with line: //-------...
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
#include <stdlib.h>
#include "dirent.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "njamutils.h"
#include "njammap.h"
#include "njamfont.h"
#include "njam.h"
#include "njamsingleton.h"
//-----------------------------------------------------------------------------
void NjamEngine::LevelEditor()
{
	int level_type = 0;	// cooperative
	int level_type_was = 0;
	char filename[512];
	filename[0] = '\0';

	int x=2, y=1;
	m_Maps.Clear();
	if( m_GameOptions.UsedSkin > 0 )
		m_Maps.SetMapImages(m_Skins[m_GameOptions.UsedSkin-1], 25, 25);
	else
		m_Maps.SetMapImages(m_Skins[0], 25, 25);
	m_CurrentMap = 0;
	m_Maps.SetCurrentMap(0);
	m_SwapLevel = -1;
	bool changed = false;

	while (true)
	{
		RenderEditor(changed, x, y, level_type);
		Render(m_Screen);
		RenderEditor(changed, x, y, level_type);		// Twice for double buffer. Only needed when hwsurfaces
		Render(m_Screen);

		// wait for a keypress
        SDL_Keycode key = NjamGetch(true);				// true = wait for keypress
		if (key == SDLK_ESCAPE && (!changed || Query("ARE YOU SURE YOU WISH TO EXIT? (Y/N)")))
			return;

		if (key == SDLK_k)
		{
			level_type = 1 - level_type;
			changed = true;
		}

		if (key == SDLK_p)
		{
			if (m_Maps.IsOk(MAPS))
				m_Maps.SetTile(1, 1, ttWall);
			else
				m_Maps.SetTile(1, 1, ttEmpty);
			changed = true;
		}

		if (key == SDLK_c)
		{
			m_Maps.ClearCurrent();
			changed = true;
		}

		if (key == SDLK_l)	// select file from directory
		{
			if (SelectMap('A', filename, sizeof(filename)) > -1)
			{
				int w=0;
				while (filename[w])
					w++;

				if (filename[w-1] == 'P')	// COOP
					level_type_was = 0;
				else
					level_type_was = 1;

				level_type = level_type_was;
				m_CurrentMap = 0;
				m_Maps.SetCurrentMap(0);
				changed = false;
			}
		}

		if (key == SDLK_s)
		{
			if (level_type != level_type_was || filename[0] == '\0')
				key = SDLK_a;
			else if (CheckForSave())
			{
				LogFile::LogFile("Saving maps");
				m_Maps.Save(filename);
				level_type_was = level_type;
				changed = false;
			}
		}

		if (key == SDLK_a)
		{
			if (CheckForSave())
			{
				if (EnterFileName(filename))
				{
					char buf[512];
					char types[2][5] = { "COOP", "DUEL" };
#ifdef __linux__
					char *home = NjamGetHomeDir();
					snprintf(buf, sizeof(buf),
						"%s/.njam-levels/%s.%s",
						home? home:"",
#else
					snprintf(buf, sizeof(buf),
						"levels/%s.%s",
#endif					
						filename, types[level_type]);
					LogFile::LogFile("Saving maps");
					m_Maps.Save(buf);
					level_type_was = level_type;
					changed = false;
				}
			}
		}

		if (key == SDLK_t)	// test the level
		{
			if (!m_Maps.IsOk(MAPS))
				Message("LEVEL IS NOT MARKED AS PLAYABLE. PRESS P TO CHANGE.");
			else if (m_Maps.CountSpecificTiles(ttDoor) != 1)
				Message("THERE MUST BE EXACTLY ONE DOOR ON LEVEL!");
			else if (m_Maps.CountSpecificTiles(ttGHouse) != 1)
				Message("THERE MUST BE EXACTLY ONE PENTAGRAM ON LEVEL!");
			else
			{
				int used_skin_orig = m_GameOptions.UsedSkin;
				m_Ghosts = 8;
				bool AllowPowerups = true;
				if (level_type == 0)
				{
					m_Ghosts = 5;
					AllowPowerups = false;
				}

				// if random skin, then use first skin always
				if (m_GameOptions.UsedSkin == 0)
					m_GameOptions.UsedSkin = 1;

				m_Maps.RevertToMap(m_CurrentMap);	// make copy because playing the game will ruin it
				SetupGame();						// because of this undo isn't available anymore...
				m_Player[0].Playing = true;
				m_Player[1].Playing = true;
				m_Player[2].Playing = false;
				m_Player[3].Playing = false;
				m_GameType = gtDuel;
				PlayMap(AllowPowerups);
				m_Maps.SetCurrentMap(m_CurrentMap);	// get the data back
				m_GameOptions.UsedSkin = used_skin_orig;
			}
		}

		if (key == SDLK_u)
			m_Maps.SetCurrentMap(m_CurrentMap);

		if (key == SDLK_w)	// swap
		{
			if (m_SwapLevel == -1)					// turn swap on
				m_SwapLevel = m_CurrentMap;
			else if (m_SwapLevel == m_CurrentMap)	// turn swap off
				m_SwapLevel = -1;
			else									// do the swap and turn off
			{
				m_Maps.RevertToMap(m_CurrentMap);			// copy data back
				Message("LEVELS SWAPPED");
				m_Maps.SwapMaps(m_CurrentMap, m_SwapLevel);
				m_Maps.SetCurrentMap(m_CurrentMap);			// set the new buffer
				m_SwapLevel = -1;
				changed = true;
			}
		}

		if (key == SDLK_z && m_CurrentMap > 0)
		{
			m_Maps.RevertToMap(m_CurrentMap);	// copy data back
			m_CurrentMap--;
			m_Maps.SetCurrentMap(m_CurrentMap);
		}

		if (key == SDLK_x && m_CurrentMap < MAPS-1)
		{
			m_Maps.RevertToMap(m_CurrentMap);	// copy data back
			m_CurrentMap++;
			m_Maps.SetCurrentMap(m_CurrentMap);
		}

		if (key >= '0' && key <= '9')
		{
			changed = true;
			m_Maps.SetTile(x, y, (TileType)(key-'0'));
		}

		int oldx=x, oldy=y;

		if (key == SDLK_UP && y > 1)
			y--;
		if (key == SDLK_DOWN && y < MAPH-2)
			y++;
		if (key == SDLK_LEFT && x > 1)
			x--;
		if (key == SDLK_RIGHT && x < MAPW-2)
			x++;

		if (x == 1 || x == MAPW - 2)
		{
			if (y == 1 || y == MAPH - 2)
			{
				x = oldx;
				y = oldy;
			}
		}
	}
}
//-----------------------------------------------------------------------------
void NjamEngine::RenderEditor(bool changed, int x, int y, int level_type)
{
	SDL_Rect r, src;
	NjamSetRect(r, 700, 0, 100, 600);
	SDL_FillRect(m_Screen, &r, 0);
	m_Maps.RenderMap(m_Screen);

	// draw selector
	NjamSetRect(r, x*25-1, y*25-1, 27, 27);
	Uint32 FillColor = SDL_MapRGB(m_Screen->format, 255, 255, 127);
	SDL_FillRect(m_Screen, &r, FillColor);
	NjamSetRect(r, x*25+1, y*25+1);
	NjamSetRect(src, m_Maps.GetTile(x, y) * 25 + 1, 1, 23, 23);
	int current_skin = m_GameOptions.UsedSkin - 1;
	if( current_skin < 0 )
		current_skin = 0;
	SDL_BlitSurface(m_Skins[current_skin], &src, m_Screen, &r);

	// draw help options
	m_FontYellow->WriteTextColRow(m_Screen, 70, 0, "LEVELSET");
	m_FontYellow->WriteTextColRow(m_Screen, 70, 1, "L LOAD");
	m_FontYellow->WriteTextColRow(m_Screen, 70, 2, "S SAVE");
	m_FontYellow->WriteTextColRow(m_Screen, 70, 3, "A SAVE AS");
	m_FontYellow->WriteTextColRow(m_Screen, 70, 4, "K KIND");

	char buff[20];
	sprintf(buff, "LEVEL: %02d\0", m_CurrentMap + 1);
	m_FontYellow->WriteTextColRow(m_Screen, 70, 6, buff);
	m_FontYellow->WriteTextColRow(m_Screen, 70, 7, "P PLAYABLE");
	m_FontYellow->WriteTextColRow(m_Screen, 70, 8, "U UNDO");
	m_FontYellow->WriteTextColRow(m_Screen, 70, 9, "C CLEAR");
	m_FontYellow->WriteTextColRow(m_Screen, 70, 10, "T TEST");
	m_FontYellow->WriteTextColRow(m_Screen, 70, 11, "W SWAP");
	if (m_SwapLevel != -1)
	{
		char buff[20];
		sprintf(buff, ":%02d", m_SwapLevel + 1);
		m_FontYellow->WriteTextColRow(m_Screen, 76, 11, buff);
	}
	m_FontYellow->WriteTextColRow(m_Screen, 70, 12, "Z PREVIOUS");
	m_FontYellow->WriteTextColRow(m_Screen, 70, 13, "X NEXT");

	if (level_type == 0)
		m_FontBlue->WriteTextXY(m_Screen, 710, 250, "COOPERATIVE");
	else
		m_FontBlue->WriteTextXY(m_Screen, 725, 250, "DUEL");

	if (changed)
		m_FontBlue->WriteTextXY(m_Screen, 705, 270, "UNSAVED CHANGES");

	if (!m_Maps.IsOk(MAPS))
	{
		m_FontBlue->WriteTextXY(m_Screen, 705, 222, "LEVEL MARKED AS");
		m_FontBlue->WriteTextXY(m_Screen, 714, 232, "NOT PLAYABLE");
	}

	for (int i=0; i<10; i++)
	{
		sprintf(buff, "%d\0", i);
		m_FontYellow->WriteTextXY(m_Screen, 720, 297+i*30, buff);
		NjamSetRect(src, i*25, 0, 25, 25);
		NjamSetRect(r, 750, 292 + i*30);
		SDL_BlitSurface(m_Skins[current_skin], &src, m_Screen, &r);
	}

	NjamSetRect(r, 227, 583, 244, 13);
	FillColor = SDL_MapRGB(m_Screen->format, 170, 170, 210);
	SDL_FillRect(m_Screen, &r, FillColor);
	NjamSetRect(r, 228, 584, 242, 11);
	SDL_FillRect(m_Screen, &r, 0);
	m_FontBlue->WriteTextXY(m_Screen, 231, 585, "SEND YOUR LEVELS TO MBABUSKOV@YAHOO.COM");
}
//-----------------------------------------------------------------------------
void NjamEngine::Message(char *text)
{
	int l=0;
	while (text[l])
		l++;

	// render background
	Uint32 FillColor;
	SDL_Rect dest;
	FillColor = SDL_MapRGB(m_Screen->format, 0, 0, 0);
	NjamSetRect(dest, (800-l*10-50)/2, 275, l*10+50, 55);
	SDL_FillRect(m_Screen, &dest, FillColor);

	FillColor = SDL_MapRGB(m_Screen->format, 0, 120, 0);
	NjamSetRect(dest, (800-l*10-30)/2, 285, l*10+30, 35);
	SDL_FillRect(m_Screen, &dest, FillColor);

	m_FontYellow->WriteTextXY(m_Screen, (800-l*10)/2, 295, text);
	Render(m_Screen);
	NjamGetch(true);
}
//-----------------------------------------------------------------------------
bool NjamEngine::Query(char *text)
{
	int l=0;
	while (text[l])
		l++;

	// render background
	Uint32 FillColor;
	SDL_Rect dest;
	FillColor = SDL_MapRGB(m_Screen->format, 0, 0, 0);
	NjamSetRect(dest, (800-l*10-50)/2, 275, l*10+50, 55);
	SDL_FillRect(m_Screen, &dest, FillColor);

	FillColor = SDL_MapRGB(m_Screen->format, 0, 120, 0);
	NjamSetRect(dest, (800-l*10-30)/2, 285, l*10+30, 35);
	SDL_FillRect(m_Screen, &dest, FillColor);

	m_FontYellow->WriteTextXY(m_Screen, (800-l*10)/2, 295, text);
	Render(m_Screen);

	while (true)
	{
		SDL_Keycode k = NjamGetch(true);
		if (k == SDLK_y)
			return true;
		if (k == SDLK_n)
			return false;
	}
}
//-----------------------------------------------------------------------------
// returns true if eveything is ok
bool NjamEngine::CheckForSave()
{
	m_Maps.RevertToMap(m_CurrentMap);	// copy CURRENT data back
	char message[200];

	for (int i=0; i<MAPS; i++)
	{
		if (!m_Maps.IsOk(i))
			break;

		m_Maps.SetCurrentMap(i);
		if (m_Maps.CountSpecificTiles(ttDoor) != 1)
		{
			sprintf(message, "THERE MUST BE EXACTLY ONE DOOR ON LEVEL %d\0", i+1);
			Message(message);
			m_Maps.SetCurrentMap(m_CurrentMap);
			return false;
		}

		if (m_Maps.CountSpecificTiles(ttGHouse) != 1)
		{
			sprintf(message, "THERE MUST BE EXACTLY ONE PENTAGRAM ON LEVEL %d\0", i+1);
			Message(message);
			m_Maps.SetCurrentMap(m_CurrentMap);
			return false;
		}
	}
	m_Maps.SetCurrentMap(m_CurrentMap);
	return Query("ARE YOU SURE (Y/N) ?");
}
//-----------------------------------------------------------------------------
bool NjamEngine::EnterFileName(char *file_name)
{
	char filename[30];
	Uint32 white = SDL_MapRGB(m_Screen->format, 255, 255, 255);
	Uint32 t = SDL_GetTicks();
	bool CursorOn = true;

	filename[0] = '\0';
	while (true)
	{
		SDL_Rect dest;
		NjamSetRect(dest, 249, 224);
		SDL_BlitSurface(m_LevelsetImage, NULL, m_Screen, &dest);
		m_FontYellow->WriteTextXY(m_Screen, 274, 303, filename);

		// get len
		int l = 0;
		while (filename[l])
			l++;

		if (SDL_GetTicks() > t + 400)	// make the cursor blink
		{
			t = SDL_GetTicks();
			CursorOn = !CursorOn;
		}

		if (CursorOn)
		{
			NjamSetRect(dest, 276 + 10 * l, 303, 10, 15);	// draw cursor
			SDL_FillRect(m_Screen, &dest, white);
		}

		Render(m_Screen);

		SDL_Keycode key = NjamGetch(false);
		char allowed[] = "0123456789abcdefghijklmnopqrstuvwxyz ";
		bool ok = false;
		for (int k=0; allowed[k]; k++)
			if (allowed[k] == key)
				ok = true;

		if (ok && l < 23)
		{
			char c = key;
			if (c >= 'a' && c <= 'z')
				c -= ('a' - 'A');
			filename[l] = c;
			filename[l+1] = '\0';
		}

		if (key == SDLK_ESCAPE)
			return false;

		if (key == SDLK_BACKSPACE && l > 0)			// backspace
			filename[l-1] = '\0';

		if ((key == SDLK_KP_ENTER || key == SDLK_RETURN) && filename[0])	// entered
		{
			sprintf(file_name, "%s\0", filename);
			return true;
		}
	}
}
//---------------------------------------------------------------------------
int NjamEngine::SelectMap(char type, char *filename, int filename_size)
{
	struct list_item *first = NULL;

	// open folders, load all entries
	DIR *dir;
	struct dirent *ent;
	int i;
	char dirname[512] = "levels";

#ifdef __linux__
	for (i=0; i<2; i++)
	{
		if (i==0)
		{
#endif
			if ((dir = opendir(dirname)) == NULL)
			{
				printf("Unable to open directory\n");
				return -1;
			}
#ifdef __linux__
		}
		else
		{
			char *home = NjamGetHomeDir();

			snprintf(dirname, sizeof(dirname), "%s/.njam-levels",
				home? home:"");
			if ((dir = opendir(dirname)) == NULL)
				break;
		}
#endif
		while ((ent = readdir(dir)) != NULL)
		{
			// check if last letter of name maches:
			int p = 0;
			while (ent->d_name[p])
				p++;

			char c = ent->d_name[p-1];
			if (c != 'P' && c != 'L')
				continue;

			if (type != 'A' && c != type)
				continue;

			struct list_item *new_file = new struct list_item;
			new_file->next = first;
			new_file->prev = NULL;
			if (first)
				first->prev = new_file;
			first = new_file;

			// copy file name
			snprintf(first->item_text, sizeof(first->item_text),
			  "%s", ent->d_name);
			snprintf(first->item_path, sizeof(first->item_path),
			  "%s/%s", dirname, ent->d_name);
		}

		if (closedir(dir) != 0)
			printf("Unable to close directory\n");
#ifdef __linux__
	}
#endif

	// show and let player select if more than one levelset
	struct list_item *selected;
	if (first->next)	// there is more than 1 levelset
		selected = SelectFromList(first);
	else
		selected = first;

	int result = 0;	// no levels loaded (so far)
	if (selected)
	{
		result = m_Maps.Load(selected->item_path);
		if (filename)
			snprintf(filename, filename_size, "%s",
				selected->item_path);
	}

	while (first)	// free memory
	{
		struct list_item *temp = first;
		first = first->next;
		delete temp;
	}

	return result;
}
//-----------------------------------------------------------------------------
struct list_item *NjamEngine::SelectFromList(struct list_item *first)
{
	if (!first)
		return NULL;

	struct list_item *visible = first;
	struct list_item *selected = first;

	while (true)
	{
		// render background, FIXME: make nice bitmap for this
		Uint32 FillColor;
		SDL_Rect dest;			// thick black border (1px)
		FillColor = SDL_MapRGB(m_Screen->format, 0, 0, 0);
		NjamSetRect(dest, 240, 175, 320, 250);
		SDL_FillRect(m_Screen, &dest, FillColor);

		FillColor = SDL_MapRGB(m_Screen->format, 230, 200, 100);	// whole
		NjamSetRect(dest, 241, 176, 318, 248);
		SDL_FillRect(m_Screen, &dest, FillColor);

		// title
		FillColor = SDL_MapRGB(m_Screen->format, 100, 0, 0);
		NjamSetRect(dest, 249, 184, 302, 22);
		SDL_FillRect(m_Screen, &dest, FillColor);

		m_FontYellow->WriteTextXY(m_Screen, 325, 187, "SELECT LEVELSET");

		// filelist
		FillColor = SDL_MapRGB(m_Screen->format, 80, 50, 0);
		NjamSetRect(dest, 249, 214, 302, 202);
		SDL_FillRect(m_Screen, &dest, FillColor);

		// render items
		struct list_item *temp = visible;
		int current = 0;
		while (temp && current < 10)	// ten items on screen allowed
		{
			if (temp == selected)
			{
				FillColor = SDL_MapRGB(m_Screen->format, 120, 80, 0);
				NjamSetRect(dest, 250, 215+20*current, 300, 20);
				SDL_FillRect(m_Screen, &dest, FillColor);
			}

			m_FontYellow->WriteTextXY(m_Screen, 252, 216+20*current, temp->item_text);
			temp = temp->next;
			current++;
		}

		Render(m_Screen);

		// wait for a keypress
        SDL_Keycode key = NjamGetch(true);				// true = wait for keypress
		if (key == SDLK_ESCAPE)
			return NULL;

		if ((key == SDLK_KP_ENTER  || key == SDLK_RETURN))
			break;

		if (key == SDLK_DOWN)
		{
			if (selected->next)
			{
				selected = selected->next;
				if (current == 10 && selected == temp)	// out of screen - move the top
					visible = visible->next;
			}
		}

		if (key == SDLK_UP)
		{
			if (selected->prev)
			{
				if (selected == visible)		// over the top
					visible = selected->prev;
				selected = selected->prev;
			}
		}
	}

	return selected;
}
//-----------------------------------------------------------------------------
