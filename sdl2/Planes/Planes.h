/*
 * Planes.h
 *
 *  Created on: 15 de ago. de 2016
 *      Author: colosu
 */

#ifndef PLANES_H_
#define PLANES_H_

#include "Sprite.h"
#include "SDL2/SDL.h"
#include "Settings.h"

class Planes {
public:
	Planes();
	Planes(int posx, int posy, int nf, string direccion, char* file, SDL_Renderer *renderizado);
	~Planes();
	void addFrame(string direccion, char* file, SDL_Renderer *renderizado) {img->addFrame(direccion, file, renderizado);};
	void selFrame(int nf) {img->selFrame(nf);};
	int frames() {return img->frames();}
	void setx(int c) {x = c; img->setx(c);}
	void sety(int c) {y = c; img->sety(c);}
	void addx(int c) {img->addx(c);}
	void addy(int c) {img->addy(c);}
	int getx() {return x;}
	int gety() {return y;}
	int getw() {return img->getw();}
	int geth() {return img->geth();}
	Sprite* getSprite() {return img;}
	Sprite* getBala() {return mibala;}
	void draw(SDL_Renderer *renderizado);
	bool colision(Sprite* sp) {return img->colision(*sp);};
	void creaDisparo();
	void mueveBalas();
	void explota();
	void explosion(SDL_Renderer *renderizado);

private:
	int x;
	int y;
	Sprite* img;
	Sprite* mibala;
	Sprite* explode;
	struct disparo {
		int x;
		int y;
	} bala[MAXBALAS + 1];
	struct explosion {
		int activo;
		int x;
		int y;
		int nframe;
	} exp;
};

#endif /* PLANES_H_ */
