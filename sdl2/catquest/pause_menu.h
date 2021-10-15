#ifndef __PAUSE_MENU_H__
#define __PAUSE_MENU_H__

#include "object.h"
#include "gamebase.h"
#include "room.h"

class PauseMenu : public Object {
	int c_opt;
public:
	PauseMenu(int, int, Sprite* = Gamebase::getSprite(Gamebase::sprite::point));

	void step();
	void draw();
};

#endif