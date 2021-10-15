#include "SpriteManager.h"

SpriteManager::SpriteManager()
{

}

SpriteManager::~SpriteManager()
{

}

SpriteManager* SpriteManager::s_pSpriteManager = 0;

bool SpriteManager::load(std::string fileName, std::string id, SDL_Renderer* renderer)
{
	SDL_Surface* pTempSurface = IMG_Load(fileName.c_str());

	if (pTempSurface == 0)
		return false;

	SDL_Texture* pTexture = SDL_CreateTextureFromSurface(renderer, pTempSurface);
	SDL_FreeSurface(pTempSurface);

	if (pTexture != 0)
	{
		m_SpriteMap[id] = pTexture;
		return true;
	}

	return false;
}

void SpriteManager::draw(std::string id, int x, int y, int width, int height, SDL_Renderer* renderer)
{
	SDL_Rect srcRect;
	SDL_Rect destRect;

	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = destRect.w = width;
	srcRect.h = destRect.h = height;
	destRect.x = x;
	destRect.y = y;

	SDL_RenderCopyEx(renderer, m_SpriteMap[id], &srcRect, &destRect, 0, 0, SDL_FLIP_NONE);
}

void SpriteManager::drawFrame(std::string id, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer* renderer)
{
	SDL_Rect srcRect;
	SDL_Rect destRect;

	srcRect.x = width * currentFrame;
	srcRect.y = height *(currentRow - 1);
	srcRect.w = destRect.w = width;
	srcRect.h = destRect.h = height;
	destRect.x = x;
	destRect.y = y;

	SDL_RenderCopyEx(renderer, m_SpriteMap[id], &srcRect, &destRect, 0, 0, SDL_FLIP_NONE);
}

SpriteManager* SpriteManager::instance()
{
	if (s_pSpriteManager == 0)
	{
		s_pSpriteManager = new SpriteManager();
		return s_pSpriteManager;
	}
	return s_pSpriteManager;
}

void SpriteManager::clearFromMemory(std::string id)
{
	m_SpriteMap.erase(id);
}
