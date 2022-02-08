#ifndef __MODULESCENEBISON_H__
#define __MODULESCENEBISON_H__

#include "Module.h"
#include "Globals.h"
#include "Animation.h"

struct SDL_Texture;

class ModuleSceneBison : public Module
{
public:
	ModuleSceneBison(bool start_enabled = true);
	~ModuleSceneBison();

	bool Start();
	UpdateStatus Update();
	UpdateStatus PostUpdate();
	bool CleanUp();

	void RestartScene(int wins);

private:	
	SDL_Texture* graphics = nullptr;
	SDL_Texture* miscellaneous = nullptr;
	SDL_Texture* restartTexture = nullptr;
	SDL_Rect ground;
	SDL_Rect background;
	SDL_Rect air;
	SDL_Rect ceiling;
	SDL_Rect black_surface;
	SDL_Rect black_surface2;
	SDL_Rect bell;
	SDL_Rect column1;
	SDL_Rect column2;
	SDL_Rect couple_with_child;
	SDL_Rect two_boys;
	SDL_Rect life;
	SDL_Rect name;
	SDL_Rect restartFont;
	SDL_Rect victory;
	Animation two_mans_one_ground;
	Animation praying_man_ground;
	Animation praying_man_up;
	Animation three_man;

	Uint32 initialTime;

	bool restarting;
	bool end;
	bool actualizeFirstTime;
	Uint32 timeRestarting;

};

#endif // __MODULESCENEBISON_H__