#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include "Vector2D.h"

enum MOUSE_BUTTONS
{
	LEFT = 0,
	MIDDLE = 1,
	RIGHT = 2
};

class InputController
{
private:
	InputController();
	~InputController() {}
	static InputController* s_pInstance;

	const Uint8* m_keyState;
	std::vector<bool> m_mouseButtonStates;
	Vector2D* m_mousePosition;

public:
	//Get singleton instance
	static InputController* Instance();
	
	//Called each game frame
	void update();

	//Clean up and close resources/files
	void clean();

	//Return pressed status of 'key'
	bool isKeyDown(SDL_Scancode key);

	//Called if any key is pressed down
	void onKeyDown();

	//Called if any key is up
	void onKeyUp();

	//Handle mouse button down events
	void onMouseButtonDown(SDL_Event e);

	//Handle mouse button release events
	void onMouseButtonUp(SDL_Event e);

	//Handle mouse motion events
	void onMouseMove(SDL_Event e);

	//Return if 'button' is pressed
	bool isMouseDown(MOUSE_BUTTONS button);

	//Get mouse current position
	Vector2D* getMousePosition();
};
