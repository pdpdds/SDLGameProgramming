#include "InputController.h"
#include "Game.h"

InputController* InputController::s_pInstance = 0;

InputController::InputController() :m_mousePosition(new Vector2D(0, 0)), m_keyState(0)
{
	for (int i = 0; i < 3; ++i)
	{
		m_mouseButtonStates.push_back(false);
	}
}

InputController* InputController::Instance()
{
	if (s_pInstance == 0)
	{
		s_pInstance = new InputController();
		return s_pInstance;
	}

	return s_pInstance;
}

void InputController::update()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			Game::Instance()->quit();
			break;

		case SDL_KEYDOWN:
			onKeyDown();
			break;

		case SDL_KEYUP:
			onKeyUp();
			break;

		case SDL_MOUSEMOTION:
			onMouseMove(event);
			break;

		case SDL_MOUSEBUTTONDOWN:
			onMouseButtonDown(event);
			break;

		case SDL_MOUSEBUTTONUP:
			onMouseButtonUp(event);
			break;

		default:
			break;
		}
	}
}

void InputController::clean()
{

}

bool InputController::isKeyDown(SDL_Scancode key)
{
	if (m_keyState != 0)
	{
		if (m_keyState[key] == 1)
		{
			return true;
		}
		else
			return false;
	}
	return false;
}

void InputController::onKeyDown()
{
	m_keyState = SDL_GetKeyboardState(0);
}

void InputController::onKeyUp()
{
	m_keyState = SDL_GetKeyboardState(0);
}

void InputController::onMouseButtonDown(SDL_Event e)
{
	if (e.button.button == SDL_BUTTON_LEFT)
	{
		m_mouseButtonStates[LEFT] = true;
	}
	if (e.button.button == SDL_BUTTON_MIDDLE)
	{
		m_mouseButtonStates[MIDDLE] = true;
	}
	if (e.button.button == SDL_BUTTON_RIGHT)
	{
		m_mouseButtonStates[RIGHT] = true;
	}
}

void InputController::onMouseButtonUp(SDL_Event e)
{
	if (e.button.button == SDL_BUTTON_LEFT)
	{
		m_mouseButtonStates[LEFT] = false;
	}
	if (e.button.button == SDL_BUTTON_MIDDLE)
	{
		m_mouseButtonStates[MIDDLE] = false;
	}
	if (e.button.button == SDL_BUTTON_RIGHT)
	{
		m_mouseButtonStates[RIGHT] = false;
	}
}

bool InputController::isMouseDown(MOUSE_BUTTONS button)
{
	return m_mouseButtonStates[button];
}

Vector2D* InputController::getMousePosition()
{
	return m_mousePosition;
}

void InputController::onMouseMove(SDL_Event e)
{
	m_mousePosition->setX(e.motion.x);
	m_mousePosition->setY(e.motion.y);
}

