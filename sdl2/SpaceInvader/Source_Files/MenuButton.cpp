#include "MenuButton.h"

MenuButton::MenuButton()
{

}

void MenuButton::load(LoaderParams* pParams)
{
	SDLGameObject::load(pParams);
	m_iCallbackID = pParams->getCallbackID();
	m_iCurrentFrame = MOUSE_OUT;
}

MenuButton::~MenuButton()
{

}

void MenuButton::draw()
{
	SDLGameObject::draw();
}

void MenuButton::update()
{
	Vector2D* mousePos = InputController::Instance()->getMousePosition();

	if (mousePos->getX() < (m_vPosition.getX() + m_iWidth)
		&& mousePos->getX() > m_vPosition.getX()
		&& mousePos->getY() < (m_vPosition.getY() + m_iHeight)
		&& mousePos->getY() > m_vPosition.getY())
	{
		m_iCurrentFrame = MOUSE_OVER;

		if (InputController::Instance()->isMouseDown(LEFT) && m_bReleased)
		{
			m_iCurrentFrame = CLICKED;
			m_callback();
			m_bReleased = false;
		}
		else if (!InputController::Instance()->isMouseDown(LEFT))
		{
			m_bReleased = true;
			m_iCurrentFrame = MOUSE_OVER;
		}
	}
	else
	{
		m_iCurrentFrame = MOUSE_OUT;
	}
}

void MenuButton::clean()
{
	SDLGameObject::clean();
}

int MenuButton::getCallbackID()
{
	return SDLGameObject::m_iCallbackID;
}

void MenuButton::setCallback(void(*callback)())
{
	m_callback = callback;
}

