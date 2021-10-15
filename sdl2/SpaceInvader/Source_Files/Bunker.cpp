#include "Bunker.h"
#include "PlayState.h"
#include "SoundManager.h"

Bunker::Bunker() :SDLGameObject()
{

}

void Bunker::load(const LoaderParams* pParams)
{
	SDLGameObject::load(pParams);
	m_iCurrentFrame = 0;
}

Bunker::~Bunker()
{
}

void Bunker::draw()
{
	SDLGameObject::draw();
}

void Bunker::update()
{

	SDLGameObject::update();
}

void Bunker::clean()
{

}

void Bunker::onCollision()
{
	SoundManager::Instance()->playSound("bunkerdamage", false);
	if (m_iCurrentFrame < 3)
	{
		++m_iCurrentFrame;
	}
	else
	{
		Game::Instance()->getGameStateMachine()->getCurrentState()->removeGameObject(this);
		PlayState* pState = dynamic_cast<PlayState*>(Game::Instance()->getGameStateMachine()->getCurrentState());
		pState->removeBunkerObj(this);
		delete this;
	}
}
