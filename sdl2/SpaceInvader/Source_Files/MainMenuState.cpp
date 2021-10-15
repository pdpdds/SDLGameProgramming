#include <algorithm>
#include "MainMenuState.h"
#include "StateParser.h"
#include "MenuButton.h"
#include "PlayState.h"
#include "TextDisplay.h"

void MainMenuState::update()
{
	for (size_t i = 0; i < m_gameObjects.size(); i++)
	{
		if(m_gameObjects.size() < 100)
			m_gameObjects[i]->update();
		else
		{
			m_gameObjects.clear();
		}
	}
}

void MainMenuState::render()
{
	SpriteManager::instance()->draw("highscore", 150, 400, 300, 50, Game::Instance()->getRenderer());
	for (size_t i = 0; i < m_gameObjects.size(); i++)
	{
		m_gameObjects[i]->draw();
	}
}

void MainMenuState::onEnter()
{
	std::cout << "ON MAIN MENU STATE ENTER" << std::endl;
	StateParser stateParser;
	stateParser.parseState("Assets/test.xml", m_strMenuID, &m_gameObjects, &m_spriteIDs);
	m_callbacks.push_back(0);
	m_callbacks.push_back(s_menuToPlay);
	m_callbacks.push_back(s_exitFromMenu);
	setCallbacks(m_callbacks);
	displayHighScore("SAVE");
}

void MainMenuState::onExit()
{
	std::cout << "ON MAIN MENU STATE EXIT" << std::endl;
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

void MainMenuState::setCallbacks(const std::vector<Callback>& callbacks)
{
	for (size_t i = 0; i < m_gameObjects.size(); i++)
	{
		if (dynamic_cast<MenuButton*>(m_gameObjects[i]))
		{
			MenuButton* pButton = dynamic_cast<MenuButton*>(m_gameObjects[i]);
			pButton->setCallback(callbacks[pButton->getCallbackID()]);
		}
	}

	for (size_t i = 0; i < m_gameObjects.size(); i++)
	{
		if (dynamic_cast<MenuButton*>(m_gameObjects[i]))
		{
			MenuButton* pButton = dynamic_cast<MenuButton*>(m_gameObjects[i]);
		}
	}
}

std::string MainMenuState::getStateID()
{
	return m_strMenuID;
}

std::vector<GameObject*> MainMenuState::getActiveGameObjects()
{
	return m_gameObjects;
}

void MainMenuState::s_exitFromMenu()
{
	Game::Instance()->quit();
}

void MainMenuState::s_menuToPlay()
{
	Game::Instance()->getGameStateMachine()->changeState(new PlayState());
}

void MainMenuState::addGameObject(GameObject* obj)
{
	m_gameObjects.push_back(obj);
}

void MainMenuState::removeGameObject(GameObject* obj)
{
	std::vector<GameObject*>::iterator it = std::find(m_gameObjects.begin(), m_gameObjects.end(), obj);
	if (it != m_gameObjects.end())
		m_gameObjects.erase(it);
}

void MainMenuState::displayHighScore(std::string saveStateID)
{
	TiXmlDocument xmlDoc;
	if (!xmlDoc.LoadFile("Assets/test.xml"))
	{
		std::cerr << xmlDoc.ErrorDesc() << std::endl;
		return;
	}

	TiXmlElement* pRoot = xmlDoc.RootElement();

	TiXmlElement* pStateRoot = 0;
	for (TiXmlElement* e = pRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement())
	{
		if (e->Value() == saveStateID)
		{
			pStateRoot = e;
		}
	}

	TiXmlElement* pObjectRoot = 0;
	for (TiXmlElement* e = pStateRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement())
	{
		if (e->Value() == std::string("OBJECTS"))
		{
			pObjectRoot = e;
			TiXmlElement* ele = e->FirstChildElement("object")->ToElement();
			std::string prevScore = ele->Attribute("score");
			prevScore = "HIGH SCORE " + prevScore;
			SDL_Color color = { 255,255,255,255 };
			SDL_Texture* tex = TextDisplay::Instance()->renderText(prevScore, "Assets/ca.ttf", color, 15, Game::Instance()->getRenderer());
			SpriteManager::instance()->addTexture("highscore", tex);
		}
	}
}

