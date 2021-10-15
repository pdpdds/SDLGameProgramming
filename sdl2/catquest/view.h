#ifndef __VIEW_H__
#define __VIEW_H__

#include <SDL2/SDL.h>
#include "gameobject.h"

class View : public GameObject {
	SDL_Rect *port;
	SDL_Rect *view;
public:
	View(int, int, int, int);
	~View();

	void set_portx(int);
	void set_porty(int);
	void set_portw(int);
	void set_porth(int);
	void set_viewx(int);
	void set_viewy(int);
	void set_vieww(int);
	void set_viewh(int);

	SDL_Rect* getView();
	SDL_Rect* getPort();
};

#endif
