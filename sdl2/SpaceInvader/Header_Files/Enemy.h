#pragma once

#include "SDLGameObject.h"
#include "BaseCreator.h"
#include "SoundManager.h"

class Enemy :public SDLGameObject
{
private:
	int m_iRow;
	int m_iColumn;
	Vector2D m_vMoveBy;
	float m_iTimeGap;
	Uint32 m_iTimeStamp = 0;

public:
	Enemy();

	virtual void draw();
	virtual void update();
	virtual void clean();
	virtual void load(const LoaderParams* pParams);
	virtual void onCollision();

	//Get enemy's position by row in current horde
	int getRow();

	//Get enemy's position by column in current horde
 	int getColumn();

	//Set enemy's row
	void setRow(int row);

	//Set enemy's column
	void setColumn(int column);

	//Move by given amount
	void setMoveBy(Vector2D vel);

	//Move in downwards direction
	void moveDown(Vector2D vel);

	//Set time gap between each move
	void setTimeGap(int t);

	//Shoot bullet in downwards direction
	void shoot();

	~Enemy();

};
