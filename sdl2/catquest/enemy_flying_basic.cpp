#include "enemy_flying_basic.h"

EnemyFlyingBasic::EnemyFlyingBasic(int x, int y, Sprite* spr) {
	sprite = spr;
	init(x,y);
	danger = (Danger*)Gamebase::addObject<Danger>(pos.x, pos.y + 10)->resize(bbox.w, bbox.h - 10);
	spd.x = -2;
	setDepth(-1);
}

void EnemyFlyingBasic::step() {
	SDL_Point off = {spd.x, 0};
	if(Gamebase::checkCollision<Wall>(this, off)) {
		reverse();
	}

	pos.x += spd.x;
	if(pos.x < -TILE_SIZE) { 
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

void EnemyFlyingBasic::reverse() {
	spd.x = -spd.x;
}

void EnemyFlyingBasic::die() {
	Gamebase::removeObject(danger);
	Gamebase::removeObject(this);
}

Danger* EnemyFlyingBasic::getDanger() {
	return danger;
}