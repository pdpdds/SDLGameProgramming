#pragma once

#include <vector>
#include "EnemyGroupController.h"
#include "SDLGameObject.h"
#include "GameState.h"
#include "Bullet.h"
#include "Bunker.h"

class Player;

class PlayState :public GameState
{
private:
	const std::string m_strStateID = "PLAY";
	std::vector<GameObject*> m_gameObjects;
	std::vector<std::string> m_spriteIDs;
	EnemyGroupController* m_ptrController;
	Player* m_ptrPlayer;
	std::vector<Bunker*> m_vBunkers;

public:
	virtual void update();
	virtual void render();
	virtual void onEnter();
	virtual void onExit();
	virtual std::string getStateID();
	virtual std::vector<GameObject*> getActiveGameObjects();
	virtual void removeGameObject(GameObject* obj);
	virtual void addGameObject(GameObject* obj);

	//Get reference to EnemyGroupController object
	EnemyGroupController* getEnemyController();

	//Get reference to Player object
	Player& getPlayerGameObject();

	//Get list of active bunkers in the scene
	std::vector<Bunker*> getBunkerBlocks();
	
	//Remove bunker from active game object list
	void removeBunkerObj(Bunker* pBunker);
};
