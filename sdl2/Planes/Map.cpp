/*
 * Map.cpp
 *
 *  Created on: 15 de ago. de 2016
 *      Author: colosu
 */

#include <fstream>
using namespace std;
#include "Map.h"
#include "Sprite.h"
#include "Settings.h"
#include "SDL2/SDL.h"

Map::Map() {

	mapa = NULL;
	indice = 0;
	indice_y = 0;
}

Map::Map(string direccion, char* file) {
	mapa = new int* [MAP_ROWS];
	for (int i = 0; i < MAP_ROWS; i++) {

		mapa[i] = new int [MAP_COLUMNS];
	}

	ifstream input;
	char in;
	input.open((direccion + file).c_str());

	for (int i = 0; i < MAP_ROWS; i++) {
		for (int j = 0; j < MAP_COLUMNS; j++) {

			input.get(in);
			input.ignore(1);
			mapa[i][j] = (int)in - 48;
		}
	}

	input.close();

	indice = MAP_ROWS - ROWS;
	indice_y = 0;

}

Map::~Map() {

	if (mapa != NULL) {
		for (int i = 0; i < MAP_ROWS; i++) {

			delete mapa[i];
		}
		delete mapa;
	}
}

int** Map::getMapa() {

	return mapa;
}

void Map::scroll(int &indi, int &indi_y) {

	indice_y += 2;
	if (indice_y >= SPRITE_H) {
		indice_y = 0;
		indice -= 1;
	}
	if (indice <= 0) {
		indice = MAP_ROWS-ROWS; // si llegamos al final, empezamos de nuevo.
		indice_y = 0;
	}

	indi = indice;
	indi_y = indice_y;
}
