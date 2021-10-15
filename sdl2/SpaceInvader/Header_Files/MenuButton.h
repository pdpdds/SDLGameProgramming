#pragma once

#include "SDLGameObject.h"
#include "BaseCreator.h"

class MenuButton :public SDLGameObject
{
private:
	enum button_state { MOUSE_OUT = 0, MOUSE_OVER = 1, CLICKED = 2 };
	bool m_bReleased;
	int m_iCallbackID;
	void(*m_callback)();

public:
	MenuButton();
	~MenuButton();

	virtual void draw();
	virtual void update();
	virtual void clean();
	virtual void load(LoaderParams* pParams);
	virtual void onCollision() {}

	//Set callback when button is pressed
	void setCallback(void(*callback)());

	//Get button's callback ID
	int getCallbackID();
};

class MenuButtonCreator :public BaseCreator
{
	GameObject* createGameObject()
	{
		return new MenuButton();
	}
};
