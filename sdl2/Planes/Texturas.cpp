/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * texturas.cc
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

#include "Texturas.h"
#include <iostream>
#include <string>
using namespace std;
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

Texturas::Texturas() {

	textura = NULL;
}

Texturas::Texturas(string direccion, char* file, SDL_Renderer *renderizado) {
	try {
		textura = IMG_LoadTexture(renderizado, (direccion + file).c_str());
		if (textura == NULL) {
			throw new runtime_error("Textura");
		}
	}
	catch (exception *e) {

		logSDLError("IMG_LoadTexture()", cerr);
		SDL_DestroyTexture(textura);
		throw e;
	}
}

Texturas::~Texturas() {
	if (textura != NULL) {
		SDL_DestroyTexture(textura);
	}
}

SDL_Texture* Texturas::getTextura() {

	return textura;
}

void Texturas::renderizarTextura(SDL_Renderer *renderizado, int x, int y, SDL_Rect *clip) {

	SDL_Rect rectangulo;

	rectangulo.x = x;
	rectangulo.y = y;

	if (clip != NULL) {

		rectangulo.w = clip->w;
		rectangulo.h = clip->h;
	}
	else {

		SDL_QueryTexture(textura, NULL, NULL, &rectangulo.w, &rectangulo.h);
	}

	SDL_RenderCopy(renderizado, textura, clip, &rectangulo);
}

void Texturas::renderizarTextura(SDL_Renderer *renderizado, SDL_Rect rectangulo, SDL_Rect *clip) {

	SDL_RenderCopy(renderizado, textura, clip, &rectangulo);
}

void Texturas::logSDLError(const string &mensaje, ostream &oflujo) {

	oflujo << mensaje << " error: " << SDL_GetError() << endl;
}

