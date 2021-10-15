#include "Bullet.h"
#include <iostream>
#include "Game.h"
#include "BulletHandler.h"

Bullet::Bullet() :SDLGameObject()
{

}

void Bullet::load(const LoaderParams* pParams)
{
	SDLGameObject::load(pParams);
	m_iCurrentFrame = 0;
}

Bullet::~Bullet()
{
}

void Bullet::draw()
{
	SDLGameObject::draw();
}

void Bullet::update()
{
	SDLGameObject::update();

	if (m_vPosition.getY() < 0)
	{
		Game::Instance()->getGameStateMachine()->getCurrentState()->removeGameObject(this);
		BulletHandler::Instance()->removePlayerBullet(this);
		delete this;
	}

	if (m_vPosition.getY() > 480)
	{
		Game::Instance()->getGameStateMachine()->getCurrentState()->removeGameObject(this);
		BulletHandler::Instance()->removeEnemyBullet(this);
		delete this;
	}
}

void Bullet::clean()
{
	SDLGameObject::clean();
}


void Bullet::setVelocity(Vector2D newVelocity)
{
	m_vVelocity.setX(newVelocity.getX());
	m_vVelocity.setY(newVelocity.getY());
}

void Bullet::setPosition(Vector2D newPosition)
{
	m_vPosition.setX(newPosition.getX());
	m_vPosition.setY(newPosition.getY());
}

void Bullet::onCollision()
{
	if (b_isPlayerBullet)
		BulletHandler::Instance()->removePlayerBullet(this);
	else
		BulletHandler::Instance()->removeEnemyBullet(this);

	Game::Instance()->getGameStateMachine()->getCurrentState()->removeGameObject(this);
	//delete this;
}
