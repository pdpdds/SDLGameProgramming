#pragma once

#include <string>
#include <map>
#include "GameObject.h"

class BaseCreator
{
public:
	//virtual function that will be implemented by classes that will 
	// be data driven
	virtual GameObject* createGameObject() = 0;
	virtual ~BaseCreator() {}
};
