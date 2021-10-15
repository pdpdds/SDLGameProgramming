#include "BulletHandler.h"
#include "Game.h"
#include "SpriteManager.h"

BulletHandler* BulletHandler::s_pInstance = 0;

BulletHandler::BulletHandler()
{
	SpriteManager::instance()->load("Assets/bullet.png", "bullet", Game::Instance()->getRenderer());
}

BulletHandler::~BulletHandler()
{

}

BulletHandler* BulletHandler::Instance()
{
	if (s_pInstance == 0)
	{
		s_pInstance = new BulletHandler();
		
		return s_pInstance;
	}
	return s_pInstance;
}

void BulletHandler::addPlayerBullet(int x, int y, int width, int height, std::string spriteID, int numFrames, Vector2D velocity)
{
	Bullet* playerBullet = new Bullet();
	playerBullet->load(new LoaderParams(x, y, width, height, spriteID, numFrames));
	playerBullet->setPosition(Vector2D(x, y));
	playerBullet->setVelocity(velocity);
	playerBullet->setPlayerBullet(true);
	Game::Instance()->getGameStateMachine()->getCurrentState()->addGameObject(playerBullet);
	m_playerBullets.push_back(playerBullet);
}

void BulletHandler::addEnemyBullet(int x, int y, int width, int height, std::string spriteID, int numFrames, Vector2D velocity)
{
	Bullet* enemyBullet = new Bullet();
	enemyBullet->load(new LoaderParams(x, y, width, height, spriteID, numFrames));
	enemyBullet->setPosition(Vector2D(x, y));
	enemyBullet->setVelocity(velocity);
	enemyBullet->setPlayerBullet(false);
	Game::Instance()->getGameStateMachine()->getCurrentState()->addGameObject(enemyBullet);
	m_enemyBullets.push_back(enemyBullet);
}

void BulletHandler::removePlayerBullet(Bullet* bullet)
{
	std::vector<Bullet*>::iterator it = std::find(m_playerBullets.begin(), m_playerBullets.end(), bullet);
	if (it != m_playerBullets.end())
		m_playerBullets.erase(it);
}

void BulletHandler::removeEnemyBullet(Bullet* bullet)
{
	std::vector<Bullet*>::iterator it = std::find(m_enemyBullets.begin(), m_enemyBullets.end(), bullet);
	if (it != m_enemyBullets.end())
		m_enemyBullets.erase(it);
}

std::vector<Bullet*> BulletHandler::getPlayerBullets()
{
	return m_playerBullets;
}

std::vector<Bullet*> BulletHandler::getEnemyBullets()
{
	return m_enemyBullets;
}
