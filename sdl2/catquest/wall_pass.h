#ifndef __WALL_PASS_H__
#define __WALL_PASS_H__

#include "object.h"
#include "wall.h"

class WallPass : public Object {
private:
	Object* wall;
public:
	WallPass(int, int, Sprite* = Gamebase::getSprite(Gamebase::sprite::pass_wall));
	~WallPass();
	void step();
};

#endif