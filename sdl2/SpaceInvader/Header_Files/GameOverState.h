#pragma once

#include <vector>
#include "GameState.h"
#include "SDLGameObject.h"
#include "MenuState.h"

class GameOverState :public MenuState
{
private:
	std::vector<GameObject*> m_gameObjects;
	std::vector<std::string> m_spriteIDs;
	std::vector<Callback> m_callbacks;
	const std::string m_strStateID = "GAMEOVER";

	static void s_gameOverToMain();
	static void s_restartPlay();

public:
	virtual void update();
	virtual void render();
	virtual void onEnter();
	virtual void onExit();
	virtual std::string getStateID();
	virtual std::vector<GameObject*> getActiveGameObjects();
	virtual void removeGameObject(GameObject* obj);
	virtual void addGameObject(GameObject* obj);
	virtual void setCallbacks(const std::vector<Callback>& callbacks);
};
