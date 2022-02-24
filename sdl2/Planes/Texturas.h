/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * texturas.h
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

#ifndef _TEXTURAS_H_
#define _TEXTURAS_H_

#include <string>
using namespace std;
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

class Texturas {
public:
	Texturas();
	Texturas(string direccion, char* file, SDL_Renderer *renderizado);
	~Texturas();
	SDL_Texture* getTextura();
	void renderizarTextura(SDL_Renderer *renderizado, int x, int y, SDL_Rect *clip = NULL); //Renderiza la textura "textura" para mostrarla por pantalla en la posición ("x", "y"). Con "clip" seleccionas una parte de la textura.
	void renderizarTextura(SDL_Renderer *renderizado, SDL_Rect rectangulo, SDL_Rect *clip = NULL); //Renderiza la textura "textura" para mostrarla por pantalla dentro del rectángulo "rectangulo". Con "clip" seleccionas una parte de la textura.

protected:

private:
	void logSDLError(const string &mensaje, ostream &oflujo); //Muestra por el flujo "oflujo" el error "mensaje" producido.

	SDL_Texture *textura;

};

#endif // _TEXTURAS_H_

