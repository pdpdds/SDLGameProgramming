#pragma once

#include "MenuState.h"
#include "GameObject.h"

class MainMenuState :public MenuState
{
private:
	virtual void setCallbacks(const std::vector<Callback>& callbacks);
	static void s_menuToPlay();
	static void s_exitFromMenu();
	const std::string m_strMenuID = "MENU";
	std::vector<GameObject*> m_gameObjects;
	std::vector<std::string> m_spriteIDs;
	SDL_Texture* highScoreTexture;

public:
	virtual void update();
	virtual void render();
	virtual void onEnter();
	virtual void onExit();
	virtual std::vector<GameObject*> getActiveGameObjects();
	virtual void removeGameObject(GameObject* obj);
	virtual void addGameObject(GameObject* obj);
	virtual std::string getStateID();

	//Display stored high score in main menu
	void displayHighScore(std::string saveStateID);
};
