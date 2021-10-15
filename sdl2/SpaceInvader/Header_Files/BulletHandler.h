#pragma once

#include <vector>
#include "Bullet.h"


class BulletHandler
{
private:
	static BulletHandler* s_pInstance;
	BulletHandler();
	std::vector<Bullet*> m_playerBullets;
	std::vector<Bullet*> m_enemyBullets;

public:
	~BulletHandler();
	//Get singleton instance
	static BulletHandler* Instance();

	//Add bullet shot by player in current state's active gameobject list
	void addPlayerBullet(int x, int y, int width, int height, std::string spriteID, int numFrames, Vector2D velocity);

	//Add bullet shot by enemy in current state's active gameobject list
	void addEnemyBullet(int x, int y, int width, int height, std::string spriteID, int numFrames, Vector2D velocity);

	//Remove bullet shot by player from current state's active gameobject list
	void removePlayerBullet(Bullet* bullet);

	//Remove bullet shot by enemy from current state's active gameobject list
	void removeEnemyBullet(Bullet* bullet);

	//Get list of palyer shot bullets currently active in the scene
	std::vector<Bullet*> getPlayerBullets();

	//Get list of enemy shot bullets currently active in the scene
	std::vector<Bullet*> getEnemyBullets();
};
