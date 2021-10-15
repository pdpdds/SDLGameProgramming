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

extern void activateEntities(char *name);
extern void activateEntityGroups(char *groupName);
extern void activateJumpgate(int activate);
extern void activateLocations(char *locations);
extern void activateNextWaypoint(void);
extern void activateObjectives(char *objectives);
extern void activateSpawner(char *name, int active);
extern void activateTrespasserSpawner(void);
extern void addHudMessage(SDL_Color c, char *format, ...);
extern void addMessageBox(char *title, char *body, int type);
extern void completeAllObjectives(void);
extern void completeMission(void);
extern void createChristabelLocation(void);
extern void failMission(void);
extern void killEntity(char *name);
extern void retreatAllies(void);
extern void retreatEnemies(void);
extern int showingMessageBoxes(void);
extern void updateEntitySide(char *side, char *entity);

extern Battle battle;
extern Colors colors;
