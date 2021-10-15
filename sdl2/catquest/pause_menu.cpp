#include <string>
#include "pause_menu.h"

PauseMenu::PauseMenu(int x, int y, Sprite* spr) {
	sprite = spr;
	init(x, y);
	c_opt = 0;
}

void PauseMenu::step() {
	if (Gamebase::checkKeyPressed(SDL_SCANCODE_UP)) {
		c_opt = (c_opt + 3 - 1) % 3;
	} else if (Gamebase::checkKeyPressed(SDL_SCANCODE_DOWN)) {
		c_opt = (c_opt + 1) % 3;
	}
	if (Gamebase::checkKeyPressed(SDL_SCANCODE_RETURN)) {
		if (c_opt == 2) {
			Gamebase::end();
		} else if(c_opt == 1) {
			Room::go(-1);
		} else {
			Gamebase::pause();
		}
		Gamebase::clearInput();
	} else if (Gamebase::checkKeyPressed(SDL_SCANCODE_P) || Gamebase::checkKeyPressed(SDL_SCANCODE_ESCAPE)) {
		Gamebase::pause();
	}
}

void PauseMenu::draw() {
	Draw::setColor(255, 255, 255, 255);
	int w = 0;
	TTF_SizeText(Gamebase::getFont(0)->getFont(), "Paused", &w, NULL);
	Draw::text(1024 / 2 - w / 2, 64, "Paused", Gamebase::getFont(0));

	if (c_opt == 0) {
		Draw::setColor(255, 255, 255, 255);
	}
	else {
		Draw::setColor(200, 200, 200, 255);
	}
	TTF_SizeText(Gamebase::getFont(0)->getFont(), "Resume", &w, NULL);
	Draw::text(1024 / 2 - w / 2, 128, "Resume", Gamebase::getFont(0));

	if (c_opt == 1) {
		Draw::setColor(255, 255, 255, 255);
	}
	else {
		Draw::setColor(200, 200, 200, 255);
	}
	TTF_SizeText(Gamebase::getFont(0)->getFont(), "Menu", &w, NULL);
	Draw::text(1024 / 2 - w / 2, 160, "Menu", Gamebase::getFont(0));

	if (c_opt == 2) {
		Draw::setColor(255, 255, 255, 255);
	}
	else {
		Draw::setColor(200, 200, 200, 255);
	}
	TTF_SizeText(Gamebase::getFont(0)->getFont(), "Quit", &w, NULL);
	Draw::text(1024 / 2 - w / 2, 192, "Quit", Gamebase::getFont(0));
}
