#include "PauseState.h"
#include "MenuState.h"
#include "StateParser.h"

void PauseState::s_pauseToMain()
{
	//Game::Instance()->getGameStateMachine()->changeState(new MenuState());
}

void PauseState::s_resumeGame()
{
	Game::Instance()->getGameStateMachine()->popState();
}

void PauseState::update()
{
	for (size_t i = 0; i < m_gameObjects.size(); i++)
	{
		m_gameObjects[i]->update();
	}
}

void PauseState::render()
{
	for (size_t i = 0; i < m_gameObjects.size(); i++)
	{
		m_gameObjects[i]->draw();
	}
}

void PauseState::onEnter()
{
	std::cout << "ON PAUSE STATE ENTER" << std::endl;
	StateParser stateParser;
	stateParser.parseState("Assets/test.xml", m_strStateID, &m_gameObjects, &m_spriteIDs);
	m_callbacks.push_back(0);
	m_callbacks.push_back(s_pauseToMain);
	m_callbacks.push_back(s_resumeGame);

	setCallbacks(m_callbacks);
}

void PauseState::onExit()
{
	std::cout << "ON PAUSE STATE ENTER" << std::endl;
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

void PauseState::setCallbacks(const std::vector<Callback>& callbacks)
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

std::vector<GameObject*> PauseState::getActiveGameObjects()
{
	return m_gameObjects;
}

std::string PauseState::getStateID()
{
	return m_strStateID;
}

void PauseState::addGameObject(GameObject* obj)
{
	m_gameObjects.push_back(obj);
}

void PauseState::removeGameObject(GameObject* obj)
{
	std::vector<GameObject*>::iterator it = std::find(m_gameObjects.begin(), m_gameObjects.end(), obj);
	if (it != m_gameObjects.end())
		m_gameObjects.erase(it);
}
