/*
 * Sprite.cpp
 *
 *  Created on: 15 de ago. de 2016
 *      Author: colosu
 */

#include "Sprite.h"
#include "SDL2/SDL.h"
#include "Texturas.h"

Sprite::Sprite() {

	sprite = new Texturas*[1];
	nframes = 1;
	cont = 0;
	posx = 0;
	posy = 0;
	estado = 0;
}

Sprite::Sprite(int nf) {

	sprite = new Texturas*[nf];
	nframes = nf;
	cont = 0;
	posx = 0;
	posy = 0;
	estado = 0;
}

Sprite::~Sprite() {

}

void Sprite::addFrame(string direccion, char* file, SDL_Renderer *renderizado) {

	if (cont < nframes) {
		sprite[cont] = new Texturas(direccion, file, renderizado);
		cont++;
	}
}

void Sprite::selFrame(int nf) {

	if (nf < nframes) {
		estado = nf;
	}
}

int Sprite::getw() {

	int w;
	int h;
	SDL_QueryTexture(sprite[estado]->getTextura(), NULL, NULL, &w, &h);
	return w;
}

int Sprite::geth() {

	int w;
	int h;
	SDL_QueryTexture(sprite[estado]->getTextura(), NULL, NULL, &w, &h);
	return h;
}

void Sprite::draw(SDL_Renderer *renderizado) {

	sprite[estado]->renderizarTextura(renderizado, posx, posy);
}

bool Sprite::colision(Sprite sp) {
	int w1, h1, w2, h2, x1, y1, x2, y2;
	w1 = getw(); // ancho del sprite1
	h1 = geth(); // altura del sprite1
	w2 = sp.getw(); // ancho del sprite2
	h2 = sp.geth(); // alto del sprite2
	x1 = getx(); // pos. X del sprite1
	y1 = gety(); // pos. Y del sprite1
	x2 = sp.getx(); // pos. X del sprite2
	y2 = sp.gety(); // pos. Y del sprite2
	if (((x1 + w1) > x2) && ((y1 + h1) > y2) && ((x2 + w2) > x1) && ((y2 + h2) > y1)) {
		return true;
	} else {
		return false;
	}
}
