/*
Copyright (C) 2015-2019 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "../common.h"

#include "../json/cJSON.h"

extern void addHudMessage(SDL_Color c, char *format, ...);
extern int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
extern long flagsToLong(char *flags, int *add);
extern Entity **getAllEntsInRadius(int x, int y, int radius, Entity *ignore);
extern AtlasImage *getAtlasImage(char *filename);
extern int getJSONValue(cJSON *node, char *name, int defValue);
extern char *getJSONValueStr(cJSON *node, char *name, char *defValue);
extern void playBattleSound(int id, int x, int y);
extern char *readFile(char *filename);
extern Entity *spawnEntity(void);
extern void updateObjective(char *name, int type);

extern Battle battle;
extern Colors colors;
extern Entity *player;
extern Entity *self;
