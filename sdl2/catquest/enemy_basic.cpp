#include "enemy_basic.h"

EnemyBasic::EnemyBasic(int x, int y, Sprite* spr) {
	sprite = spr;
	init(x,y);
	danger = (Danger*)Gamebase::addObject<Danger>(pos.x, pos.y + 10)->resize(bbox.w, bbox.h - 10);
	spd.x = -2;
	setDepth(-1);
}

void EnemyBasic::step() {
	SDL_Point off = {spd.x, 0};
	if(Gamebase::checkCollision<Wall>(this, off)) {
		reverse();
	}

	off.x = 0;
	off.y = 1;
	if(!Gamebase::checkCollision<Wall>(this, spd + off)) {
		spd.y++;
		if(spd.y > 10) { spd.y = 10; }
	}

	Object* wall = Gamebase::instCollision<Wall>(this, spd + off);
	if (wall == NULL) {
		wall = Gamebase::instCollision<WallPass>(this, spd + off);
	}
	if(wall != NULL) {
		if(spd.y > 0) {
			pos.y = wall->getY() - bbox.h;
		}
		spd.y = 0;
	}

	pos.x += spd.x;
	pos.y += spd.y;
	if(pos.y > Room::h * TILE_SIZE) { 
		die();
		return;
	}
	danger->move(pos.x, pos.y + 10);
	if(spd.x < 0) {
		setAnim(0);
	} else {
		setAnim(1);
	}
}

void EnemyBasic::reverse() {
	spd.x = -spd.x;
}

void EnemyBasic::die() {
	Gamebase::removeObject(danger);
	Gamebase::removeObject(this);
}

Danger* EnemyBasic::getDanger() {
	return danger;
}