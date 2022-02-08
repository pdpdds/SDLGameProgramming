#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "SDL2/SDL.h"
#include "ModulePlayerDhalsim.h"

// Constructor
ModuleRender::ModuleRender()
{
	camera.x = - (100 * SCREEN_SIZE);
	camera.y = 0;
	camera.w = SCREEN_WIDTH * SCREEN_SIZE;
	camera.h = SCREEN_HEIGHT* SCREEN_SIZE;
	offsetCameraY = 0.0f;
}

// Destructor
ModuleRender::~ModuleRender()
{}

// Called before render is available
bool ModuleRender::Init()
{
	LOG("Creating Renderer context");
	bool ret = true;
	Uint32 flags = 0;

	if(VSYNC == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
	}

	renderer = SDL_CreateRenderer(App->window->window, -1, flags);
	
	if(renderer == nullptr)
	{
		LOG("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// PreUpdate
UpdateStatus ModuleRender::PreUpdate()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	int player_one_y_pos = App->player_one->GetDistanceJumped();
	int player_two_y_pos = App->player_two->GetDistanceJumped();

	int max_y_distance = player_one_y_pos;
	if (player_two_y_pos > player_one_y_pos)
		max_y_distance = player_two_y_pos;

	camera.y = (int)((double)max_y_distance * (double)SCREEN_SIZE / 8.0f);

	return UpdateStatus_Continue;
}

// PostUpdate
UpdateStatus ModuleRender::PostUpdate()
{
	SDL_RenderPresent(renderer);
	return UpdateStatus_Continue;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	LOG("Destroying renderer");

	//Destroy window
	if(renderer != nullptr)
	{
		SDL_DestroyRenderer(renderer);
	}

	return true;
}

// Blit to screen
bool ModuleRender::Blit(SDL_Texture* texture, int x, int y, SDL_Rect* section, float speed, SDL_RendererFlip flip_texture) const
{
	bool ret = true;
	SDL_Rect rect;
	rect.x = (int)(camera.x * speed) + x * SCREEN_SIZE;
	if (speed == 0)
		rect.y = y * SCREEN_SIZE;
	else
		rect.y = (int)(camera.y) + y * SCREEN_SIZE;

	if(section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= SCREEN_SIZE;
	rect.h *= SCREEN_SIZE;

	if (SDL_RenderCopyEx(renderer, texture, section, &rect, 0.0f, NULL, flip_texture) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Draw a Rectangle
bool ModuleRender::DrawRect(SDL_Rect* rec) const
{
	SDL_Rect rec_aux;
	rec_aux.x = rec->x * SCREEN_SIZE + App->renderer->camera.x;
	rec_aux.y = rec->y * SCREEN_SIZE + App->renderer->camera.y;
	rec_aux.w = rec->w * SCREEN_SIZE;
	rec_aux.h = rec->h * SCREEN_SIZE;

	if(SDL_RenderFillRect(renderer, &rec_aux) != 0)
		return false;
	return true;
}

// Draw a Rectangle always in the same position of the screen
bool ModuleRender::DrawStaticRect(SDL_Rect* rec) const
{
	SDL_Rect rec_aux;
	rec_aux.x = rec->x * SCREEN_SIZE;
	rec_aux.y = rec->y * SCREEN_SIZE;
	rec_aux.w = rec->w * SCREEN_SIZE;
	rec_aux.h = rec->h * SCREEN_SIZE;

	if (SDL_RenderFillRect(renderer, &rec_aux) != 0)
		return false;
	return true;
}

// Tells if the screen is iin the left limit
bool ModuleRender::ScreenLeftLimit() const
{
	if (camera.x == 0)
		return true;
	return false;
}

// Tells if the screen is in the right limit
bool ModuleRender::ScreenRightLimit() const
{
	if (camera.x == -(200 * SCREEN_SIZE))
		return true;
	return false;
}