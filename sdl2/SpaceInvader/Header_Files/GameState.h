#pragma once

#include <string>
#include <vector>
#include "GameObject.h"

class GameState
{
public:
	virtual void update() = 0;
	virtual void render() = 0;
	virtual std::vector<GameObject*> getActiveGameObjects() = 0;
	virtual void addGameObject(GameObject* obj) = 0;
	virtual void removeGameObject(GameObject* obj) = 0;
	virtual void onEnter() = 0;
	virtual void onExit() = 0;
	virtual std::string getStateID() = 0;
};
