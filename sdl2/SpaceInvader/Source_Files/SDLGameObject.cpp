#include "SDLGameObject.h"

SDLGameObject::SDLGameObject() :GameObject()
{

}

void SDLGameObject::load(const LoaderParams* pParams)
{
	m_vPosition = Vector2D(pParams->getX(), pParams->getY());
	m_vVelocity = Vector2D(0, 0);
	m_vAcceleration = Vector2D(0, 0);
	m_iWidth = pParams->getWidth();
	m_iHeight = pParams->getHeight();
	m_strSpriteID = pParams->getSpriteID();
	m_iCurrentRow = 1;
	m_iCurrentFrame = 1;
	m_iNumOfFrames = pParams->getNumberOfFrames();
	m_iCallbackID = pParams->getCallbackID();
}

void SDLGameObject::draw()
{
	SpriteManager::instance()->drawFrame(m_strSpriteID, (int)m_vPosition.getX(), (int)m_vPosition.getY(), m_iWidth, m_iHeight, m_iCurrentRow, m_iCurrentFrame, Game::Instance()->getRenderer());
}

void SDLGameObject::update()
{
	m_vVelocity += m_vAcceleration;
	m_vPosition += m_vVelocity;
}

void SDLGameObject::clean()
{

}

int SDLGameObject::getHeight()
{
	return m_iHeight;
}

int SDLGameObject::getWidth()
{
	return m_iWidth;
}

Vector2D& SDLGameObject::getPosition()
{
	return m_vPosition;
}

void SDLGameObject::onCollision()
{

}
