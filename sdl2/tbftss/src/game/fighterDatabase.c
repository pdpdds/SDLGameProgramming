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

#include "fighterDatabase.h"

static void prevFighter(void);
static void nextFighter(void);
static int countFighterGuns(Entity *fighter, int type);
static void setNumDestroyed(void);

static int page;
static int maxPages;
static int numDestroyed;
static Widget *prev;
static Widget *next;
static char *DB_TEXT;
static char *PAGE_TEXT;
static char *COMMON_TEXT;
static char *DESTROYED_TEXT;
static char *AFFILIATION_TEXT;
static char *ARMOUR_TEXT;
static char *SHIELD_TEXT;
static char *SPEED_TEXT;
static char *MISSILES_TEXT;
static char *MISSILE_NUM_TEXT;
static const char *gunName[BT_MAX];
static Entity **dbFighters;
static float rotation;

void initFighterDatabase(void)
{
	DB_TEXT = _("Fighter Database");
	PAGE_TEXT = _("Page %d / %d");
	
	COMMON_TEXT = _("(Common)");
	DESTROYED_TEXT = _("Destroyed");
	AFFILIATION_TEXT = _("Affiliation");
	ARMOUR_TEXT = _("Armour");
	SHIELD_TEXT = _("Shield");
	SPEED_TEXT = _("Speed");
	MISSILES_TEXT = _("Missiles");
	MISSILE_NUM_TEXT = _("Missiles x %d");
	
	dbFighters = getDBFighters(&maxPages);
	
	gunName[BT_NONE] = "";
	gunName[BT_PARTICLE] = _("Particle Cannon");
	gunName[BT_PLASMA] = _("Plasma Cannon");
	gunName[BT_LASER] = _("Laser Cannon");
	gunName[BT_MAG] = _("Mag Cannon");
	gunName[BT_ROCKET] = _("Rockets");
	gunName[BT_MISSILE] = _("Missiles");
	
	rotation = 0;
}

void destroyFighterDatabase(void)
{
	free(dbFighters);
}

void initFighterDatabaseDisplay(void)
{
	page = 0;
	
	prev = getWidget("prev", "fighterDB");
	prev->action = prevFighter;
	
	next = getWidget("next", "fighterDB");
	next->action = nextFighter;
	
	setNumDestroyed();
}

void doFighterDatabase(void)
{
	rotation++;
}

void drawFighterDatabase(void)
{
	SDL_Rect r;
	Entity *fighter;
	int i, y, numCannons;
	
	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 128);
	SDL_RenderFillRect(app.renderer, NULL);
	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_NONE);
	
	SDL_SetRenderTarget(app.renderer, app.uiBuffer);
	
	r.w = 700;
	r.h = 650;
	r.x = (UI_WIDTH / 2) - r.w / 2;
	r.y = (UI_HEIGHT / 2) - r.h / 2;
	
	SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(app.renderer, &r);
	SDL_SetRenderDrawColor(app.renderer, 200, 200, 200, 255);
	SDL_RenderDrawRect(app.renderer, &r);
	
	drawText(UI_WIDTH / 2, 50, 28, TA_CENTER, colors.white, DB_TEXT);
	
	drawText(UI_WIDTH / 2, 90, 16, TA_CENTER, colors.lightGrey, PAGE_TEXT, page + 1, (int)maxPages);
	
	fighter = dbFighters[page];
	
	drawText(UI_WIDTH / 2, 130, 28, TA_CENTER, colors.white, fighter->name);
	
	blitRotated(fighter->texture, r.x + (r.w / 2), 250, rotation);
	
	if (fighter->flags & EF_COMMON_FIGHTER)
	{
		drawText(UI_WIDTH / 2, 170, 18, TA_CENTER, colors.darkGrey, COMMON_TEXT);
		
		drawText(r.x + (r.w / 2), 290, 18, TA_CENTER, colors.lightGrey, "%s: %d", DESTROYED_TEXT, numDestroyed);
	}
	
	drawText(r.x + 25, 200, 22, TA_LEFT, colors.white, "%s: %s", AFFILIATION_TEXT, fighter->affiliation);
	drawText(r.x + 25, 240, 22, TA_LEFT, colors.white, "%s: %d", ARMOUR_TEXT, fighter->health);
	drawText(r.x + 25, 280, 22, TA_LEFT, colors.white, "%s: %d", SHIELD_TEXT, fighter->shield);
	drawText(r.x + 25, 320, 22, TA_LEFT, colors.white, "%s: %.0f", SPEED_TEXT, ((fighter->speed * fighter->speed) * FPS));
	
	y = 200;
	
	for (i = 1 ; i < BT_MAX ; i++)
	{
		numCannons = countFighterGuns(fighter, i);
		if (numCannons > 0)
		{
			drawText(r.x + r.w - 25, y, 22, TA_RIGHT, colors.white, "%s x %d", gunName[i], numCannons);
			
			y += 40;
		}
	}
	
	if (fighter->missiles > 0)
	{
		drawText(r.x + r.w - 25, y, 22, TA_RIGHT, colors.white, MISSILE_NUM_TEXT, fighter->missiles);
	}
	
	y = MAX(y, 320) + 75;
	
	app.textWidth = r.w - 50;
	
	drawText(r.x + 25, y, 18, TA_LEFT, colors.white, fighter->description);
	
	app.textWidth = 0;
	
	drawWidgets("fighterDB");
	
	SDL_SetRenderTarget(app.renderer, app.backBuffer);
}

static int countFighterGuns(Entity *fighter, int type)
{
	int i, num;
	
	num = 0;
	
	for (i = 0 ; i < MAX_FIGHTER_GUNS ; i++)
	{
		if (fighter->guns[i].type == type)
		{
			num++;
		}
	}
	
	return num;
}

static void prevFighter(void)
{
	page = mod(page - 1, maxPages);
	
	setNumDestroyed();
}

static void nextFighter(void)
{
	page = mod(page + 1, maxPages);
	
	setNumDestroyed();
}

static void setNumDestroyed(void)
{
	Tuple *t;
	Entity *fighter;
	
	fighter = dbFighters[page];
	
	numDestroyed = 0;
	
	for (t = game.fighterStatHead.next ; t != NULL ; t = t->next)
	{
		if (strcmp(t->key, fighter->name) == 0)
		{
			numDestroyed = t->value;
			return;
		}
	}
}
