#pragma once

#include "SDLGameObject.h"
#include "BaseCreator.h"

class Bunker:public SDLGameObject
{
public:
	Bunker();
	~Bunker();
	virtual void draw();
	virtual void update();
	virtual void clean();
	virtual void load(const LoaderParams* pParams);
	virtual void onCollision();

};

class BunkerCreator :public BaseCreator
{
	GameObject* createGameObject()
	{
		return new Bunker();
	}
};
