#include "EnemyGroupController.h"
#include "GameOverState.h"

EnemyGroupController::EnemyGroupController(int row, int column, std::string type, int width, int height)
{
	m_ptrScoreHandler = new PlayerScoreHandler(475, 460);
	m_ptrScoreHandler->updateScore(std::to_string(m_iScore));
	m_iShootUpperLimit = 5;
	m_iTimeGap = 1;
	m_shootTime = rand() % m_iShootUpperLimit;
	spawnWave(row, column, "enemy1", 46, 33);
}

EnemyGroupController::~EnemyGroupController()
{
	m_enemies.clear();
}

void EnemyGroupController::updateDifficultyStats()
{
	m_iTimeGap += (m_iTimeGap * 5) / 100;
	m_iShootUpperLimit -= 0.5;
	if (m_iShootUpperLimit < 2)
		m_iShootUpperLimit = 2;
}

PlayerScoreHandler* EnemyGroupController::getPlayerScoreHandler()
{
	return m_ptrScoreHandler;
}

void EnemyGroupController::MovementDirectionCheck()
{
	if (!b_first)
	{
		for (int i = 0; i < m_iColumn; ++i)
		{
			std::vector<Enemy*>::iterator it = std::find_if(m_enemies.begin(), m_enemies.end(), [i](Enemy* e) { return (e->getColumn() == i); });
			if (it != m_enemies.end())
			{
				if ((*it)->getPosition().getX() <= 10)
				{
					for (size_t i = 0; i < m_enemies.size(); i++)
					{
						m_enemies[i]->moveDown(Vector2D(m_iSpeed, 0));
						if (m_enemies[i]->getPosition().getY() > 340)
						{
							Game::Instance()->getGameStateMachine()->changeState(new GameOverState());
							return;
						}
					}

					b_first = !b_first;
					return;
				}
			}
		}
	}

	if (b_first)
	{
		for (int j = m_iColumn - 1; j >= 0; --j)
		{
			std::vector<Enemy*>::iterator it = std::find_if(m_enemies.begin(), m_enemies.end(), [j](Enemy* e) { return (e->getColumn() == j); });
			if (it != m_enemies.end())
			{
				if ((*it)->getPosition().getX() > 590)
				{
					for (size_t i = 0; i < m_enemies.size(); i++)
					{
						m_enemies[i]->moveDown(Vector2D(-m_iSpeed, 0));

						if (m_enemies[i]->getPosition().getY() > 340)
						{
							Game::Instance()->getGameStateMachine()->changeState(new GameOverState());
							return;
						}
					}

					b_first = !b_first;
					return;
				}
			}
		}
	}
}

void EnemyGroupController::spawnWave(int row, int column, std::string type, int width, int height)
{
	std::cout << "HORDE OF " << row*column << " ENEMIES SPAWNED" << std::endl;
	b_first = true;
	updateDifficultyStats();
	m_shootTime = rand() % m_iShootUpperLimit;
	m_iColumn = column;
	m_iRow = row;
	if (type == "enemy1")
	{
		for (size_t i = 0; i < column; i++)
		{
			for (size_t j = 0; j < row; j++)
			{
				Enemy* enemy = new Enemy();
				enemy->load(new LoaderParams(10 + (i*width) + (i * 5), 10 + (j*height) + (j * 5), 46, 33, type, 2));
				enemy->setColumn(i);
				enemy->setRow(j);
				Game::Instance()->getGameStateMachine()->getCurrentState()->addGameObject(enemy);
				enemy->setMoveBy(Vector2D(m_iSpeed, 0));
				enemy->setTimeGap(m_iTimeGap);
				m_enemies.push_back(enemy);
			}
		}
	}
}

void EnemyGroupController::update()
{
	if (((SDL_GetTicks() - m_iTimeStamp) / 1000.0) > m_iTimeGap)
	{
		MovementDirectionCheck();
		m_iTimeStamp = SDL_GetTicks();
	}

	if (((SDL_GetTicks() - m_shootTimeStamp) / 1000.0) > m_shootTime)
	{
		Enemy* enemy = m_enemies[rand() % m_enemies.size()];
		std::cout << "ENEMY AT ROW : " << enemy->getRow() << " COLUMN : " << enemy->getColumn() << " SHOOTS" << std::endl;
		enemy->shoot();
		m_shootTimeStamp = SDL_GetTicks();
		m_shootTime = rand() % m_iShootUpperLimit;
	}
}

void EnemyGroupController::removeEnemy(Enemy* enemy)
{
	m_iScore += 10;
	m_ptrScoreHandler->updateScore(std::to_string(m_iScore));
	m_enemies.erase(std::remove(m_enemies.begin(), m_enemies.end(), enemy), m_enemies.end());
	if (m_enemies.size() == 0)
	{
		spawnWave(5, 11, "enemy1", 46, 33);
	}
}

void EnemyGroupController::resetEnemyShootTimer()
{
	m_shootTime = rand() % m_iShootUpperLimit;
	m_shootTimeStamp = SDL_GetTicks();
}

std::vector<Enemy*> EnemyGroupController::getEnemies()
{
	return m_enemies;
}
