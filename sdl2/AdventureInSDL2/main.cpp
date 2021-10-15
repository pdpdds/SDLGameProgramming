
/*
 * Copyright (C) 2013 Michael Imamura
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

#include "StdAfx.h"

#include <iostream>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "App.h"
#include "Config.h"
#include "Exception.h"

using namespace AISDL;

/**
 * Parse command-line options and set Config flags.
 * @param argc The command-line argument count.
 * @param argv The command-line arguments.
 * @return @c true if options are okay,
 *         @c false if option parsing was aborted.
 * @throws Exception Invalid option encountered.
 */
static bool ProcessCommandLine(int argc, char **argv)
{
	Config &cfg = Config::instance;

	for (int i = 1; i < argc; i++) {
		const std::string arg = argv[i];

		if (arg == "-h" || arg == "--help" || arg == "-?") {
			std::cout <<
				"Usage: adventures-in-sdl2 [options] [startingScene]\n"
				"\n"
				"  -h, --help, --?\n"
				"    Print this help text.\n"
				"\n"
				"  -p, --presentation\n"
				"    Enable presentation mode.\n";
			return false;
		}
		else if (arg == "-p" || arg == "--presentation") {
			SDL_Log("Presentation mode enabled.  "
				"Speaker notes will appear in the logs.");
			cfg.presentationMode = true;
		}
		else {
			bool validScene = false;
			// Remaining argument is the starting scene index.
			try {
				cfg.startingScene = std::stoi(arg);
				if (cfg.startingScene >= 0) {
					validScene = true;
				}
			}
			catch (...) {
			}
			if (!validScene) {
				std::ostringstream oss;
				oss << "Invalid starting scene index: " << arg;
				throw Exception(oss.str());
			}
		}
	}

	return true;
}

/// Initialize each of the SDL subsystems.
static void InitApp()
{
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);

	SDL_Log("Starting up all SDL subsystems and libraries.");

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		throw Exception("SDL init", SDL_GetError());
	}

	int reqFmts, actualFmts;

	reqFmts = IMG_INIT_JPG | IMG_INIT_PNG;
	actualFmts = IMG_Init(reqFmts);
	if ((actualFmts & reqFmts) != reqFmts) {
		throw Exception("SDL_image init", IMG_GetError());
	}

	reqFmts = MIX_INIT_OGG;
	actualFmts = Mix_Init(reqFmts);
	if ((actualFmts & reqFmts) != reqFmts) {
		throw Exception("SDL_mixer init", Mix_GetError());
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512) < 0) {
		throw Exception("SDL_mixer open", Mix_GetError());
	}

	if (TTF_Init() == -1) {
		throw Exception("SDL_ttf init", TTF_GetError());
	}
}

/// Shutdown each of the SDL subsystems.
static void ShutdownApp()
{
	TTF_Quit();

	Mix_HaltChannel(-1);
	Mix_CloseAudio();
	Mix_Quit();

	IMG_Quit();
	SDL_Quit();
}

//#ifdef _WIN32
//int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
//#else
#undef main
int main(int argc, char** argv)
//#endif
{
	srand(static_cast<unsigned int>(time(nullptr)));

#ifdef _WIN32
	int vargc = __argc;
	char **vargv = __argv;
#else
	int vargc = argc;
	char **vargv = argv;
#endif
	try {
		if (!ProcessCommandLine(vargc, vargv)) {
			return 0;
		}
	}
	catch (Exception &ex) {
		std::cerr << ex.what() << std::endl;
		return 1;
	}

	try {
		InitApp();
		App(Config::instance.startingScene).Run();
	}
	catch (Exception &ex) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"Adventures in SDL2", ex.what(), NULL);

		std::cerr << ex.what() << std::endl;

		return 1;
	}

	ShutdownApp();

	return 0;
}
