#pragma once

#include <vector>
#include "GameState.h"
#include "MenuState.h"
#include "SDLGameObject.h"
#include "MenuButton.h"

class PauseState : public MenuState
{
private:
	static void s_pauseToMain();
	static void s_resumeGame();

	const std::string m_strStateID = "PAUSE";
	std::vector<GameObject*> m_gameObjects;
	std::vector<std::string> m_spriteIDs;

public:
	virtual void update();
	virtual void render();
	virtual void onEnter();
	virtual void onExit();
	virtual std::vector<GameObject*> getActiveGameObjects();
	virtual std::string getStateID();
	virtual void setCallbacks(const std::vector<Callback>& callbacks);
	virtual void removeGameObject(GameObject* obj);
	virtual void addGameObject(GameObject* obj);
};
