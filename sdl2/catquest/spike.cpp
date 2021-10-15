#include "spike.h"

Spike::Spike(int x, int y, Sprite* spr) {
	sprite = spr;
	init(x,y);
	danger = (Danger*)Gamebase::addObject<Danger>(pos.x, pos.y - sprite->getOrigin()->y)->resize(bbox.w, bbox.h);
	setDepth(-2);
}