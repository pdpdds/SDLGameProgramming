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

#include "time.h"

extern void activateNextWaypoint(void);
extern void addAllToQuadtree(void);
extern void addEpicKillsObjective(void);
extern void addEpicLivesObjective(void);
extern void awardCraftTrophy(void);
extern void awardPostMissionTrophies(void);
extern void awardStatsTrophies(void);
extern void completeConditions(void);
extern void countNumEnemies(void);
extern void endSectionTransition(void);
extern void failIncompleteObjectives(void);
extern long flagsToLong(char *flags, int *add);
extern AtlasImage *getAtlasImage(char *filename);
extern char *getBackgroundTextureName(unsigned long n);
extern int getJSONValue(cJSON *node, char *name, int defValue);
extern char *getJSONValueStr(cJSON *node, char *name, char *defValue);
extern char *getMusicFilename(unsigned long n);
extern char *getPlanetTextureName(unsigned long n);
extern SDL_Texture *getTexture(char *filename);
extern void initMissionInfo(void);
extern void initPlayer(void);
extern void initScript(cJSON *missionJSON);
extern void loadCapitalShips(cJSON *node);
extern void loadChallenge(Mission *mission, cJSON *node);
extern void loadFighters(cJSON *node);
extern void loadItems(cJSON *node);
extern void loadLocations(cJSON *node);
extern void loadObjectives(cJSON *node);
extern void loadPlayer(cJSON *node);
extern void loadSpawners(cJSON *node);
extern long lookup(char *name);
extern void playMusic(char *filename, int loop);
extern char *readFile(char *filename);
extern void retreatEnemies(void);
extern void selectWidget(const char *name, const char *group);
extern void setInitialPlayerAngle(void);
extern Entity *spawnJumpgate(int side, long flags);
extern Entity *spawnMine(int type);
extern Entity *spawnWaypoint(void);
extern void startSectionTransition(void);
extern void stopMusic(void);
extern void updateChallengeMissions(void);
extern void updateStarSystemMissions(void);

extern App app;
extern Battle battle;
extern Dev dev;
extern Entity *player;
extern Game game;
