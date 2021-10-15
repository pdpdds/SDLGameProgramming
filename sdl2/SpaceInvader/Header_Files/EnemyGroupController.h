#pragma once

#include <string>
#include <algorithm>
#include "Enemy.h"
#include "PlayerscoreHandler.h"

class EnemyGroupController
{
private:
	PlayerScoreHandler* m_ptrScoreHandler;
	std::vector<Enemy*> m_enemies;
	int m_iRow, m_iColumn;
	int m_iSpeed = 8;
	int m_shootTime;
	Uint32 m_shootTimeStamp = 0;
	float m_iTimeGap;
	Uint32 m_iTimeStamp = 0;
	int m_iShootUpperLimit;
	bool b_first;

public:
	~EnemyGroupController();

	EnemyGroupController(int row, int column, std::string type, int width, int height);

	//Current game's score
	int m_iScore = 0;

	//Get list of current active enemies
	std::vector<Enemy*> getEnemies();

	//Reset random shoot time to default value
	void resetEnemyShootTimer();

	//Remove enemy from currently active enemies list
	void removeEnemy(Enemy* enemy);

	//Called each frame
	void update();

	//Check and move enemy horde in correct direction
	void MovementDirectionCheck();

	//Increase difficulty after each horde is destroyed
	void updateDifficultyStats();

	//Get reference to PlayerScorehandler class
	PlayerScoreHandler* getPlayerScoreHandler();

	//Spawn enemy wave with row*column. Width,column of sprite must be specified
	void spawnWave(int row, int column, std::string type, int width, int height);
};
