#include <string>
#include "menu.h"

Menu::Menu(int x, int y, Sprite* spr) {
	sprite = spr;
	init(x, y);
	c_opt = 0;
	Player::resetLives();
	Gamebase::getView(0)->set_viewx(0);
	Gamebase::getView(0)->set_viewy(0);
}

void Menu::step() {
	if (Gamebase::checkKeyPressed(SDL_SCANCODE_UP) || Gamebase::checkKeyPressed(SDL_SCANCODE_DOWN)) {
		c_opt = !c_opt;
	}
	if (Gamebase::checkKeyPressed(SDL_SCANCODE_RETURN)) {
		if (c_opt) {
			Gamebase::end();
		} else {
			Room::go(0);
		}
		Gamebase::clearInput();
	}
}

void Menu::draw() {
	Draw::setColor(255, 255, 255, 255);
	int w = 0;
	TTF_SizeText(Gamebase::getFont(0)->getFont(), "Cat quest", &w, NULL);
	Draw::text(1024 / 2 - w / 2, 64, "Cat Quest", Gamebase::getFont(0));
	TTF_SizeText(Gamebase::getFont(0)->getFont(), "Play", &w, NULL);

	if (c_opt == 0) {
		Draw::setColor(255, 255, 255, 255);
	} else {
		Draw::setColor(200, 200, 200, 255);
	}
	Draw::text(1024 / 2 - w / 2, 128, "Play", Gamebase::getFont(0));
	TTF_SizeText(Gamebase::getFont(0)->getFont(), "Quit", &w, NULL);
	if (c_opt == 1) {
		Draw::setColor(255, 255, 255, 255);
	} else {
		Draw::setColor(200, 200, 200, 255);
	}
	Draw::text(1024 / 2 - w / 2, 160, "Quit", Gamebase::getFont(0));
}
