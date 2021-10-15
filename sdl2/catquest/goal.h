#ifndef __GOAL_H__
#define __GOAL_H__

#include "object.h"
#include "gamebase.h"

class Goal : public Object {
public:
	Goal(int, int, Sprite* = Gamebase::getSprite(Gamebase::sprite::goal));
};

#endif