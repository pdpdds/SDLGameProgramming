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
#include "../json/cJSON.h"

#define FONT_SIZE		48

extern void blitAtlasImage(AtlasImage *atlasImage, int x, int y, int center);
extern void blitAtlasImageRotated(AtlasImage *atlasImage, int x, int y, int center, float angle);
extern void calcTextDimensions(char *text, int size, int *w, int *h);
extern int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
extern void drawFilledRect(int x, int y, int w, int h, int r, int g, int b, int a);
extern void drawRect(int x, int y, int w, int h, int r, int g, int b, int a);
extern void drawShadowText(int x, int y, int align, int size, const char *format, ...);
extern char **getFileList(const char *dir, int *count);
extern AtlasImage *getImageFromAtlas(char *filename, int required);
extern long lookup(const char *name);
extern char *readFile(const char *filename);
extern void setTextColor(int r, int g, int b, int a);
extern float wrap(float value, float low, float high);

extern App app;
