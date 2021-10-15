#include "Game.h"

const int FPS = 30;
const float DELAY_TIME = 1000.0f / FPS;

int main(int, char**)
{
	Uint32 frameStart, frameTime;
	if (Game::Instance()->initializeGame("Space Invaders - ANAND DHAVLE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN))
	{
		while (Game::Instance()->running())
		{
			frameStart = SDL_GetTicks();

			Game::Instance()->handleEvents();
			Game::Instance()->update();
			Game::Instance()->render();
			
			frameTime = SDL_GetTicks() - frameStart;

			if (frameTime < DELAY_TIME)
			{
				SDL_Delay((int)(DELAY_TIME - frameTime));
			}
		}
	}

	Game::Instance()->clean();
	return 0;
}
