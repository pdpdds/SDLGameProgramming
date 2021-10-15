#include "AnimatedGraphics.h"

AnimatedGraphics::AnimatedGraphics() :SDLGameObject()
{

}

void AnimatedGraphics::load(const LoaderParams* pParams)
{
	SDLGameObject::load(pParams);
	m_iAnimSpeed = pParams->getAnimSpeed();
}

void AnimatedGraphics::draw()
{
	SDLGameObject::draw();
}

void AnimatedGraphics::update()
{
	if (m_iAnimSpeed == 0)
		m_iAnimSpeed = 1;
	m_iCurrentFrame = (int)((SDL_GetTicks() / (1000 * m_iAnimSpeed)) % m_iNumOfFrames);
}

void AnimatedGraphics::clean()
{
	SDLGameObject::clean();
}

void AnimatedGraphics::hide()
{
	m_strSpriteID = "";
}
