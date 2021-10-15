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

#define MAX_BOUNCERS		5
#define MAX_BOUNCER_TYPES	8

extern void blitAtlasImage(AtlasImage *atlasImage, int x, int y, int center);
extern void doWidgets(void);
extern void doWipe(void);
extern void drawBackground(Background *background);
extern void drawShadowText(int x, int y, int align, int size, const char *format, ...);
extern void drawWidgets(void);
extern void drawWipe(void);
extern AtlasImage *getImageFromAtlas(char *filename, int required);
extern Widget *getWidget(const char *name, const char *group);
extern void initCredits(void);
extern void initLevelSelect(void);
extern void initOptions(void);
extern void initStats(void);
extern void initWipe(int type);
extern void loadMusic(const char *filename);
extern Texture *loadTexture(const char *filename);
extern void playMusic(int loop);
extern void playSound(int snd, int ch);
extern void setTextColor(int r, int g, int b, int a);
extern void showWidgetGroup(const char *name);

extern App app;
