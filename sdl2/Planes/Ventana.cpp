/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ventana.cc
 * Copyright (C) 2016 Colosu <alfredocolosu@gmail.com>
 *
 * 2048 is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * 2048 is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Ventana.h"
#include <iostream>
#include <string>
#include <cstring>
using namespace std;
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "Texturas.h"
#include "Settings.h"

Ventana::Ventana(string nombre) {

	X = COLUMNS * SPRITE_W;
	Y = ROWS * SPRITE_H;
	extraX = 0;
	extraY = 0;

	try {

		inicializarSDL(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
		inicializarSDL_Image(IMG_INIT_PNG);
		inicializarSDL_ttf();
		inicializarVentana(nombre);
		setRenderizado();

	}
	catch (runtime_error *e) {

		if (strcmp(e->what(), "SDL") == 0) {

			throw new exception();
		}
		else if (strcmp(e->what(), "Image") == 0) {

			SDL_Quit();
			throw new exception();
		}
		else if (strcmp(e->what(), "ttf") == 0) {

			IMG_Quit();
			SDL_Quit();
			throw new exception();
		}
		else if (strcmp(e->what(), "window") == 0) {

			TTF_Quit();
			IMG_Quit();
			SDL_Quit();
			throw new exception();
		}
		else if (strcmp(e->what(), "renderer") == 0) {

			SDL_DestroyWindow(ventana);
			TTF_Quit();
			IMG_Quit();
			SDL_Quit();
			throw new exception();
		}
		else {

			SDL_DestroyRenderer(renderizado);
			SDL_DestroyWindow(ventana);
			TTF_Quit();
			IMG_Quit();
			SDL_Quit();
			throw new exception();
		}
	}
}

Ventana::Ventana(int x, int y, string nombre) {

	X = x;
	Y = y;
	extraX = 0;
	extraY = 0;

	try {

		inicializarSDL(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
		inicializarSDL_Image(IMG_INIT_PNG);
		inicializarSDL_ttf();
		inicializarVentana(nombre);
		setRenderizado();

	}
	catch (runtime_error *e) {

		if (strcmp(e->what(), "SDL") == 0) {

			throw new exception();
		}
		else if (strcmp(e->what(), "Image") == 0) {

			SDL_Quit();
			throw new exception();
		}
		else if (strcmp(e->what(), "ttf") == 0) {

			IMG_Quit();
			SDL_Quit();
			throw new exception();
		}
		else if (strcmp(e->what(), "window") == 0) {

			TTF_Quit();
			IMG_Quit();
			SDL_Quit();
			throw new exception();
		}
		else if (strcmp(e->what(), "renderer") == 0) {

			SDL_DestroyWindow(ventana);
			TTF_Quit();
			IMG_Quit();
			SDL_Quit();
			throw new exception();
		}
		else {

			SDL_DestroyRenderer(renderizado);
			SDL_DestroyWindow(ventana);
			TTF_Quit();
			IMG_Quit();
			SDL_Quit();
			throw new exception();
		}
	}
}

Ventana::Ventana(int x, int y, int extrax, int extray, string nombre) {

	X = x;
	Y = y;
	extraX = extrax;
	extraY = extray;

	try {

		inicializarSDL(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
		inicializarSDL_Image(IMG_INIT_PNG);
		inicializarSDL_ttf();
		inicializarVentana(nombre);
		setRenderizado();

	}
	catch (runtime_error *e) {

		if (strcmp(e->what(), "SDL") == 0) {

			throw new exception();
		}
		else if (strcmp(e->what(), "Image") == 0) {

			SDL_Quit();
			throw new exception();
		}
		else if (strcmp(e->what(), "ttf") == 0) {

			IMG_Quit();
			SDL_Quit();
			throw new exception();
		}
		else if (strcmp(e->what(), "window") == 0) {

			TTF_Quit();
			IMG_Quit();
			SDL_Quit();
			throw new exception();
		}
		else if (strcmp(e->what(), "renderer") == 0) {

			SDL_DestroyWindow(ventana);
			TTF_Quit();
			IMG_Quit();
			SDL_Quit();
			throw new exception();
		}
		else {

			SDL_DestroyRenderer(renderizado);
			SDL_DestroyWindow(ventana);
			TTF_Quit();
			IMG_Quit();
			SDL_Quit();
			throw new exception();
		}
	}
}

Ventana::~Ventana() {

	SDL_DestroyRenderer(renderizado);
	SDL_DestroyWindow(ventana);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

int Ventana::getX() {

	return X;
}

int Ventana::getY() {

	return Y;
}

int Ventana::getExtraX() {

	return extraX;
}

int Ventana::getExtraY() {

	return extraY;
}

SDL_Renderer* Ventana::getRenderizado() {

	return renderizado;
}

void Ventana::inicializarSDL(Uint32 subsistemas) {

	if (SDL_Init(subsistemas) < 0) {

		logSDLError("SDL_Init()", cerr);
		throw new runtime_error("SDL");
	}
}

void Ventana::inicializarSDL_Image(IMG_InitFlags subsistemas) {

	if ((IMG_Init(subsistemas) & subsistemas) != subsistemas) {

		logSDLError("IMG_Init()", cerr);
		throw new runtime_error("Image");
	}
}

void Ventana::inicializarSDL_ttf() {

	if (TTF_Init() != 0) {

		logSDLError("TTF_Init()", cerr);
		throw new runtime_error("ttf");
	}
}

void Ventana::inicializarVentana(string nombre) {

	ventana = SDL_CreateWindow(nombre.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, (X + extraX), (Y + extraY), SDL_WINDOW_SHOWN);
	if (ventana == NULL) {

		logSDLError("SDL_CreateWindow()", cerr);
		throw new runtime_error("window");
	}
}

void Ventana::setRenderizado() {

	renderizado = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderizado == NULL) {

		logSDLError("SDL_CreateRenderer()", cerr);
		throw new runtime_error("renderer");
	}
	else {

		SDL_SetRenderDrawColor(renderizado, 0x00, 0x0, 0x00, 0xFF);
	}
}

void Ventana::logSDLError(const string &mensaje, ostream &oflujo) {

	oflujo << mensaje << " error: " << SDL_GetError() << endl;
}

