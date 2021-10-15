#include "GameOverState.h"
#include "MenuState.h"
#include "PlayState.h"
#include "AnimatedGraphics.h"
#include "MainMenuState.h"
#include "StateParser.h"
#include "MenuButton.h"

void GameOverState::s_gameOverToMain()
{
	Game::Instance()->getGameStateMachine()->changeState(new MainMenuState());
}

void GameOverState::s_restartPlay()
{
	Game::Instance()->getGameStateMachine()->changeState(new PlayState());
}

void GameOverState::update()
{
	for (size_t i = 0; i < m_gameObjects.size(); i++)
	{
		if(m_gameObjects.size() < 300)
			m_gameObjects[i]->update();
		else
		{
			m_gameObjects.clear();
		}
	}
}

void GameOverState::render()
{
	for (size_t i = 0; i < m_gameObjects.size(); i++)
	{
		m_gameObjects[i]->draw();
	}
}

void GameOverState::onEnter()
{
	std::cout << "ON GAME OVER ENTER" << std::endl;
	StateParser stateParser;
	stateParser.parseState("Assets/test.xml", m_strStateID, &m_gameObjects, &m_spriteIDs);
	m_callbacks.push_back(0);
	m_callbacks.push_back(s_gameOverToMain);
	m_callbacks.push_back(s_restartPlay);
	setCallbacks(m_callbacks);
} 

void GameOverState::onExit()
{
	std::cout << "ON GAME OVER STATE EXIT" << std::endl;
	for (size_t i = 0; i < m_gameObjects.size(); i++)
	{
		m_gameObjects[i]->clean();
	}

	for (size_t j = 0; j < m_spriteIDs.size(); j++)
	{
		SpriteManager::instance()->clearFromMemory(m_spriteIDs[j]);
	}

	m_gameObjects.clear();
}

void GameOverState::setCallbacks(const std::vector<Callback>& callbacks)
{
	for (size_t i = 0; i < m_gameObjects.size(); i++)
	{
		if (dynamic_cast<MenuButton*>(m_gameObjects[i]))
		{
			MenuButton* pButton = dynamic_cast<MenuButton*>(m_gameObjects[i]);
			pButton->setCallback(callbacks[pButton->getCallbackID()]);
		}
	}
}

std::string GameOverState::getStateID()
{
	return m_strStateID;
}

std::vector<GameObject*> GameOverState::getActiveGameObjects()
{
	return m_gameObjects;
}

void GameOverState::addGameObject(GameObject* obj)
{
	m_gameObjects.push_back(obj);
}

void GameOverState::removeGameObject(GameObject* obj)
{
	std::vector<GameObject*>::iterator it = std::find(m_gameObjects.begin(), m_gameObjects.end(), obj);
	if (it != m_gameObjects.end())
		m_gameObjects.erase(it);
}
