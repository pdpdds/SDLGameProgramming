#include "njamsingleton.h"


SDL_Renderer* render_ = NULL;
SDL_Texture* texture_ = NULL;

void Render(SDL_Surface* pSurface)
{
	SDL_Rect srcRect;
	SDL_Rect destRect;

	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = 800;
	srcRect.h = 600;


	destRect.x = 0;
	destRect.y = 0;
	destRect.w = 800;
	destRect.h = 600;

	SDL_UpdateTexture(texture_, NULL, pSurface->pixels, pSurface->pitch);
	SDL_RenderCopyEx(render_, texture_, &srcRect, &destRect, 0, 0, SDL_FLIP_NONE);
	SDL_RenderPresent(render_);
}