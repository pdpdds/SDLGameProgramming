#pragma once

#include <vector>

class PlayState;
class Bunker;
class Bullet;
class Enemy;
class Player;

class CollisionManager
{
private:
	CollisionManager();
	static CollisionManager* s_pInstance;
	PlayState* m_ptrPlayState;
	Player* m_ptrPlayer;

public:
	//Get singleton instance of CollisionManager
	static CollisionManager* Instance();

	~CollisionManager();

	//Detect collision between bullet shot by player and enemies
	bool checkPlayerBulletEnemyCollision(Bullet* playerBullets, std::vector<Enemy*>& enemies);

	//Detect collision between bullet shot by enemy/player and bunkers
	bool checkBulletBunkerCollision(Bullet* enemyBullet, std::vector<Bunker*>& bunkers);

	//Detect collision between bullet shot by enemy and player
	bool checkEnemyBulletPlayerCollision(Bullet* enemyBullets, Player* player);

	//Called each frame
	void update();

	//Reset singleton instance to NULL
	void reset()
	{
		s_pInstance = 0;
	}
};
