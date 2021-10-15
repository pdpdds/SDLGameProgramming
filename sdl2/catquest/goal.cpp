#include "goal.h"

Goal::Goal(int x, int y, Sprite* spr) {
	sprite = spr;
	init(x,y);
	setAnim(0);
}