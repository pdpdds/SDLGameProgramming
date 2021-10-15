#ifndef __ENEMY_BASIC_H__
#define __ENEMY_BASIC_H__

#include "object.h"
#include "danger.h"
#include "wall.h"
#include "room.h"

class EnemyBasic : public Object {
private:
	Danger* danger;
	
public:
	EnemyBasic(int, int, Sprite* = Gamebase::getSprite(Gamebase::sprite::dog));

	void step();
	void reverse();
	void die();
	Danger* getDanger();
};

#endif