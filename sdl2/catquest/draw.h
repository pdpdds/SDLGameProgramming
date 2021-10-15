#ifndef __DRAW_H__
#define __DRAW_H__

#include <SDL2/SDL.h>
#include "gameobject.h"
#include "sprite.h"
#include "font.h"
#include "gamebase.h"

struct Draw {
	static SDL_Color c_color;

	static void setColor(int, int, int, int);
	static void setColor(SDL_Color);

	static void sprite_sheet(Sprite*, int, int, int = -1, int = -1);
	static void sprite(Sprite*, int, int, int, int, int, double = 0);
	static void rect(int, int, int, int, bool = false);
	static void rect(SDL_Rect, bool = false);
	static void line(int, int, int, int);
	static void point(int, int);
	static void text(int, int, std::string, Font*);
	static void clear(int = 255, int = 255, int = 255);
};

#endif
