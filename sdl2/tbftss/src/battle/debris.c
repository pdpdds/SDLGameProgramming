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

#include "debris.h"

static void changeCourse(Debris *d);
static void resizeDrawList(void);

static Debris **debrisToDraw;
static AtlasImage *debrisTexture[MAX_DEBRIS_TEXTURES];
static int drawCapacity;

void initDebris(void)
{
	debrisTexture[0] = getAtlasImage("gfx/debris/debris1.png");
	debrisTexture[1] = getAtlasImage("gfx/debris/debris2.png");
	debrisTexture[2] = getAtlasImage("gfx/debris/debris3.png");
	debrisTexture[3] = getAtlasImage("gfx/debris/debris4.png");
	debrisTexture[4] = getAtlasImage("gfx/debris/debris5.png");
	debrisTexture[5] = getAtlasImage("gfx/debris/debris6.png");

	drawCapacity = INITIAL_DEBRIS_DRAW_CAPACITY;

	debrisToDraw = malloc(sizeof(Bullet*) * drawCapacity);
	memset(debrisToDraw, 0, sizeof(Bullet*) * drawCapacity);
}

void addDebris(int x, int y, int amount)
{
	int i;
	Debris *d;

	for (i = 0 ; i < amount ; i++)
	{
		d = malloc(sizeof(Debris));
		memset(d, 0, sizeof(Debris));
		battle.debrisTail->next = d;
		battle.debrisTail = d;

		d->x = x;
		d->y = y;
		d->dx = rand() % 1000 - rand() % 1000;
		d->dx *= 0.01;
		d->dy = rand() % 1000 - rand() % 1000;
		d->dy *= 0.01;
		d->health = FPS + (FPS * (rand() % 3));

		d->texture = debrisTexture[rand() % MAX_DEBRIS_TEXTURES];
	}
}

void doDebris(void)
{
	int i;
	Debris *d, *prev;

	memset(debrisToDraw, 0, sizeof(Debris*) * drawCapacity);

	prev = &battle.debrisHead;

	i = 0;

	for (d = battle.debrisHead.next ; d != NULL ; d = d->next)
	{
		d->x += d->dx;
		d->y += d->dy;

		if (--d->thinkTime <= 0)
		{
			changeCourse(d);
		}

		d->angle = mod(d->angle + 1, 360);

		addDebrisFire(d->x, d->y);

		if (--d->health <= 0)
		{
			if (d == battle.debrisTail)
			{
				battle.debrisTail = prev;
			}

			prev->next = d->next;
			free(d);
			d = prev;
		}
		else
		{
			if (isOnBattleScreen(d->x, d->y, 32, 32))
			{
				debrisToDraw[i++] = d;

				if (i == drawCapacity)
				{
					resizeDrawList();
				}
			}
		}

		prev = d;
	}
}

static void resizeDrawList(void)
{
	int n;

	n = drawCapacity + INITIAL_DEBRIS_DRAW_CAPACITY;

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Resizing debris draw capacity: %d -> %d", drawCapacity, n);

	debrisToDraw = resize(debrisToDraw, sizeof(Debris*) * drawCapacity, sizeof(Debris*) * n);
	drawCapacity = n;
}

static void changeCourse(Debris *d)
{
	float dir;

	dir = rand() % 25 - rand() % 25;
	dir *= 0.01;
	d->dx += dir;

	dir = rand() % 25 - rand() % 25;
	dir *= 0.01;
	d->dy += dir;

	d->thinkTime = 1 + (rand() % 5);
}

void drawDebris(void)
{
	int i;
	Debris *d;

	for (i = 0, d = debrisToDraw[i] ; d != NULL ; d = debrisToDraw[++i])
	{
		blitRotated(d->texture, d->x - battle.camera.x, d->y - battle.camera.y, d->angle);
	}
}

void destroyDebris(void)
{
	free(debrisToDraw);

	debrisToDraw = NULL;
}
