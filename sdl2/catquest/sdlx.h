#ifndef __SDLX_H__
#define __SDLX_H__

#include <SDL2/SDL.h>
#include <cmath>

SDL_Rect operator+(const SDL_Rect&, const SDL_Point&);
SDL_Rect operator-(const SDL_Rect&, const SDL_Point&);
SDL_Rect operator-(const SDL_Rect&, const SDL_Point*);
SDL_Rect operator|(const SDL_Rect&, const SDL_Point&);
SDL_Rect operator+(const SDL_Rect&, const SDL_Rect&);
SDL_Rect operator-(const SDL_Rect&, const SDL_Rect&);
SDL_Rect operator*(const SDL_Rect&, const SDL_Point&);
SDL_Rect operator&(const SDL_Rect&, const SDL_Point&);
SDL_Rect operator<<(const SDL_Rect&, int);
SDL_Point operator+(const SDL_Point&, const SDL_Point&);
SDL_Point operator+=(SDL_Point&, const SDL_Point&);
SDL_Point operator-(const SDL_Point&, const SDL_Point&);
SDL_Point operator-=(SDL_Point&, const SDL_Point&);
SDL_Point abs(const SDL_Point&);
SDL_Point operator/(const SDL_Point&, const SDL_Point&);
#endif
