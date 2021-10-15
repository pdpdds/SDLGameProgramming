#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include "GameObject.h"
#include "GameStateMachine.h"
#include "CollisionManager.h"

class Game
{

private:
	static Game* s_pInstance;
	SDL_Window* m_ptrWindow;
	SDL_Renderer* m_ptrRenderer;
	GameStateMachine* m_ptrGameStateMachine;
	bool m_bRunning;
	int m_currentFrame;
	std::vector<GameObject*> m_gameObjects;
	Game();

public:
	~Game();

	//Get singleton instance of Game class
	static Game* Instance();

	//Create window with title,position and size
	bool initializeGame(const char* title, int windowXpos, int windowYpos, int width, int height, int flags);

	//Draw frame on screen
	void render();

	//Called each game frame
	void update();

	//Handle input events
	void handleEvents();

	//Clean and close resources
	void clean();

	//Get status if game is running
	bool running();

	//Quit game loop
	void quit();

	//Get SDL_Renderer
	SDL_Renderer* getRenderer() const;

	//Get reference to StateMachine class
	GameStateMachine* getGameStateMachine() const;
};
