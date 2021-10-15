#include <string>
#include "score.h"

Score::Score(int x, int y, Sprite* spr) {
	sprite = spr;
	init(x, y);
	timer = 45;
	val = 0;
	setDepth(-4);
}

void Score::setScore(int n) {
	val = n;
}

void Score::step() {
	if (!(timer%2)) {
		pos.y--;
	}
	timer--;
	if (!timer) {
		Gamebase::removeObject(this);
		return;
	}
}

void Score::draw() {
	Draw::setColor(255, 255, 255, 255);
	std::string str = std::to_string((long long)val);
	int w = 0;
	TTF_SizeText(Gamebase::getFont(0)->getFont(), (char*)str.c_str(), &w, NULL);
	Draw::text(pos.x - w / 2 - Gamebase::getView(0)->getView()->x, pos.y - Gamebase::getView(0)->getView()->y, (char*)str.c_str(), Gamebase::getFont(0));
}
