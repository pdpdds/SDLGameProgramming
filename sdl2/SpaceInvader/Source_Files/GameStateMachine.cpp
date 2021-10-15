#include "GameStateMachine.h"
#include <iostream>

void GameStateMachine::pushState(GameState* state)
{
	m_gameStates.push_back(state);
	m_gameStates.back()->onEnter();
}

void GameStateMachine::popState()
{
	if (!m_gameStates.empty())
	{
		m_gameStates.back()->onExit();
		delete m_gameStates.back();
		m_gameStates.pop_back();
	}
}

void GameStateMachine::changeState(GameState* state)
{
	if (!m_gameStates.empty())
	{
		if (m_gameStates.back()->getStateID() == state->getStateID())
		{
			return;
		}

		m_gameStates.back()->onExit();
		delete m_gameStates.back();
		m_gameStates.pop_back();
	}

	m_gameStates.push_back(state);
	m_gameStates.back()->onEnter();
}

void GameStateMachine::update()
{
	if (!m_gameStates.empty())
	{
		m_gameStates.back()->update();
	}
}

void GameStateMachine::render()
{
	if (!m_gameStates.empty())
	{
		m_gameStates.back()->render();
	}
}

GameState* GameStateMachine::getCurrentState()
{
	if (!m_gameStates.empty())
	{
		return m_gameStates.back();
	}
}
