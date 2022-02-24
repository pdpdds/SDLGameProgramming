/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * vista.cc
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

#include <string>
using namespace std;
#include "Vista.h"
#include "Ventana.h"
#include "Sprite.h"
#include "Fuentes.h"
#include "Map.h"
#include "Settings.h"
#include "SDL2/SDL.h"

Vista::Vista(const string &direc) {
	try {
		direccion = getDireccion(direc);
		window = new Ventana(COLUMNS * SPRITE_W, ROWS * SPRITE_H, 0, 0, TITLE);
		sprites = new Sprite [N_SPRITES];
		fonts = new Fuentes [N_FONTS];
		mapa = new Map(direccion, (char*)"mapa.csv");
		contSprites = 0;
		contFonts = 0;
	} catch (exception *e) {
		throw e;
	}
}

Vista::~Vista() {
	delete window;
}

Ventana* Vista::getWindow() {

	return window;
}

void Vista::addSprite(int nf) {

	if (contSprites < N_SPRITES) {
		sprites[contSprites] = Sprite(nf);
		contSprites++;
	}
}

void Vista::addFrame(char* file, int sprite) {

	if (sprite < contSprites) {
		sprites[sprite].addFrame(direccion, file, window->getRenderizado());
	}
}

void Vista::addFont(char* file, int size) {

	if (contFonts < N_FONTS) {
		fonts[contFonts] = Fuentes(direccion, file, size);
		contFonts++;
	}
}

void Vista::updateVista() {

	int indice = 0;
	int indice_y = 0;
	int** map = mapa->getMapa();
	mapa->scroll(indice, indice_y);
	int x, y;
	for (int i = indice - 1; i < indice + ROWS; i++) {
		for (int j = 0; j < COLUMNS; j++) {

			// calculo de la posición del tile
			x = j*SPRITE_W;
			y = (i - indice)*SPRITE_H + indice_y;

			// dibujamos el tile
			sprites[map[i][j]].setx(x);
			sprites[map[i][j]].sety(y);
			sprites[map[i][j]].draw(window->getRenderizado());
		}
	}
}

/*
bool Vista::mostrarInicio(int &ancho, int &alto) {

	bool iniciado = false;
	SDL_Texture *titulo = NULL;
	SDL_Texture *inicio = NULL;
	SDL_Texture *automatico = NULL;
	SDL_Texture *manual = NULL;
	SDL_Color color = { 255, 255, 255, 255 };

	inicio = window->getTexturas()->cargarTextura(window->getDireccion() + "inicio.png");
	titulo = window->getTexturas()->renderizarTexto(window->getTexturas()->getFuentes()->getFuentePrincipal(), "2048", color);
	automatico = window->getTexturas()->renderizarTexto(window->getTexturas()->getFuentes()->getFuenteSecundaria(), "Automático", color);
	manual = window->getTexturas()->renderizarTexto(window->getTexturas()->getFuentes()->getFuenteSecundaria(), "Manual", color);

	if (inicio != NULL && titulo != NULL && automatico != NULL && manual != NULL) {

		SDL_RenderClear(window->getTexturas()->getRenderizado());

		SDL_QueryTexture(inicio, NULL, NULL, &ancho, &alto);
		window->getTexturas()->renderizarTextura(inicio, (window->getX() / 2) - (ancho / 2), ((window->getY() + window->getExtraY()) / 10)*5 - (alto / 2));
		window->getTexturas()->renderizarTextura(inicio, (window->getX() / 2) - (ancho / 2), ((window->getY() + window->getExtraY()) / 10)*8 - (alto / 2));
		SDL_QueryTexture(titulo, NULL, NULL, &ancho, &alto);
		window->getTexturas()->renderizarTextura(titulo, (window->getX() / 2) - (ancho / 2), ((window->getY() + window->getExtraY()) / 5) - (alto / 2));
		SDL_QueryTexture(automatico, NULL, NULL, &ancho, &alto);
		window->getTexturas()->renderizarTextura(automatico, (window->getX() / 2) - (ancho / 2), ((window->getY() + window->getExtraY()) / 10)*5 - (alto / 2));
		SDL_QueryTexture(manual, NULL, NULL, &ancho, &alto);
		window->getTexturas()->renderizarTextura(manual, (window->getX() / 2) - (ancho / 2), ((window->getY() + window->getExtraY()) / 10)*8 - (alto / 2));

		SDL_RenderPresent(window->getTexturas()->getRenderizado());

		iniciado = true;
	} else {

		iniciado = false;
	}

	SDL_QueryTexture(inicio, NULL, NULL, &ancho, &alto);

	SDL_DestroyTexture(inicio);
	SDL_DestroyTexture(titulo);
	SDL_DestroyTexture(automatico);
	SDL_DestroyTexture(manual);

	return iniciado;
}

bool Vista::mostrarFinal(int &ancho, int &alto) {

	bool iniciado = false;
	SDL_Texture *titulo = NULL;
	SDL_Texture *inicio = NULL;
	SDL_Texture *automatico = NULL;
	SDL_Texture *manual = NULL;
	SDL_Color color = { 255, 255, 255, 255 };

	inicio = window->getTexturas()->cargarTextura(window->getDireccion() + "inicio.png");
	if (controlador->getPartida()->getMax() == 2048) {

		titulo = window->getTexturas()->renderizarTexto(window->getTexturas()->getFuentes()->getFuentePrincipal(), "Ganaste", color);
	} else {

		titulo = window->getTexturas()->renderizarTexto(window->getTexturas()->getFuentes()->getFuentePrincipal(), "Perdiste", color);
	}
	automatico = window->getTexturas()->renderizarTexto(window->getTexturas()->getFuentes()->getFuenteSecundaria(), "Reintentar", color);
	manual = window->getTexturas()->renderizarTexto(window->getTexturas()->getFuentes()->getFuenteSecundaria(), "Salir", color);

	if (inicio != NULL && titulo != NULL && automatico != NULL && manual != NULL) {

		SDL_RenderClear(window->getTexturas()->getRenderizado());

		SDL_QueryTexture(inicio, NULL, NULL, &ancho, &alto);
		window->getTexturas()->renderizarTextura(inicio, (window->getX() / 2) - (ancho / 2), ((window->getY() + window->getExtraY()) / 10)*6 - (alto / 2));
		window->getTexturas()->renderizarTextura(inicio, (window->getX() / 2) - (ancho / 2), ((window->getY() + window->getExtraY()) / 20)*17 - (alto / 2));
		SDL_QueryTexture(titulo, NULL, NULL, &ancho, &alto);
		window->getTexturas()->renderizarTextura(titulo, (window->getX() / 2) - (ancho / 2), ((window->getY() + window->getExtraY()) / 5) - (alto / 2));
		SDL_QueryTexture(automatico, NULL, NULL, &ancho, &alto);
		window->getTexturas()->renderizarTextura(automatico, (window->getX() / 2) - (ancho / 2), ((window->getY() + window->getExtraY()) / 10)*6 - (alto / 2));
		SDL_QueryTexture(manual, NULL, NULL, &ancho, &alto);
		window->getTexturas()->renderizarTextura(manual, (window->getX() / 2) - (ancho / 2), ((window->getY() + window->getExtraY()) / 20)*17 - (alto / 2));

		max = controlador->getPartida()->getMax();
		punt = controlador->getPartida()->getPunt();
		window->getTexturas()->renderizarTextura(window->getTexturas()->getPuntuacion(), 50, 150 + 40);
		window->getTexturas()->setMaximo(max);
		window->getTexturas()->setPuntos(punt);
		window->getTexturas()->renderizarTextura(window->getTexturas()->getMaximo(), 100, 150 + 40);
		window->getTexturas()->renderizarTextura(window->getTexturas()->getPuntos(), 280, 150 + 40);

		SDL_RenderPresent(window->getTexturas()->getRenderizado());

		iniciado = true;
	} else {

		iniciado = false;
	}

	SDL_QueryTexture(inicio, NULL, NULL, &ancho, &alto);

	SDL_DestroyTexture(inicio);
	SDL_DestroyTexture(titulo);
	SDL_DestroyTexture(automatico);
	SDL_DestroyTexture(manual);

	return iniciado;
}
*/

string Vista::getDireccion(const string &direccion) {

	return "";
#ifdef _WIN32
	const char PATH_SEP = '\\';
#else
	const char PATH_SEP = '/';
#endif

	static string baseRes;

	if (baseRes.empty()) {

		char *basePath = SDL_GetBasePath();
		if (basePath) {

			baseRes = basePath;
			SDL_free(basePath);
		}
		else {

			cerr << "Error obteniendo la ruta de datos: " << SDL_GetError() << endl;
			return "";
		}

		size_t pos = baseRes.find_last_of("bin") - 2;
		baseRes = baseRes.substr(0, pos) + "resources" + PATH_SEP;
	}

	return direccion.empty() ? baseRes : baseRes + direccion + PATH_SEP;
}
