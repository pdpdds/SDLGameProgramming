#include "player.h"

int Player::lives = 3;
int Player::score = 0;
int Player::fish = 0;

Player::Player(int x, int y, Sprite* spr) {
	sprite = spr;
	init(x,y);
	important = true;
	catchView();
	enemy_score_mult = 1;
	dead = false;
	setDepth(-3);
}

void Player::resetLives() {
	lives = 3;
	score = 0;
	fish = 0;
}

void Player::step() {
	if (!dead) {
		int horz = (Gamebase::checkKey(SDL_SCANCODE_RIGHT) - Gamebase::checkKey(SDL_SCANCODE_LEFT));
		spd.x += horz;
		if (spd.x != 0) {
			if (spd.x / abs(spd.x) != horz) {
				spd.x -= spd.x / abs(spd.x);
			}
		}
		int run = Gamebase::checkKey(SDL_SCANCODE_LCTRL) + 1;
		if (spd.x > 4 * run) {
			spd.x = 4 * run;
		} else if (spd.x < -4 * run) {
			spd.x = -4 * run;
		}
		SDL_Point off = { spd.x, 0 };
		if (Gamebase::checkCollision<Wall>(this, off)) {
			while (Gamebase::checkCollision<Wall>(this, off) && off.x != 0) {
				off.x -= off.x / abs(off.x);
			}
			pos.x += off.x;
			spd.x = 0;
		} else if (pos.x + spd.x < 0) {
			pos.x = 0;
			spd.x = 0;
		} else if (pos.x + spd.x + bbox.w > Room::w * TILE_SIZE) {
			pos.x = Room::w * TILE_SIZE - bbox.w;
			spd.x = 0;
		}
		off.x = 0;
		off.y = 1;
		if (Gamebase::checkCollision<Wall>(this, off)) {
			if (spd.y != 0) {
				spd.y = 0;
			}
			off.x = spd.x;
			off.y = -10;
			if (Gamebase::checkKeyPressed(SDL_SCANCODE_UP) && !Gamebase::checkCollision<Wall>(this, off)) {
				spd.y = -10;
				jumpTimer = 11 - (abs(spd.x) < 4) * 3;
				Gamebase::getSound(Gamebase::audio::jump)->play();
			}
		} else {
			if (!Gamebase::checkKey(SDL_SCANCODE_UP) || jumpTimer == 0) {
				spd.y++;
				if (spd.y > 10) {
					spd.y = 10;
				}
			}
			if (jumpTimer) {
				jumpTimer--;
			}
			off.x = spd.x;
			off.y = spd.y;
			if (pos.y + spd.y < 0) {
				pos.y = 0;
				spd.y = 0;
			} else {
				Wall* wall = Gamebase::instCollision<Wall>(this, off);
				if (wall != NULL) {
					if (spd.y > 0) {
						pos.y = wall->getY() - bbox.h;
						enemy_score_mult = 1;
						Gamebase::getSound(Gamebase::audio::land)->play();
					} else if (spd.y < 0) {
						pos.y = wall->getY() + wall->getH();
					}
					spd.y = 0;
					jumpTimer = 0;
				} else if (pos.y + spd.y > Room::h * TILE_SIZE) {
					die();
					return;
				}
			}
		}
		pos += spd;
		while(Gamebase::checkCollision<Wall>(this)) {
			pos -= spd / abs(spd);
		}
		if (spd.x == 0) {
			setAnim(0);
		}
		else if (spd.x > 0) {
			if (spd.x > 4) {
				setAnim(4);
			} else {
				setAnim(3);
			}
		} else {
			if (spd.x < -4) {
				setAnim(2);
			} else {
				setAnim(1);
			}
		}
		if (Gamebase::checkCollision<Danger>(this)) {
			die();
			spd.y = -8;
			return;
		}

		if (Gamebase::checkCollision<Fish>(this)) {
			Gamebase::removeObject(Gamebase::instCollision<Fish>(this));
			score += 100;
			fish++;
			if (fish == 100) {
				fish = 0;
				lives++;
				Gamebase::getSound(Gamebase::audio::life)->play();
			} else {
				Gamebase::getSound(Gamebase::audio::score)->play();
			}
		}

		if (Gamebase::checkCollision<Life>(this)) {
			Gamebase::removeObject(Gamebase::instCollision<Life>(this));
			lives++;
			Gamebase::getSound(Gamebase::audio::life)->play();
		}

		killEnemies<EnemyBasic>();
		killEnemies<EnemyFlyingBasic>();
		killEnemies<EnemyJump>();

		std::vector<Goal*> goal = Gamebase::getObjects<Goal>();
		if (goal.size() > 0) {
			if (abs(goal[0]->getX() - pos.x) < 5 && pos.y <= goal[0]->getY()) {
				Room::next();
			}
		}
		catchView();
	} else {
		image_scale.y = -1;
		spd.y = std::min(spd.y + 1, 10);
		if (abs(spd.x) > 2) {
			spd.x -= spd.x / abs(spd.x);
		}
		pos.y += spd.y;
		pos.x += spd.x;
		if (!Gamebase::on_screen(this)) {
			if (lives < 0) {
				Room::go(-1);
				Gamebase::clearInput();
			} else {
				Room::restart();
				Gamebase::clearInput();
			}
			return;
		}
	}
}

void Player::die() {
	lives--;
	dead = true;
	Gamebase::getSound(Gamebase::audio::die)->play();
}

void Player::catchView() {
	View* view = Gamebase::getView(Gamebase::currentView());
	SDL_Rect v;
	v.x = view->getView()->x;
	v.y = view->getView()->y;
	v.w = view->getView()->w;
	v.h = view->getView()->h;

	v.x = (pos.x + bbox.w / 2) - (v.w / 2);
	v.y = (pos.y + bbox.h / 2) - (v.h / 2);
	if(v.x < 0) { v.x = 0; }
	if(v.y < 0) { v.y = 0; }
	if(v.x > Room::w * TILE_SIZE - v.w) { v.x = Room::w * TILE_SIZE - v.w; }
	if(v.y > Room::h * TILE_SIZE - v.h) { v.y = Room::h * TILE_SIZE - v.h; }
	view->set_viewx(v.x);
	view->set_viewy(v.y);
}

void Player::draw() {
	Object::draw();
	SDL_Color white = {255, 255, 255, 255};
	SDL_Color black = {0, 0, 0, 255};
	Draw::setColor(white);
	Draw::text(32, 32, ("Score: " + std::to_string((long long)score)), Gamebase::getFont(0));
	Draw::text(32, 64, ("Lives: " + std::to_string((long long)std::max(0, lives))), Gamebase::getFont(0));
	Draw::text(32, 96, ("Fish: " + std::to_string((long long)fish)), Gamebase::getFont(0));
	Draw::setColor(black);
}
