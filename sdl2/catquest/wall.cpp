#include "wall.h"

Wall::Wall(int x, int y, Sprite* spr) {
	sprite = spr;
	init(x,y);
	image_index = -1;
	bbox.w = 32;
	bbox.h = 32;
	setDepth(1);
}
void Wall::chooseWalls() {
	bool sur[3][3];
	for(int i = -1; i <= 1; i++) {
		for(int j = -1; j <= 1; j++) {
			if(i != 0 || j != 0) {
				SDL_Point off = {i * 32, j * 32};
				sur[i+1][j+1] = Gamebase::checkCollision<Wall>(this, off, NULL, true);
			}
		}
	}
	if(pos.x == 0) {
		sur[0][0] = sur[0][1] = sur[0][2] = true;
	}
	if(pos.x == (Room::w - 1) * TILE_SIZE) {
		sur[2][0] = sur[2][1] = sur[2][2] = true;
	}
	if(pos.y == 0) {
		sur[0][0] = sur[1][0] = sur[2][0] = true;
	}
	if(pos.y == (Room::h - 1) * TILE_SIZE) {
		sur[0][2] = sur[1][2] = sur[2][2] = true;
	}
	image_parts[0] = 0;
	image_parts[3] = 0;
	if(!sur[1][0]) {
		image_parts[1] = 6;
		image_parts[2] = 7;
		if(!sur[0][1]) {
			image_parts[0] = 1;
			image_parts[1] = 2;
		}
		if(!sur[2][1]) {
			image_parts[2] = 13;
			image_parts[3] = 14;
		}
	} else {
		image_parts[1] = 38;
		image_parts[2] = 39;
		if(!sur[0][1]) {
			image_parts[1] = 36;
		}
		if(!sur[2][1]) {
			image_parts[2] = 43;
		}
	}

	if(!sur[1][2]) {
		image_parts[4] = 7 + 7 * 16;
		image_parts[5] = 8 + 7 * 16;
		if(!sur[0][1]) {
			image_parts[4] = 6 + 7 * 16;
		}
		if(!sur[2][1]) {
			image_parts[5] = 9 + 7 * 16;
		}
	} else {
		image_parts[4] = 22;
		image_parts[5] = 23;
		if(!sur[0][1]) {
			image_parts[4] = 50;
		} else if(!sur[0][2]) {
			image_parts[4] = 20;
		}
		if(!sur[2][1]) {
			image_parts[5] = 51;
		} else if(!sur[2][2]) {
			image_parts[5] = 27;
		}
	}

	image_index = 0;
}

void Wall::roomStart() {
	if (Gamebase::on_screen(this)) {
		chooseWalls();
	}
}

void Wall::step() {
	if (image_index == -1 && visible) {
		chooseWalls();
	}
}

void Wall::draw() {
	if (visible) {
		if (image_index != -1) {
			SDL_Rect wh = { 0, 0, sprite->getWidth(), sprite->getHeight() };
			SDL_Point offx = { 16, 0 };
			SDL_Point offy = { 0, 16 };
			if (image_parts[0] != 0) {
				sprite->draw(image_parts[0], (wh | pos) - offx);
			}

			sprite->draw(image_parts[1], (wh | pos));
			sprite->draw(image_parts[2], (wh | pos) + offx);
			sprite->draw(image_parts[4], (wh | pos) + offy);
			sprite->draw(image_parts[5], (wh | pos) + offx + offy);

			if (image_parts[3] != 0) {
				sprite->draw(image_parts[3], (wh | pos) + offx + offx);
			}
		}
	}
}