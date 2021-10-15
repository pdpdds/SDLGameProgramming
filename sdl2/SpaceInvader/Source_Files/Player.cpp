#include "Player.h"
#include "BulletHandler.h"
#include "GameOverState.h"
#include "SoundManager.h"
#include "PlayState.h"
#include <iostream>

#include "PlayerscoreHandler.h"

Player::Player():SDLGameObject()
{
	AnimatedGraphics* icon1 = new AnimatedGraphics();
	icon1->load(new LoaderParams(0, 460, 23, 14, "playericon", 1));
	AnimatedGraphics* icon2 = new AnimatedGraphics();
	icon2->load(new LoaderParams(25, 460, 23, 14, "playericon", 1));

	m_liveIcons.push_back(icon1);
	m_liveIcons.push_back(icon2);

	Game::Instance()->getGameStateMachine()->getCurrentState()->addGameObject(icon1);
	Game::Instance()->getGameStateMachine()->getCurrentState()->addGameObject(icon2);
	m_iNumOfLives = 2;
}

void Player::load(const LoaderParams* pParams)
{
	SDLGameObject::load(pParams);
}

Player::~Player()
{
}

void Player::draw()
{
	SDLGameObject::draw();
}

void Player::update()
{
	m_iCurrentFrame = int(((SDL_GetTicks() / 100) % m_iNumOfFrames));

	if (InputController::Instance()->isKeyDown(SDL_SCANCODE_LEFT))
	{
		if(canMove(m_vPosition.getX() - 3))
			m_vPosition.setX(m_vPosition.getX() - 3);
	}
	else if (InputController::Instance()->isKeyDown(SDL_SCANCODE_RIGHT))
	{
		if(canMove(m_vPosition.getX() + 3))
			m_vPosition.setX(m_vPosition.getX() + 3);
	}
	else if (InputController::Instance()->isKeyDown(SDL_SCANCODE_SPACE))
	{
		if (((SDL_GetTicks() - m_iTimeStamp) / 1000.0) > m_fShootGap)
		{
			SoundManager::Instance()->playSound("playershoot", false);
			BulletHandler::Instance()->addPlayerBullet(m_vPosition.getX() + (getWidth() / 2), m_vPosition.getY(), 5, 5, "bullet", 2, Vector2D(0, -7));
			m_iTimeStamp = SDL_GetTicks();
		}
	}

	SDLGameObject::update();
}

bool Player::canMove(int newXPos)
{
	if ((newXPos < 1) || (newXPos > (640 - getWidth())))
		return false;

	return true;
}

void Player::clean()
{

}

void Player::onCollision()
{
	SoundManager::Instance()->playSound("playerexplode", false);
	ModifyNumOfLives(-1);
	if (m_iNumOfLives < 0)
	{
		Game::Instance()->getGameStateMachine()->changeState(new GameOverState());
	}
	else
	{
		std::cout << "PLAYER DESTROYED! NOW PLAYER HAS " << m_iNumOfFrames << std::endl;
		m_liveIcons[m_iNumOfLives]->hide();
	}
}
