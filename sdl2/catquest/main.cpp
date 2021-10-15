#include "game.h"
#include <string>
#undef main
int main(int argc, char* args[]) {
	Gamebase game(1024, 640);

	if(game.isOK()) {
		Gamebase::addFont("font/pixel_font.ttf", 14);
		Gamebase::addView(0, 0, 1024, 768);
		Gamebase::useView(0);
		Gamebase::addSprite("sprite/player.png")
			->split(96, 32, 32)
			->addAnim("54,55,56,55", 5)
			->addAnim("12,13,14,13", 5)
			->addAnim("18,19,20,19", 5)
			->addAnim("24,25,26,25", 5)
			->addAnim("30,31,32,31", 5);
		Gamebase::addSprite("sprite/wall.png")->split(256, 16, 16);
		Gamebase::addSprite("sprite/goal.png")->split(11, 32, 32, 13, 0, 10, 0)->addAnim(0, 11, 5);
		Gamebase::addSprite("sprite/spike.png", 0, -24);
		Gamebase::addSprite("sprite/point.png");
		Gamebase::addSprite("sprite/enemy_basic.png")
			->split(96, 32, 32)
			->addAnim("12,13,14,13", 5)
			->addAnim("24,25,26,25", 5);
		Gamebase::addSprite("sprite/enemy_flying_basic.png")
			->split(12, 32, 32, 96, 192, 0, 16, 3)
			->addAnim("3,4,5,4", 5)
			->addAnim("6,7,8,7", 5);
		Gamebase::addSprite("sprite/fish.png");
		Gamebase::addSprite("sprite/pass_wall.png");
		Gamebase::addSprite("sprite/many_fish.png")->split(96, 32, 32);
		Gamebase::addSprite("sprite/animals.png")->split(3, 32, 32, 192, 127, 0, 0, 3);

		Gamebase::addMusic("audio/carefree.wav");
		Gamebase::addSound("audio/jump.wav");
		Gamebase::addSound("audio/die.wav");
		Gamebase::addSound("audio/land.wav");
		Gamebase::addSound("audio/life.wav");
		Gamebase::addSound("audio/fish.wav");
		Gamebase::getMusic(0)->play();

		Room::go(-1);

		while(game.isOK()) {
			game.events();

			game.runObjects();

			Draw::clear(0x55, 0xB4, 0xFF);
			game.draw();
			game.refresh();
		}
	}

	return 0;
}
