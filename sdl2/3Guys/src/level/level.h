/*
Copyright (C) 2018 Parallel Realities

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

enum
{
	SHOW_LEVEL,
	SHOW_TIPS,
	SHOW_PAUSE
};

extern void animateSprites(void);
extern void blitAtlasImage(AtlasImage *atlasImage, int x, int y, int center);
extern void blitAtlasImageRotated(AtlasImage *atlasImage, int x, int y, int center, float angle);
extern void clearRoute(void);
extern void destroyEntities(void);
extern void doEffects(void);
extern void doEntities(void);
extern void doPlayer(void);
extern void doWidgets(void);
extern int doWipe(void);
extern void drawBackground(Background *background);
extern void drawEffects(void);
extern void drawEntities(int backgroundPlane);
extern void drawFilledRect(int x, int y, int w, int h, int r, int g, int b, int a);
extern void drawRect(int x, int y, int w, int h, int r, int g, int b, int a);
extern void drawShadowText(int x, int y, int align, int size, const char *format, ...);
extern void drawText(int x, int y, int align, int size, const char *format, ...);
extern void drawWidgets(void);
extern void drawWipe(void);
extern float getAngle(int x1, int y1, int x2, int y2);
extern void getEntitiesAt(int x, int y, int *n, Entity *ignore, Entity **candidates);
extern AtlasImage *getImageFromAtlas(char *filename, int required);
extern Widget *getWidget(const char *name, const char *group);
extern void guyTouchOthers(void);
extern void initEffects(void);
extern void initEnding(void);
extern void initEntities(void);
extern void initLevelSelect(void);
extern void initOptions(void);
extern void initPlayer(void);
extern void initWipe(int type);
extern int loadLevel(int n);
extern void loadMusic(const char *filename);
extern Texture *loadTexture(const char *filename);
extern void moveEntities(void);
extern void playMusic(int loop);
extern void playSound(int snd, int ch);
extern void saveGame(void);
extern void setTextColor(int r, int g, int b, int a);
extern void setTextWidth(int width);
extern void showWidgetGroup(const char *name);
extern void updateStar(void);

extern App app;
extern Entity *self;
extern Game game;
extern Level level;
