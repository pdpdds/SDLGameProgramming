#pragma once

#include "SDLGameObject.h"

class Bullet :public SDLGameObject
{
private:
	bool b_isPlayerBullet;
public:
	Bullet();

	virtual void draw();
	virtual void update();
	virtual void clean();
	virtual void load(const LoaderParams* pParams);
	virtual void onCollision();

	//set velocity
	void setVelocity(Vector2D velocity);
	//set position
	void setPosition(Vector2D position);
	//set true to mark that bullet is shot by player
	void setPlayerBullet(bool b_isPlayer)
	{
		b_isPlayerBullet = b_isPlayer;
	}

	~Bullet();
};
