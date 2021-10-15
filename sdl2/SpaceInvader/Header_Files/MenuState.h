#pragma once

#include <vector>
#include "SDLGameObject.h"
#include "GameState.h"

class MenuState :public GameState
{
protected:
	typedef void(*Callback)();

	//Implemented by state's with button callback 
	virtual void setCallbacks(const std::vector<Callback>& callbacks) = 0;
	std::vector<Callback> m_callbacks;
};
