#include <memory>
#include "Enemy.h"
#include "PlayState.h"
#include "GameOverState.h"
#include "BulletHandler.h"
#include "EnemyGroupController.h"

Enemy::Enemy():SDLGameObject()
{

}

Enemy::~Enemy()
{

}

void Enemy::clean()
{
	SDLGameObject::clean();
}

void Enemy::draw()
{
	SDLGameObject::draw();
}

void Enemy::update()
{
	if ((float)((SDL_GetTicks() - m_iTimeStamp) / 1000.0) > m_iTimeGap)
	{
		m_vPosition.setX(m_vPosition.getX() + m_vMoveBy.getX());
		m_vPosition.setY(m_vPosition.getY() + m_vMoveBy.getY());

		m_iCurrentFrame = (m_iCurrentFrame == 0) ? 1 : 0;

		m_iTimeStamp = SDL_GetTicks();
		SoundManager::Instance()->playSound(std::to_string(m_iCurrentFrame), false);
	}

	SDLGameObject::update();
}

void Enemy::load(const LoaderParams* pParams)
{
	SDLGameObject::load(pParams);
}

int Enemy::getRow()
{
	return m_iRow;
}

int Enemy::getColumn()
{
	return m_iColumn;
}

void Enemy::setColumn(int column)
{
	m_iColumn = column;
}

void Enemy::setRow(int row)
{
	m_iRow = row;
}

void Enemy::setMoveBy(Vector2D vel)
{
	m_vMoveBy = vel;
}

void Enemy::setTimeGap(int t)
{
	m_iTimeGap = t;
}

void Enemy::moveDown(Vector2D vel)
{
	m_vPosition.setY(m_vPosition.getY() + 10);
	m_vMoveBy.setX(0);
	m_vMoveBy = vel;
}

void Enemy::shoot()
{
	BulletHandler::Instance()->addEnemyBullet(m_vPosition.getX() + (getWidth() / 2), m_vPosition.getY() - (getHeight() / 2), 5, 5, "enemybullet", 2, Vector2D(0, 7));
}

void Enemy::onCollision()
{
	SoundManager::Instance()->playSound("enemyexplode", false);
	Game::Instance()->getGameStateMachine()->getCurrentState()->removeGameObject(this);
	if (dynamic_cast<PlayState*>(Game::Instance()->getGameStateMachine()->getCurrentState()))
	{
		PlayState* pState = dynamic_cast<PlayState*>(Game::Instance()->getGameStateMachine()->getCurrentState());
		EnemyGroupController* controller = pState->getEnemyController();
		controller->removeEnemy(this);
		delete this;
	}
}
