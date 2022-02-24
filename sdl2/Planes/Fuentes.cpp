/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * fuentes.cc
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

#include "Fuentes.h"

#include <iostream>
#include <string>
#include <stdio.h>
using namespace std;
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

Fuentes::Fuentes() {
	fuente = NULL;
}

Fuentes::Fuentes(string direccion, char* file, int size) {
	try {
		fuente = TTF_OpenFont((direccion + file).c_str(), size);
		if (fuente == NULL) {

			throw new runtime_error("Fuente");
		}
	}
	catch (runtime_error *e) {

		logSDLError("TTF_OpenFont()", cerr);
		TTF_CloseFont(fuente);
		throw e;
	}
}

Fuentes::~Fuentes() {
	TTF_CloseFont(fuente);
}

TTF_Font* Fuentes::getFuente() {

	return fuente;
}


SDL_Texture* Fuentes::renderizarTexto(SDL_Renderer *renderizado, const string &mensaje, SDL_Color color) {

	SDL_Texture *textura = NULL;

	SDL_Surface *superficie = NULL;
	superficie = TTF_RenderUTF8_Blended(fuente, mensaje.c_str(), color);

	if (superficie == NULL) {

		logSDLError("TTF_RenderText_Blended()", cerr);
		throw new runtime_error("Superficie");
	}
	else {

		textura = SDL_CreateTextureFromSurface(renderizado, superficie);

		if (textura == NULL) {

			logSDLError("SDL_CreateTextureFromSurface", cerr);
			throw new runtime_error("Textura");
		}

		SDL_FreeSurface(superficie);
	}

	return textura;
}


string Fuentes::intToString(int numero) {

	string cadena;

	if (numero != 0) {

		cadena = "";

		while (numero != 0) {

			cadena = char((numero % 10) + 48) + cadena;
			numero /= 10;
		}
	}
	else {

		cadena = "0";
	}

	return cadena;
}

void Fuentes::logSDLError(const string &mensaje, ostream &oflujo) {

	oflujo << mensaje << " error: " << SDL_GetError() << endl;
}
