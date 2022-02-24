/*
 * Sprite.h
 *
 *  Created on: 15 de ago. de 2016
 *      Author: colosu
 */

#ifndef SPRITE_H_
#define SPRITE_H_

#include "Texturas.h"

class Sprite {
public:
	Sprite();
	Sprite(int nf);
	~Sprite();
	void addFrame(string direccion, char* file, SDL_Renderer *renderizado);
	void selFrame(int nf);
	int frames() {return cont;}
	void setx(int x) {posx = x;}
	void sety(int y) {posy = y;}
	void addx(int c) {posx += c;}
	void addy(int c) {posy += c;}
	int getx() {return posx;}
	int gety() {return posy;}
	int getw();
	int geth();
	void draw(SDL_Renderer *renderizado);
	bool colision(Sprite sp);

private:
	Texturas* *sprite;
	int posx;
	int posy;
	int estado;
	int nframes;
	int cont;
};

#endif /* SPRITE_H_ */
