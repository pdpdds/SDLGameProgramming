#include "wall_pass.h"

WallPass::WallPass(int x, int y, Sprite* spr) {
	sprite = spr;
	init(x, y);
	wall = NULL;
	setDepth(2);
}

WallPass::~WallPass() {
	if (wall != NULL) {
		Gamebase::removeObject(wall);
		wall = NULL;
	}
}

void WallPass::step() {
	if (Gamebase::checkKey(SDL_SCANCODE_DOWN)) {
		if (wall != NULL) {
			Gamebase::removeObject(wall);
			wall = NULL;
		}
	} else {
		std::vector<Player*> objs = Gamebase::getObjects<Player>();
		if (objs.size()) {
			Player * player = objs[0];
			int dp = (int)std::sqrt(std::pow(player->getX() - pos.x, 2.0) + std::pow(player->getY() - pos.y, 2.0));
			if (player->getY() + player->getH() <= pos.y && dp < 50) {
				if (wall == NULL) {
					wall = Gamebase::addObject<Wall>(pos.x, pos.y)->setVisible(false);
				}
			} else {
				if (wall != NULL) {
					Gamebase::removeObject(wall);
					wall = NULL;
				}
			}
		}
	}
}