#include "CollisionManager.h"
#include "BulletHandler.h"
#include "Bunker.h"
#include "Enemy.h"
#include "Player.h"
#include "Collision.h"
#include "PlayState.h"

CollisionManager* CollisionManager::s_pInstance = 0;

CollisionManager::CollisionManager()
{
	GameState* state = Game::Instance()->getGameStateMachine()->getCurrentState();
	if(m_ptrPlayState = dynamic_cast<PlayState*>(state))
		m_ptrPlayState = dynamic_cast<PlayState*>(state);

	m_ptrPlayer = &(m_ptrPlayState->getPlayerGameObject());
}

CollisionManager::~CollisionManager()
{

}

CollisionManager* CollisionManager::Instance()
{
	if (s_pInstance == 0)
	{
		s_pInstance = new CollisionManager();
		return s_pInstance;
	}
	return s_pInstance;
}

void CollisionManager::update()
{
	std::vector<Bullet*> playerBullets = BulletHandler::Instance()->getPlayerBullets();
	std::vector<Bunker*> bunkers = m_ptrPlayState->getBunkerBlocks();
	std::vector<Bullet*> enemyBullets = BulletHandler::Instance()->getEnemyBullets();

	for (size_t j = 0; j < enemyBullets.size(); j++)
	{
		checkBulletBunkerCollision(enemyBullets.at(j), bunkers);
		checkEnemyBulletPlayerCollision(enemyBullets.at(j), m_ptrPlayer);
	}

	for (size_t i = 0; i < playerBullets.size(); i++)
	{
		checkBulletBunkerCollision(playerBullets.at(i), bunkers);
		checkPlayerBulletEnemyCollision(playerBullets.at(i), m_ptrPlayState->getEnemyController()->getEnemies());
	}
}

bool CollisionManager::checkPlayerBulletEnemyCollision(Bullet* playerBullet, std::vector<Enemy*>& enemies)
{
	SDL_Rect* rect1 = new SDL_Rect();
	rect1->x = playerBullet->getPosition().getX();
	rect1->y = playerBullet->getPosition().getY();
	rect1->w = playerBullet->getWidth();
	rect1->h = playerBullet->getHeight();
	for (size_t j = 0; j < enemies.size(); j++)
	{
		SDL_Rect* rect2 = new SDL_Rect();
		rect2->x = enemies[j]->getPosition().getX();
		rect2->y = enemies[j]->getPosition().getY();
		rect2->w = enemies[j]->getWidth();
		rect2->h = enemies[j]->getHeight();

		if (RectRect(rect1, rect2))
		{
			std::cout << "PLAYER DESTROYED ENEMY AT POSITION " << enemies[j]->getRow() << " , " << enemies[j]->getColumn() << std::endl;
			playerBullet->onCollision();
			enemies[j]->onCollision();
			return true;
		}
	}
	return false;
}

bool CollisionManager::checkBulletBunkerCollision(Bullet* bullet, std::vector<Bunker*>& bunkers)
{
	SDL_Rect* rect1 = new SDL_Rect();
	rect1->x = bullet->getPosition().getX();
	rect1->y = bullet->getPosition().getY();
	rect1->w = bullet->getWidth();
	rect1->h = bullet->getHeight();
	for (size_t j = 0; j < bunkers.size(); j++)
	{
		SDL_Rect* rect2 = new SDL_Rect();
		rect2->x = bunkers.at(j)->getPosition().getX();
		rect2->y = bunkers.at(j)->getPosition().getY();
		rect2->w = bunkers.at(j)->getWidth();
		rect2->h = bunkers.at(j)->getHeight();

		if (RectRect(rect1, rect2))
		{
			bunkers.at(j)->onCollision();
			bullet->onCollision();
			return true;
		}
	}
	return false;
}

bool CollisionManager::checkEnemyBulletPlayerCollision(Bullet* enemyBullet, Player* player)
{
	SDL_Rect* rect1 = new SDL_Rect();
	rect1->x = enemyBullet->getPosition().getX();
	rect1->y = enemyBullet->getPosition().getY();
	rect1->w = enemyBullet->getWidth();
	rect1->h = enemyBullet->getHeight();

	SDL_Rect* rect2 = new SDL_Rect();
	rect2->x = player->getPosition().getX();
	rect2->y = player->getPosition().getY();
	rect2->w = player->getWidth();
	rect2->h = player->getHeight();

	if (RectRect(rect1, rect2))
	{
		enemyBullet->onCollision();
		player->onCollision();
		return true;
	}

	return false;
}
