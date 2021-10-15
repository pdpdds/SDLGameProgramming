#pragma once

#include <vector>
#include "GameState.h"

class GameStateMachine
{
private:
	std::vector<GameState*> m_gameStates;

public:
	//Change current state without calling previous state's exit
	void pushState(GameState* state);

	//Change current state. Call exit on current state and enter on new state
	void changeState(GameState* state);

	//Remove current state and resume to previous state
	void popState();

	//Called each frame. Updates current GameState's active gameobjects
	void update();

	//Called each frame. Perform render on current GameState's active gameobjects
	void render();

	//Get reference to current state
	GameState* getCurrentState();
};
