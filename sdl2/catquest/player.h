#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "object.h"
#include "gamebase.h"
#include "wall.h"
#include "goal.h"
#include "spike.h"
#include "fish.h"
#include "life.h"
#include "score.h"
#include "enemy_basic.h"
#include "enemy_flying_basic.h"
#include "draw.h"
#include "sdlx.h"
#include <vector>
#include <string>
#include <stdio.h>

class Player : public Object {
private:
	int jumpTimer, enemy_score_mult;
	static int score, lives, fish;
	bool dead;
public:
	Player(int, int, Sprite* = Gamebase::getSprite(Gamebase::sprite::player));

	static void __cdecl resetLives();

	void step();

	void die();
	
	void catchView();

	void draw();

	template<class T>
	void killEnemies();
};

template<class T>
void Player::killEnemies() {
	T* enemy;
	while ((enemy = Gamebase::instCollision<T>(this)) != NULL) {
		spd.y = -8 - (4 * Gamebase::checkKey(SDL_SCANCODE_UP));
		if (enemy_score_mult < 8) {
			Gamebase::addObject<Score>(pos.x + bbox.w / 2, pos.y)->setScore(50 * enemy_score_mult);
			score += 50 * enemy_score_mult++;
		} else {
			lives++;
			Gamebase::getSound(Gamebase::audio::life)->play();
		}
		enemy->die();
		Gamebase::getSound(Gamebase::audio::jump)->play();
	}
}

#include "room.h"

#endif