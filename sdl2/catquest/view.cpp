#include "view.h"

View::View(int px, int py, int pw, int ph) {
	port = new SDL_Rect();
	view = new SDL_Rect();
	port->x = px;
	port->y = py;
	port->w = pw;
	port->h = ph;
	view->x = 0;
	view->y = 0;
	view->w = pw;
	view->h = ph;
}
View::~View() {}

void View::set_viewx(int x) {
	view->x = x;
}
void View::set_viewy(int y) {
	view->y = y;
}
void View::set_vieww(int w) {
	view->w = w;
}
void View::set_viewh(int h) {
	view->h = h;
}

void View::set_portx(int x) {
	port->x = x;
}
void View::set_porty(int y) {
	port->y = y;
}
void View::set_portw(int w) {
	port->w = w;
}
void View::set_porth(int h) {
	port->h = h;
}

SDL_Rect* View::getView() {
	return view;
}
SDL_Rect* View::getPort() {
	return port;
}