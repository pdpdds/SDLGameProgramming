#ifndef __FONT_H__
#define __FONT_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <stdio.h>
#include "gameobject.h"

class Font : public GameObject {
	TTF_Font *font;
public:
	Font(std::string, int);
	~Font();
	TTF_Font* getFont();

	void free();
	void change(std::string, int);
};

#endif
