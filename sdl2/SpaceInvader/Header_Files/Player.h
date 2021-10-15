#pragma once

#include "SDLGameObject.h"
#include "BaseCreator.h"
#include "AnimatedGraphics.h"

class Player:public SDLGameObject
{
private:
	int m_iNumOfLives;
	std::vector<AnimatedGraphics*> m_liveIcons;
	float m_fShootGap = 0.5;
	Uint32 m_iTimeStamp = 0;

public:
	Player();

	virtual void draw();
	virtual void update();
	virtual void clean();
	virtual void load(const LoaderParams* pParams);
	virtual void onCollision();

	//Check if player can move to new position
	bool canMove(int newXPos);

	//Modify number of lives player has
	void ModifyNumOfLives(int modifier)
	{
		m_iNumOfLives += modifier;
	}

	~Player();
};

class PlayerCreator :public BaseCreator
{
	GameObject* createGameObject()
	{
		return new Player();
	}
};

