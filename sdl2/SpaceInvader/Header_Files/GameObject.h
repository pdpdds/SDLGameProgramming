#pragma once

#include <string>
#include <SDL2/SDL.h>
#include "SpriteManager.h"
#include "LoaderParams.h"

class GameObject 
{
protected:
	GameObject () {}
	virtual ~GameObject() {}

public :
	//Do rendering
	virtual void draw() = 0;

	//Called each frame
	virtual void update() = 0;

	//Clean up and close resources
	virtual void clean() = 0;

	//Load using LoaderParams class parameters
	virtual void load(const LoaderParams* pParams) = 0;
};
