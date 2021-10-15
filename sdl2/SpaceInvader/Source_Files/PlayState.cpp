#include "PlayState.h"
#include "Player.h"
#include "Enemy.h"
#include "PauseState.h"
#include "GameOverState.h"
#include "StateParser.h"
#include "BulletHandler.h"
#include "Collision.h"
#include <iostream>

void PlayState::update()
{
	if (InputController::Instance()->isKeyDown(SDL_SCANCODE_ESCAPE))
	{
		Game::Instance()->getGameStateMachine()->pushState(new PauseState());
	}

	for (size_t i = 0; i < m_gameObjects.size(); i++)
	{
		m_gameObjects[i]->update();
	}

	m_ptrController->update();
	CollisionManager::Instance()->update();
}

void PlayState::render()
{
	for (size_t i = 0; i < m_gameObjects.size(); i++)
	{
		m_gameObjects[i]->draw();
	}
}

void PlayState::onEnter()
{
	std::cout << "ON PLAY STATE ENTER" << std::endl;
	StateParser stateParser;
	stateParser.parseState("Assets/test.xml", m_strStateID, &m_gameObjects, &m_spriteIDs);

	SoundManager::Instance()->load("Assets/oh.wav", "0", SOUND_SFX);
	SoundManager::Instance()->load("Assets/uh.wav", "1", SOUND_SFX);
	SoundManager::Instance()->load("Assets/playerexplode.wav", "playerexplode", SOUND_SFX);
	SoundManager::Instance()->load("Assets/shoot.wav", "playershoot", SOUND_SFX);
	SoundManager::Instance()->load("Assets/invaderexplode.wav", "enemyexplode", SOUND_SFX);
	SoundManager::Instance()->load("Assets/damageshelter.wav", "bunkerdamage", SOUND_SFX);

	m_ptrController = new EnemyGroupController(5,11, "enemy1", 46, 33);
}

void PlayState::onExit()
{
	std::cout << "ON PLAY STATE EXIT" << std::endl;
	for (size_t i = 0; i < m_gameObjects.size(); i++)
	{
		m_gameObjects[i]->clean();
	}
	for (size_t i = 0; i < m_spriteIDs.size(); i++)
	{
		SpriteManager::instance()->clearFromMemory(m_spriteIDs[i]);
	}

	m_gameObjects.clear();
	SoundManager::Instance()->clearAudio();
	m_ptrController->getPlayerScoreHandler()->storeScore("SAVE", m_ptrController->m_iScore);
	CollisionManager::Instance()->reset();
	delete m_ptrController;
}

std::string PlayState::getStateID()
{
	return m_strStateID;
}

std::vector<GameObject*> PlayState::getActiveGameObjects()
{
	return m_gameObjects;
}

void PlayState::addGameObject(GameObject* obj)
{
	m_gameObjects.push_back(obj);
}

void PlayState::removeGameObject(GameObject* obj)
{
	std::vector<GameObject*>::iterator it = std::find(m_gameObjects.begin(), m_gameObjects.end(), obj);
	if (it != m_gameObjects.end())
		m_gameObjects.erase(it);
}

EnemyGroupController* PlayState::getEnemyController()
{
	return m_ptrController;
}

Player& PlayState::getPlayerGameObject()
{
	if (m_ptrPlayer == 0)
	{
		for (size_t i = 0; i < m_gameObjects.size(); i++)
		{
			if (dynamic_cast<Player*>(m_gameObjects[i]))
			{
				m_ptrPlayer = dynamic_cast<Player*>(m_gameObjects[i]);
				return *m_ptrPlayer;
			}
		}
	}
	return *m_ptrPlayer;
}

std::vector<Bunker*> PlayState::getBunkerBlocks()
{
	if (m_vBunkers.size() <= 0)
	{
		for (size_t i = 0; i < m_gameObjects.size(); i++)
		{
			if (dynamic_cast<Bunker*>(m_gameObjects[i]))
			{
				m_vBunkers.push_back(dynamic_cast<Bunker*>(m_gameObjects[i]));
			}
		}
		return m_vBunkers;
	}
	return m_vBunkers;
}

void PlayState::removeBunkerObj(Bunker* pBunker)
{
	std::vector<Bunker*>::iterator it = std::find(m_vBunkers.begin(), m_vBunkers.end(), pBunker);
	if (it != m_vBunkers.end())
	{
		m_vBunkers.erase(it);
	}
}
