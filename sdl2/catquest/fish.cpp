#include "fish.h"

Fish::Fish(int x, int y, Sprite* spr) {
	sprite = spr;
	init(x,y);
	yy = 0;
	dy = 1;
	timer = 6;
}

void Fish::step() {
	if(timer-- == 0) {
		if(yy == -2) {
			dy = 1;
		} else if(yy == 2) {
			dy = -1;
		}
		timer = 6;
		yy += dy;
	}
}

void Fish::draw() {
	SDL_Point off = {0, yy};
	SDL_Rect wh = {0, 0, sprite->getWidth(), sprite->getHeight()};
	sprite->draw_ext(0, ((wh | pos) + off) * image_scale, image_angle);
}
