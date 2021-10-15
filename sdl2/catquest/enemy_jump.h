#ifndef __ENEMY_JUMP_H__
#define __ENEMY_JUMP_H__

#include "object.h"
#include "danger.h"
#include "wall.h"
#include "room.h"

class EnemyJump : public Object {
private:
	Danger* danger;
	int timer;
public:
	EnemyJump(int, int, Sprite* = Gamebase::getSprite(Gamebase::sprite::frog));

	void step();
	void die();
	Danger* getDanger();
};

#endif