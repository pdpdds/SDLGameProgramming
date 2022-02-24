/*
 * main.cpp
 *
 *  Created on: 15 de ago. de 2016
 *      Author: colosu
 */

#include <iostream>
#include <string>
using namespace std;
#include "Vista.h"
#include "Planes.h"
#include "Sprite.h"
#include "SDL2/SDL.h"

Vista *vista;
int done = 0;
Planes *jugador;
Planes *enemigo;
int ini_milisegundos = 0;
int fin_milisegundos = 0;

void DrawScene(SDL_Renderer *renderizado) {

	SDL_RenderClear(renderizado);
	vista->updateVista();
	jugador->draw(renderizado);
	enemigo->draw(renderizado);
	// ¿ha colisionado con la nave?
	SDL_RenderPresent(renderizado);
}

int main() {

	SDL_Event event;

	vista = new Vista();
	jugador = new Planes(400, 400, 1, vista->getDireccion(), (char*)"jugador.png", vista->getWindow()->getRenderizado());
	enemigo = new Planes(100, 100, 1, vista->getDireccion(), (char*)"enemigo.png", vista->getWindow()->getRenderizado());

	vista->addSprite(1);
	vista->addFrame((char*)"tile0.png", 0);
	vista->addSprite(1);
	vista->addFrame((char*)"tile1.png", 1);
	vista->addSprite(1);
	vista->addFrame((char*)"tile2.png", 2);
	vista->addSprite(1);
	vista->addFrame((char*)"tile3.png", 3);

	while (done == 0) {

		ini_milisegundos=SDL_GetTicks();
		jugador->mueveBalas();
		enemigo->mueveBalas();

		if (enemigo->colision(jugador->getBala())) {
			enemigo->explota();
		}
		if (jugador->colision(enemigo->getBala())) {
			jugador->explota();
		}


		// dibujamos el frame
		DrawScene(vista->getWindow()->getRenderizado());

	 	while (SDL_PollEvent(&event)) {

	 		if (event.type == SDL_QUIT) {done=1;}

	 		if (event.type == SDL_KEYDOWN) {

	 			//if (event.key.keysym.sym == SDLK_UP && (jugador->gety() > 0)) {jugador->sety(jugador->gety()-5);}
	 			//if (event.key.keysym.sym == SDLK_DOWN && (jugador->gety() < 620)) {jugador->sety(jugador->gety()+5);}
	 			if (event.key.keysym.sym == SDLK_LEFT && (jugador->getx() > 0)) {jugador->setx(jugador->getx()-5);}
	 			if (event.key.keysym.sym == SDLK_RIGHT && (jugador->getx() < 620)) {jugador->setx(jugador->getx()+5);}
	 			if (event.key.keysym.sym == SDLK_SPACE) {jugador->creaDisparo();}
	 			if (event.key.keysym.sym == SDLK_ESCAPE) {done=1;}

	 		}
	 	}

	 	do {

	 		fin_milisegundos=SDL_GetTicks();
	 	} while (fin_milisegundos-ini_milisegundos<30);
	}

	return 0;
}
