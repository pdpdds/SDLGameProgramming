
/* Player.cpp
 *
 * Copyright (C) 2013 Michael Imamura
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

#include "StdAfx.h"

#include "Player.h"

namespace AISDL {

Player::Player() :
	name("???"),
	posX(0), posY(0), direction(Direction::RIGHT)
{
}

/**
 * Clean up any scene-specific state to get ready for another scene.
 */
void Player::Reset()
{
	Silence();
}

void Player::SetName(const std::string &name)
{
	this->name = name;
}

/**
 * Update the facing direction and total distance traveled in this direction.
 * @param direction The new direction.
 * @param distance The added distance traveled in this direction.
 */
void Player::AdjustDirection(Direction::type direction, float distance)
{
	if (this->direction == direction) {
		// Moving in the same direction.
		animDistance += fabs(distance);
	}
	else {
		// Switched directions.
		this->direction = direction;
		animDistance = fabs(distance);
	}
}

void Player::SetPos(float x, float y)
{
	this->posX = x;
	this->posY = y;
}

/**
 * Move the player by the specified units.
 * The direction will be updated automatically.
 * @param dx The units to move horizontally.
 * @param dy The units to move vertically.
 */
void Player::Move(float dx, float dy)
{
	if (dx == 0 && dy == 0) {
		// Stopped moving.
		animDistance = 0;
		return;
	}

	this->posX += dx;
	this->posY += dy;

	if (dx == 0) {
		if (dy < 0) {
			AdjustDirection(Direction::UP, dy);
		}
		else if (dy > 0) {
			AdjustDirection(Direction::DOWN, dy);
		}
	}
	else if (dx < 0) {
		AdjustDirection(Direction::LEFT, dx);
	}
	else {
		AdjustDirection(Direction::RIGHT, dx);
	}
}

/**
 * Retrieve the direction the player is facing.
 * @param direction The direction.
 */
void Player::SetDirection(Direction::type direction)
{
	this->direction = direction;
}

/**
 * Set the text that appears above the player.
 * @param s The text.
 */
void Player::SetBalloonText(const std::string &s)
{
	balloonText = s;
	balloonTs = SDL_GetTicks();
}

/**
 * Stop the player saying anything.
 */
void Player::Silence()
{
	balloonText.clear();
}

/**
 * Make the player say something.
 * @param s The text to say (empty string will silence the player).
 */
void Player::Say(const std::string &s)
{
	if (s.empty()) {
		Silence();
	}
	else {
		SetBalloonText(s);

		// Face the viewer.
		direction = Direction::DOWN;
	}
}

}  // namespace AISDL

