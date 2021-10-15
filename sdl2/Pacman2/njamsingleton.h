#pragma once
#include <SDL2/SDL.h>

extern "C" SDL_Renderer* render_;
extern "C" SDL_Texture* texture_;

void Render(SDL_Surface* pSurface);

#ifdef _WIN32
#define snprintf sprintf_s
#endif