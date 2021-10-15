#ifndef __WALL_H__
#define __WALL_H__

#include "object.h"
#include "room.h"

class Wall : public Object {
private:
	int image_parts[6];
public:
	Wall(int, int, Sprite* = Gamebase::getSprite(Gamebase::sprite::wall));
	
	void roomStart();
	void chooseWalls();
	void draw();
	void step();
};

#endif