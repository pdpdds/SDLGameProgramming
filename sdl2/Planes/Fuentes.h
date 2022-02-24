/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * fuentes.h
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

#ifndef _FUENTES_H_
#define _FUENTES_H_

#include <iostream>
#include <string>
using namespace std;
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

class Fuentes {
public:
	Fuentes();
	Fuentes(string direccion, char* file, int size);
	~Fuentes();
	TTF_Font* getFuente();
	SDL_Texture* renderizarTexto(SDL_Renderer *renderizado, const string &mensaje, SDL_Color color); //Crea una textura con el texto "mensaje" y con la fuente "fuente" y el color "color".
	string intToString(int numero);

protected:

private:
	void logSDLError(const string &mensaje, ostream &oflujo); //Muestra por el flujo "oflujo" el error "mensaje" producido.

	TTF_Font *fuente;
};

#endif // _FUENTES_H_

