#include "Globals.h"
#include "Application.h"
#include "ModuleSceneBison.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "SDL2/SDL.h"
#include "ModulePlayerDhalsim.h"

// Creator
ModuleSceneBison::ModuleSceneBison(bool start_enabled) : Module(start_enabled)
{
	// Ground
	ground.x = 0;
	ground.y = 160;
	ground.w = 512;
	ground.h = 56;

	// Background
	background.x = 48;
	background.y = 44;
	background.w = 416;
	background.h = 116;

	// Air
	air.x = 108;
	air.y = 503;
	air.w = 404;
	air.h = 49;

	// Ceiling
	ceiling.x = 32;
	ceiling.y = 0;
	ceiling.w = 438;
	ceiling.h = 32;

	// Black surface
	black_surface.x = 110;
	black_surface.y = 590;
	black_surface.w = 384;
	black_surface.h = 22;

	// Black surface
	black_surface2.x = 110;
	black_surface2.y = 590;
	black_surface2.w = 384;
	black_surface2.h = 22;

	// Bell
	bell.x = 221;
	bell.y = 234;
	bell.w = 70;
	bell.h = 133;

	// Column 1
	column1.x = 82;
	column1.y = 248;
	column1.w = 20;
	column1.h = 134;

	// Column2
	column2.x = 410;
	column2.y = 248;
	column2.w = 20;
	column2.h = 134;

	// Couple with child
	couple_with_child.x = 104;
	couple_with_child.y = 337;
	couple_with_child.w = 40;
	couple_with_child.h = 55;

	// Two boys
	two_boys.x = 296;
	two_boys.y = 334;
	two_boys.w = 20;
	two_boys.h = 58;

	// Animation two mans
	two_mans_one_ground.frames.push_back({ { 144, 336, 24, 56 }, 28, { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, 10});
	two_mans_one_ground.frames.push_back({ { 481, 432, 24, 56 }, 28, { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, 10 });

	// Animation praying man ground
	praying_man_ground.frames.push_back({ { 177, 333, 30, 63 }, 32, { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, 10 });
	praying_man_ground.frames.push_back({ { 374, 432, 30, 63 }, 32, { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, 10 });

	// Animation praying man up
	praying_man_up.frames.push_back({ { 368, 329, 32, 62 }, 31, { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, 10 });
	praying_man_up.frames.push_back({ { 229, 432, 32, 62 }, 31, { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, 10 });

	// Animation three man
	three_man.frames.push_back({ { 320, 336, 39, 56 }, 28, { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, 10 });
	three_man.frames.push_back({ { 275, 432, 39, 56 }, 28, { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, 10 });

	// Life Bar Rect
	life.x = 2;
	life.y = 2;
	life.w = 198;
	life.h = 14;

	// Restart Letters Rect
	restartFont.x = 0;
	restartFont.y = 0;
	restartFont.w = 130;
	restartFont.h = 25;

	// Dhalsim Name Rect
	name.x = 2;
	name.y = 20;
	name.w = 41;
	name.h = 10;

	// Victory Mark Rect
	victory.x = 205;
	victory.y = 4;
	victory.w = 11;
	victory.h = 14;

	restarting = false;
	end = false;
	actualizeFirstTime = true;
}

// Destructor
ModuleSceneBison::~ModuleSceneBison()
{}

// Load assets
bool ModuleSceneBison::Start()
{
	LOG("Loading bison scene");
	
	graphics = App->textures->Load("Game/bison_stage_v2.png");
	miscellaneous = App->textures->Load("Game/miscellaneous_v2.png");
	restartTexture = App->textures->Load("Game/restartFont.png");

	if (!App->player_one->Enable())
		return false;

	if (!App->player_two->Enable())
		return false;

	initialTime = SDL_GetTicks();

	App->audio->PlayMusic("Game/bison.wav");
	
	return true;
}

// UnLoad assets
bool ModuleSceneBison::CleanUp()
{
	LOG("Unloading bison scene");

	two_mans_one_ground.~Animation();
	praying_man_ground.~Animation();
	praying_man_up.~Animation();
	three_man.~Animation();
	
	App->textures->Unload(graphics);
	App->textures->Unload(miscellaneous);

	return true;
}

// Update: draw background
UpdateStatus ModuleSceneBison::Update()
{
	// Draw
	App->renderer->Blit(graphics, -28, 159, &ground, 1.0f); // Ground
	App->renderer->Blit(graphics, 0, 31, &air, 0.0f); // Air
	App->renderer->Blit(graphics, -28, 43, &background, 0.55f); // Background
	App->renderer->Blit(graphics, -16, -1, &ceiling, 0.8f); // Ceiling

	App->renderer->Blit(graphics, 221 - 28, 17, &bell, 1.0f); // Bell
	App->renderer->Blit(graphics, 82 - 28, 31, &column1, 1.0f); // Column1
	App->renderer->Blit(graphics, 410 - 28, 31, &column2, 1.0f); // Column2
	
	App->renderer->Blit(graphics, 296 - 28, 31 + 86, &two_boys, 1.0f); // Two boys
	App->renderer->Blit(graphics, 104 - 28, 31 + 89, &couple_with_child, 1.0f); // Couple with boy

	App->renderer->Blit(graphics, 144 - 28, 31 + 88, &(two_mans_one_ground.GetCurrentFrame()), 1.0f); //
	two_mans_one_ground.NextFrame();
	App->renderer->Blit(graphics, 177 - 28, 31 + 85, &(praying_man_ground.GetCurrentFrame()), 1.0f); //
	praying_man_ground.NextFrame();
	App->renderer->Blit(graphics, 369 - 28, 31 + 81, &(praying_man_up.GetCurrentFrame()), 1.0f); //
	praying_man_up.NextFrame();
	App->renderer->Blit(graphics, 320 - 28, 31 + 88, &(three_man.GetCurrentFrame()), 1.0f); //
	three_man.NextFrame();

	// Draw names
	App->renderer->Blit(miscellaneous, 30, 47, &name, 0.0f); //
	App->renderer->Blit(miscellaneous, 185, 47, &name, 0.0f); //

	// Draw bar life and life rectangles
	App->renderer->Blit(miscellaneous, 30, 34, &life, 0.0f); //
	SDL_SetRenderDrawColor(App->renderer->renderer, 255, 255, 0, 255);
	SDL_Rect rec_aux = { 32, 37, (int)(App->player_one->GetLife() * 89 / 200), 8 };
	App->renderer->DrawStaticRect(&rec_aux);
	rec_aux = { 137, 37, (int)(App->player_two->GetLife() * 89 / 200), 8 };
	App->renderer->DrawStaticRect(&rec_aux);
	SDL_SetRenderDrawColor(App->renderer->renderer, 255, 0, 0, 255);
	rec_aux = { 32 + (int)(App->player_one->GetLife() * 89 / 200), 37, 89 - (int)(App->player_one->GetLife() * 89 / 200), 8 };
	App->renderer->DrawStaticRect(&rec_aux);
	rec_aux = { 137 + (int)(App->player_two->GetLife() * 89 / 200), 37, 89 - (int)(App->player_two->GetLife() * 89 / 200), 8 };
	App->renderer->DrawStaticRect(&rec_aux);

	//Time manager
	SDL_Rect numberRect;
	int timeNow = (int)SDL_GetTicks();
	timeNow -= initialTime;
	timeNow /= 1000;
	timeNow = 99 - timeNow;
	if (timeNow <= 0 && !restarting && !end)
	{
		timeNow = 0;
		if (App->player_one->GetLife() > App->player_two->GetLife())
		{
			App->player_one->SetWin(true);
			App->player_two->SetTime0(true);
			RestartScene(App->player_one->GetWins() + 1);
		}
		else if (App->player_one->GetLife() < App->player_two->GetLife())
		{
			App->player_two->SetWin(true);
			App->player_one->SetTime0(true);
			RestartScene(App->player_two->GetWins() + 1);
		}
		else {
			App->player_one->SetTime0(true);
			App->player_two->SetTime0(true);
			RestartScene(0);
		}
	}
	
	if (timeNow <= 0)
	{
		timeNow = 0;
	}
	numberRect.x = (int)floor(timeNow / 10) * 10;
	numberRect.y = 36;
	numberRect.w = 8;
	numberRect.h = 14;
	App->renderer->Blit(miscellaneous, 121, 50, &numberRect, 0.0f); //
	numberRect.x = (int)floor(timeNow % 10) * 10;
	App->renderer->Blit(miscellaneous, 129, 50, &numberRect, 0.0f); //

	// Draw wins rectangles
	if (App->player_one->GetWins() >= 1)
	{
		App->renderer->Blit(miscellaneous, 3, 34, &victory, 0.0f);
	}
	if (App->player_one->GetWins() >= 2)
	{
		App->renderer->Blit(miscellaneous, 17, 34, &victory, 0.0f);
	}
	if (App->player_two->GetWins() >= 1)
	{
		App->renderer->Blit(miscellaneous, 229, 34, &victory, 0.0f);
	}
	if (App->player_two->GetWins() >= 2)
	{
		App->renderer->Blit(miscellaneous, 243, 34, &victory, 0.0f);
	}

	// Draw black rectangles at the top and at the bottom of the screen
	SDL_SetRenderDrawColor(App->renderer->renderer, 0, 0, 0, 255);
	SDL_Rect black_rectangles = { 0, 0, 256  , 22   };
	App->renderer->DrawStaticRect(&black_rectangles);
	black_rectangles = { 0, 214, 256  , 10 };
	App->renderer->DrawStaticRect(&black_rectangles);

	return UpdateStatus_Continue;
}

// Manages the black fades to restart screens
UpdateStatus ModuleSceneBison::PostUpdate()
{
	if (restarting)
	{
		SDL_SetRenderDrawColor(App->renderer->renderer, 0, 0, 0, 0);
		if (SDL_GetTicks() - timeRestarting > 5000 && SDL_GetTicks() - timeRestarting < 7000)
		{
			SDL_SetRenderDrawColor(App->renderer->renderer, 0, 0, 0, (SDL_GetTicks() - timeRestarting - 5000) / 8);
		}
		else if (SDL_GetTicks() - timeRestarting >= 7000 && SDL_GetTicks() - timeRestarting < 8000)
		{
			SDL_SetRenderDrawColor(App->renderer->renderer, 0, 0, 0, 255);
		}
		else if (SDL_GetTicks() - timeRestarting >= 8000 && SDL_GetTicks() - timeRestarting < 10000 && actualizeFirstTime)
		{
			SDL_SetRenderDrawColor(App->renderer->renderer, 0, 0, 0, 255);
			App->player_one->restartPlayer(false);
			App->player_two->restartPlayer(false);
			App->renderer->camera.x = -(100 * SCREEN_SIZE);
			App->renderer->camera.y = 0;
			App->audio->PlayMusic("bison.wav");
			actualizeFirstTime = false;
		}
		else if (SDL_GetTicks() - timeRestarting >= 8000 && SDL_GetTicks() - timeRestarting < 10000)
		{
			SDL_SetRenderDrawColor(App->renderer->renderer, 0, 0, 0, 255 - (SDL_GetTicks() - timeRestarting - 8000) / 8);
		}
		else if (SDL_GetTicks() - timeRestarting >= 10000)
		{
			SDL_SetRenderDrawColor(App->renderer->renderer, 0, 0, 0, 0);
			restarting = false;
			initialTime = SDL_GetTicks();
			actualizeFirstTime = true;
		}
		SDL_Rect fade_black_rectangle = { 0, 0, 256 * SCREEN_SIZE, 224 * SCREEN_SIZE };
		App->renderer->DrawRect(&fade_black_rectangle);
	}
	else if (end)
	{
		if (SDL_GetTicks() - timeRestarting > 5000 && SDL_GetTicks() - timeRestarting < 7000)
		{

			SDL_SetRenderDrawColor(App->renderer->renderer, 0, 0, 0, (SDL_GetTicks() - timeRestarting - 5000) / 8);

		}
		else if (SDL_GetTicks() - timeRestarting >= 7000)
		{
			SDL_SetRenderDrawColor(App->renderer->renderer, 0, 0, 0, 255);
			SDL_Rect fade_black_rectangle = { 0, 0, 256 * SCREEN_SIZE, 224 * SCREEN_SIZE };
			App->renderer->DrawRect(&fade_black_rectangle);
			App->renderer->Blit(restartTexture, 50, 100, &restartFont, 0.0f);

			if (App->input->GetKey(SDL_SCANCODE_SPACE))
			{
				App->renderer->camera.x = -(100 * SCREEN_SIZE);
				App->renderer->camera.y = 0;
				App->audio->PlayMusic("bison.wav");
				end = false;
				initialTime = SDL_GetTicks();
				actualizeFirstTime = true;
				App->player_one->restartPlayer(true);
				App->player_two->restartPlayer(true);
			}


		}

	}
	return UpdateStatus_Continue;
}

// Used to indicates the necessity of restarting the scene
void ModuleSceneBison::RestartScene(int wins)
{
	timeRestarting = SDL_GetTicks();
	if (wins >= 2)
	{
		end = true;
	}
	else
	{
		restarting = true;
	}
}