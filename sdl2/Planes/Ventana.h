/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ventana.h
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

#ifndef _VENTANA_H_
#define _VENTANA_H_

#include <string>
using namespace std;
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "Texturas.h"

class Ventana {
public:
	Ventana(string nombre = "");
	Ventana(int x, int y, string nombre = "");
	Ventana(int x, int y, int extrax, int extray, string nombre = "");
	~Ventana();
	int getX();
	int getY();
	int getExtraX();
	int getExtraY();
	SDL_Renderer* getRenderizado();
	void inicializarSDL(Uint32 subsistemas); //Inicializa SDL con los subsistemas "subsistemas".
	void inicializarSDL_Image(IMG_InitFlags subsistemas); //Inicia el subsistema de Imagen con los subsistemas "subsistemas".
	void inicializarSDL_ttf(); //Inicializa el subsistema de texto.
	void inicializarVentana(string nombre); //Inicializa la ventana del programa y su capa de renderizado con el nombre "nombre".
	void setRenderizado();

protected:

private:
	void logSDLError(const string &mensaje, ostream &oflujo); //Muestra por el flujo "oflujo" el error "mensaje" producido.

	int X; //Ancho de la ventana.
	int Y; //Alto de la ventana.
	int extraX; //Añadido al ancho de la ventana para colocar un posible HUD.
	int extraY; //Añadido al alto de la ventana para colocar un posible HUD.
	SDL_Window *ventana = NULL; //La ventana.
	SDL_Renderer *renderizado; //El renderizado de la ventana.

};

#endif // _VENTANA_H_

