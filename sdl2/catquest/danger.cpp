#include "danger.h"

Danger::Danger(int x, int y, Sprite* spr) {
	sprite = spr;
	init(x,y);
}

void Danger::move(int x, int y) {
	pos.x = x;
	pos.y = y;
}