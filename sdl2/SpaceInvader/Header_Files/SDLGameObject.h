#pragma once

#include "Game.h"
#include "GameObject.h"
#include "Vector2D.h"
#include "InputController.h"
#include "GameStateMachine.h"

class SDLGameObject:public GameObject
{
public:
	SDLGameObject();	
	virtual void draw();
	virtual void update();
	virtual void clean();
	virtual void load(const LoaderParams* pParams);

	//Called when collison detected
	virtual void onCollision();

	//Get width of current sprite
	int getWidth();

	//Get Height of current sprite
	int getHeight();

	//Get position of current object
	Vector2D& getPosition();

protected:
	Vector2D m_vPosition;
	Vector2D m_vVelocity;
	Vector2D m_vAcceleration;
	int m_iWidth;
	int m_iHeight;
	int m_iCurrentRow;
	int m_iCurrentFrame;
	int m_iNumOfFrames;
	int m_iCallbackID;
	std::string m_strSpriteID;
};
