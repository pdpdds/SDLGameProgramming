/*#include "MenuState.h"
#include "MenuButton.h"
#include <iostream>
#include "PlayState.h"

MenuState::MenuState()
{
}

void MenuState::onEnter()
{
	std::cout << "ON MENU ENTER" << std::endl;
	SpriteManager::instance()->load("Assets/button.png", "playButton", Game::Instance()->getRenderer());
	SpriteManager::instance()->load("Assets/exit.png", "exitButton", Game::Instance()->getRenderer());

	m_gameObjects.push_back(new MenuButton(new LoaderParams(100, 100, 200, 80, "playButton",3),s_menuToPlay));
	m_gameObjects.push_back(new MenuButton(new LoaderParams(100, 300, 200, 80, "exitButton",3),s_exitFromMenu));
}

void MenuState::onExit()
{
	std::cout << "ON MENU EXIT" << std::endl;
	for (size_t i = 0; i < m_gameObjects.size(); i++)
	{
		m_gameObjects[i]->clean();
	}

	m_gameObjects.clear();
	SpriteManager::instance()->clearFromMemory("playButton");
	SpriteManager::instance()->clearFromMemory("exitButton");
}

std::string MenuState::getStateID()
{
	return m_strStateID;
}



*/
