#pragma once

#include <SDL2/SDL.h>

const static int s_buffer = 4;

//Detect collision using AABB algorithm
static bool RectRect(SDL_Rect* A, SDL_Rect* B)
{
	//TODO : remove this workaround
	if ((A->w > 1000) || A->w < -1000) { return false; }
	if ((B->w > 1000) || B->w < -1000) { return false; }
	if ((A->h > 1000) || A->h < -1000) { return false; }
	if ((B->h > 1000) || B->h < -1000) { return false; }

	if ((A->y + A->h) <= B->y) { return false; }
	if (A->y >= (B->y + B->h)) { return false; }
	if ((A->x + A->w) <= B->x) { return false; }
	if (A->x >= (B->x + B->w)) { return false; }
	return true;
}
