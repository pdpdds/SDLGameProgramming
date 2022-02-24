/*
 * Planes.cpp
 *
 *  Created on: 15 de ago. de 2016
 *      Author: colosu
 */

#include "Planes.h"
#include "Sprite.h"
#include "SDL2/SDL.h"
#include "Settings.h"

Planes::Planes() {

	x = 0;
	y = 0;
	img = NULL;
	mibala = NULL;
	explode = NULL;
	exp.nframe = 1;
	exp.activo = 0;
}

Planes::Planes(int posx, int posy, int nf, string direccion, char* file, SDL_Renderer *renderizado) {

	x = posx;
	y = posy;
	img = new Sprite [nf];
	mibala = new Sprite [1];
	explode = new Sprite [8];
	img->addFrame(direccion, file, renderizado);
	mibala->addFrame(direccion, (char*)"bala.png", renderizado);
	for (int i = 0; i < MAXBALAS; i++) {
		bala[i].x = 0;
		bala[i].y = 0;
	}
	explode->addFrame(direccion, (char*)"explosion1.png", renderizado);
	explode->addFrame(direccion, (char*)"explosion2.png", renderizado);
	explode->addFrame(direccion, (char*)"explosion3.png", renderizado);
	explode->addFrame(direccion, (char*)"explosion4.png", renderizado);
	explode->addFrame(direccion, (char*)"explosion5.png", renderizado);
	explode->addFrame(direccion, (char*)"explosion6.png", renderizado);
	explode->addFrame(direccion, (char*)"explosion7.png", renderizado);
	explode->addFrame(direccion, (char*)"explosion8.png", renderizado);
}

Planes::~Planes() {

}

void Planes::draw(SDL_Renderer *renderizado) {

	img->setx(x);
	img->sety(y);
	if (exp.activo == 1) {
		explosion(renderizado);
	} else {
		img->draw(renderizado);
	}
	for (int i = 0; i <= MAXBALAS; i++) {
		if (bala[i].x != 0) {

			mibala->setx(bala[i].x);
			mibala->sety(bala[i].y);
			mibala->draw(renderizado);
		}
	}
}

void Planes::creaDisparo() {

	int libre = -1;

	for (int i = 0; i <= MAXBALAS; i++) {
		if (bala[i].x == 0)
			libre = i;
	}

	if (libre >= 0) {

		bala[libre].x = getx();
		bala[libre].y = gety();
	}
}

void Planes::mueveBalas() {

	for (int i = 0; i <= MAXBALAS; i++) {
		if (bala[i].x != 0) {
			bala[i].y -= 5;
		}
		if (bala[i].y < 0) {
			bala[i].x = 0;
			bala[i].y = 0;
		}
	}
}

void Planes::explota() {

	exp.activo = 1;
	exp.nframe = 1;
	exp.x = x;
	exp.y = y;
}

void Planes::explosion(SDL_Renderer *renderizado){

	explode->selFrame(exp.nframe);
	explode->setx(exp.x);
	explode->sety(exp.y);
	explode->draw(renderizado);
	exp.nframe = exp.nframe + 1;
	if (exp.nframe >= 8) {
		printf("hola");
		exp.activo = 0;
	}
}
