#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <vector>
#include <string>
#include "gameobject.h"
#include "animation.h"

class Sprite : public GameObject {
	SDL_Texture* texture;
	std::vector<SDL_Rect> frames;
	std::vector<Animation> anim;

	int width;
	int height;
	SDL_Point* origin;
public:
	Sprite(std::string url = "", int = 0, int = 0);
	virtual ~Sprite();
	void free();
	void change(std::string, int = 0, int = 0);
	Sprite* split(int, int, int, int = 0, int = 0, int = 0, int = 0, int = -1);

	void draw(int, int, int, int = -1, int = -1);
	void draw(int, SDL_Rect);
	void draw_ext(int, SDL_Rect, double);
	void draw_raw(int, int, int = -1, int = -1);

	void colorMod(Uint8, Uint8, Uint8);
	void colorMod(Uint8[3]);
	void alphaMod(Uint8);
	void setBlendMode(SDL_BlendMode);

	Sprite* addAnim(int, int, int = 30);
	Sprite* addAnim(std::string, int = 30);
	void removeAnim(int);

	SDL_Texture* getTexture();
	SDL_Rect* getFrame(int);
	Animation* getAnim(int);
	int getWidth();
	int getHeight();
	SDL_Point* getOrigin();
};

#endif
