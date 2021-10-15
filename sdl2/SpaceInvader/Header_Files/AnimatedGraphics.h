#pragma once

#include "SDLGameObject.h"
#include "BaseCreator.h"

class AnimatedGraphics :public SDLGameObject
{
private:
	int m_iAnimSpeed;

public:
	AnimatedGraphics();
	virtual void draw();
	virtual void update();
	virtual void clean();
	virtual void load(const LoaderParams* pParams);
	virtual void onCollision() {}

	//utility function to hide rendered texture
	void hide();
};

class AnimatedGraphicsCreator :public BaseCreator
{
	GameObject* createGameObject()
	{
		return new AnimatedGraphics();
	}
};
