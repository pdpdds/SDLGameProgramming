#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "SDL2/SDL.h"

#include <iostream>
using namespace std;

#define MAX_KEYS 300

ModuleInput::ModuleInput() : Module(), mouse({0, 0}), mouse_motion({0,0})
{
	keyboard = new KeyState[MAX_KEYS];
	joystick = SDL_JoystickOpen(0);
	memset(keyboard, KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KeyState) * NUM_MOUSE_BUTTONS);
}

// Destructor
ModuleInput::~ModuleInput()
{
	RELEASE_ARRAY(keyboard);
}

// Called before render is available
bool ModuleInput::Init()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0)
	{
		LOG("SDL_JOYSTICK could not initialize! SDL_Error: %s\n", SDL_GetError());
//		ret = false;
	}

	//Check for joysticks
	if (SDL_NumJoysticks() < 1)
	{
		LOG("Warning: No joysticks connected!\n");
//		ret = false;
	}
	else
	{
		//Load joystick
		LOG("Joystick present");
		joystick = SDL_JoystickOpen(0);
		if (joystick == NULL)
		{
			LOG("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
//			ret = false;
		}
	}

	return ret;
}

// Called before the first frame
bool ModuleInput::Start()
{
	for (int i = 0; i < NUM_JOYSTICK_BUTTONS; ++i)
		joystick_buttons[i] = KEY_IDLE;
	return true;
}

// Called each loop iteration
UpdateStatus ModuleInput::PreUpdate()
{
	static SDL_Event event;

	mouse_motion = {0, 0};
	memset(windowEvents, false, WE_COUNT * sizeof(bool));
	
	for (int i = 0; i < NUM_JOYSTICK_BUTTONS; ++i)
	{
		if (joystick_buttons[i] == KEY_DOWN)
			joystick_buttons[i] = KEY_REPEAT;
		if (joystick_buttons[i] == KEY_UP)
			joystick_buttons[i] = KEY_IDLE;
	}

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	for(int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if(mouse_buttons[i] == KEY_DOWN)
			mouse_buttons[i] = KEY_REPEAT;

		if(mouse_buttons[i] == KEY_UP)
			mouse_buttons[i] = KEY_IDLE;
	}

	while(SDL_PollEvent(&event) != 0)
	{
		switch(event.type)
		{
			case SDL_QUIT:
				windowEvents[WE_QUIT] = true;
			break;

			case SDL_WINDOWEVENT:
				switch(event.window.event)
				{
					//case SDL_WINDOWEVENT_LEAVE:
					case SDL_WINDOWEVENT_HIDDEN:
					case SDL_WINDOWEVENT_MINIMIZED:
					case SDL_WINDOWEVENT_FOCUS_LOST:
					windowEvents[WE_HIDE] = true;
					break;

					//case SDL_WINDOWEVENT_ENTER:
					case SDL_WINDOWEVENT_SHOWN:
					case SDL_WINDOWEVENT_FOCUS_GAINED:
					case SDL_WINDOWEVENT_MAXIMIZED:
					case SDL_WINDOWEVENT_RESTORED:
					windowEvents[WE_SHOW] = true;
					break;
				}
			break;

			case SDL_MOUSEBUTTONDOWN:
				mouse_buttons[event.button.button - 1] = KEY_DOWN;
			break;

			case SDL_MOUSEBUTTONUP:
				mouse_buttons[event.button.button - 1] = KEY_UP;
			break;

			case SDL_MOUSEMOTION:
				mouse_motion.x = event.motion.xrel / SCREEN_SIZE;
				mouse_motion.y = event.motion.yrel / SCREEN_SIZE;
				mouse.x = event.motion.x / SCREEN_SIZE;
				mouse.y = event.motion.y / SCREEN_SIZE;
			break;

			case SDL_JOYAXISMOTION:
				if (event.jaxis.which == 0)
				{
					if (event.jaxis.axis == 0)
					{
						if (event.jaxis.value < -8000)
						{
							xDir = -1;
						}
						else if (event.jaxis.value > 8000)
						{
							xDir = 1;
						}
						else
						{
							xDir = 0;
						}
					}
					else if (event.jaxis.axis == 1)
					{
						if (event.jaxis.value < -8000)
						{
							yDir = -1;
						}
						else if (event.jaxis.value > 8000)
						{
							yDir = 1;
						}
						else
						{
							yDir = 0;
						}
					}
				}
			break;

			case SDL_JOYBUTTONDOWN:
				joystick_buttons[event.jbutton.button] = KEY_DOWN;
			break;

			case SDL_JOYBUTTONUP:
				joystick_buttons[event.jbutton.button] = KEY_UP;
			break;

		}
	}

	if(GetWindowEvent(EventWindow::WE_QUIT) == true || GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		return UpdateStatus_Stop;

	return UpdateStatus_Continue;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

// ---------
bool ModuleInput::GetWindowEvent(EventWindow ev) const
{
	return windowEvents[ev];
}

const iPoint& ModuleInput::GetMousePosition() const
{
	return mouse;
}

const iPoint& ModuleInput::GetMouseMotion() const
{
	return mouse_motion;
}