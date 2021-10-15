#include "Game.h"
#include "Player.h"
#include "InputController.h"
#include "MainMenuState.h"
#include "PlayState.h"
#include "GameObjectFactory.h"
#include "MenuButton.h"
#include "AnimatedGraphics.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Bunker.h"

Game::Game()
{

}

Game::~Game()
{

}

Game* Game::s_pInstance = 0;

Game* Game::Instance()
{
	if (s_pInstance == 0)
	{
		s_pInstance = new Game();
		return s_pInstance;
	}
	return s_pInstance;
}

bool Game::initializeGame(const char* title, int windowXpos, int windowYpos, int width, int height, int flags)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) >= 0)
	{
		m_ptrWindow = SDL_CreateWindow(title, windowXpos, windowYpos, width, height, flags);
		if (m_ptrWindow != 0)
		{
			m_ptrRenderer = SDL_CreateRenderer(m_ptrWindow, -1, 0);
			if (m_ptrRenderer == 0)
				return false;
			else
			{
				GameObjectFactory::Instance()->registerType("MenuButton", new MenuButtonCreator());
				GameObjectFactory::Instance()->registerType("Player", new PlayerCreator());
				GameObjectFactory::Instance()->registerType("AnimatedGraphic", new AnimatedGraphicsCreator());
				GameObjectFactory::Instance()->registerType("Bunker", new BunkerCreator());
				//GameObjectFactory::Instance()->registerType("Bullet", new BulletCreator());

				m_ptrGameStateMachine = new GameStateMachine();
				m_ptrGameStateMachine->changeState(new MainMenuState());

				m_bRunning = true;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

void Game::render()
{

	SDL_SetRenderDrawColor(m_ptrRenderer, 0, 0, 0, 255);
	SDL_RenderClear(m_ptrRenderer);

	m_ptrGameStateMachine->render();

	SDL_RenderPresent(m_ptrRenderer);
}

void Game::quit()
{
	m_bRunning = false;
}

void Game::handleEvents()
{
	InputController::Instance()->update();

	if (InputController::Instance()->isKeyDown(SDL_SCANCODE_RETURN))
	{
		m_ptrGameStateMachine->changeState(new PlayState());
	}
}

void Game::update()
{
	m_ptrGameStateMachine->update();
}

bool Game::running()
{
	return m_bRunning;
}

SDL_Renderer* Game::getRenderer() const
{
	return m_ptrRenderer;
}

GameStateMachine* Game::getGameStateMachine() const
{
	return m_ptrGameStateMachine;
}

void Game::clean()
{
	SDL_DestroyWindow(m_ptrWindow);
	SDL_DestroyRenderer(m_ptrRenderer);
	SDL_Quit();
}
