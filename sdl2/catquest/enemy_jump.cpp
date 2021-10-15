#include "enemy_jump.h"

EnemyJump::EnemyJump(int x, int y, Sprite* spr) {
	sprite = spr;
	init(x,y);
	danger = (Danger*)Gamebase::addObject<Danger>(pos.x, pos.y + 16)->resize(bbox.w, bbox.h - 16);
	setDepth(-1);
	timer = 45;
}

void EnemyJump::step() {
	SDL_Point off = {0,1};
	if (Gamebase::checkCollision<Wall>(this, off)) {
		if (spd.y != 0) {
			spd.y = 0;
		}
		if(!--timer) {
			spd.y = -14;
			timer = 45;
		}
	} else {
			spd.y++;
			if (spd.y > 10) {
				spd.y = 10;
			}
			off.x = 0;
			off.y = spd.y;
			Wall* wall = Gamebase::instCollision<Wall>(this, off);
			if (wall != NULL) {
				if (spd.y > 0) {
					pos.y = wall->getY() - bbox.h;
				} else if (spd.y < 0) {
					pos.y = wall->getY() + wall->getH();
				}
				spd.y = 0;
			}
	}

	pos += spd;
	danger->move(pos.x, pos.y + 16);
	if(spd.y < -4) {
		image_index = 0;
	} else if(spd.y > 4) {
		image_index = 2;
	} else {
		image_index = 1;
	}
}

void EnemyJump::die() {
	Gamebase::removeObject(danger);
	Gamebase::removeObject(this);
}

Danger* EnemyJump::getDanger() {
	return danger;
}