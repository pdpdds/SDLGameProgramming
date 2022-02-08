#ifndef __MODULERENDER_H__
#define __MODULERENDER_H__

#include "Module.h"
#include "Globals.h"
#include <SDL2/SDL_image.h>

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;
//typedef enum SDL_RendererFlip;

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init();
	UpdateStatus PreUpdate();
	UpdateStatus PostUpdate();
	bool CleanUp();

	bool ScreenLeftLimit() const;
	bool ScreenRightLimit() const;

	bool Blit(SDL_Texture* texture, int x, int y, SDL_Rect* section, float speed = 1.0f, SDL_RendererFlip flip_texture = SDL_FLIP_NONE) const;
	bool DrawRect(SDL_Rect* rec) const;
	bool DrawStaticRect(SDL_Rect* rec) const;

public:
	SDL_Renderer* renderer = nullptr;
	SDL_Rect camera;
	double offsetCameraY;
};

#endif // __MODULERENDER_H__