/*
 * Map.h
 *
 *  Created on: 15 de ago. de 2016
 *      Author: colosu
 */

#ifndef MAP_H_
#define MAP_H_

#include "Sprite.h"
#include "SDL2/SDL.h"

class Map {
public:
	Map();
	Map(string direccion, char* file);
	~Map();
	int** getMapa();
	void scroll(int &indi, int &indi_y);

private:

	int** mapa;
	int indice;
	int indice_y;
};

#endif /* MAP_H_ */
