#include "sdlx.h"

SDL_Rect operator+(const SDL_Rect &r, const SDL_Point &p) {
	SDL_Rect b;
	b.x = r.x + p.x;
	b.y = r.y + p.y;
	b.w = r.w;
	b.h = r.h;
	return b;
}
SDL_Rect operator-(const SDL_Rect &r, const SDL_Point &p) {
	SDL_Rect b;
	b.x = r.x - p.x;
	b.y = r.y - p.y;
	b.w = r.w;
	b.h = r.h;
	return b;
}
SDL_Rect operator-(const SDL_Rect &r, const SDL_Point *p) {
	SDL_Rect b;
	b.x = r.x - p->x;
	b.y = r.y - p->y;
	b.w = r.w;
	b.h = r.h;
	return b;
}
SDL_Rect operator|(const SDL_Rect &r, const SDL_Point &p) {
	SDL_Rect b;
	b.x = p.x;
	b.y = p.y;
	b.w = r.w;
	b.h = r.h;
	return b;
}
SDL_Rect operator+(const SDL_Rect &r, const SDL_Rect &p) {
	SDL_Rect b;
	b.x = r.x + p.x;
	b.y = r.y + p.y;
	b.w = r.w;
	b.h = r.h;
	return b;
}
SDL_Rect operator-(const SDL_Rect &r, const SDL_Rect &p) {
	SDL_Rect b;
	b.x = r.x - p.x;
	b.y = r.y - p.y;
	b.w = r.w;
	b.h = r.h;
	return b;
}
SDL_Rect operator*(const SDL_Rect &r, const SDL_Point &p) {
	SDL_Rect b;
	b.x = r.x;
	b.y = r.y;
	b.w = r.w * p.x;
	b.h = r.h * p.y;
	if (p.x < 0) {
		b.x -= b.w;
	}
	if (p.y < 0) {
		b.y -= b.h;
	}
	return b;
}
SDL_Rect operator&(const SDL_Rect &r, const SDL_Point &p) {
	SDL_Rect b;
	b.x = r.x * p.x;
	b.y = r.y * p.y;
	b.w = r.w;
	b.h = r.h;
	return b;
}
SDL_Rect operator<<(const SDL_Rect &r, int i) {
	SDL_Rect b;
	b.x = r.x;
	b.y = r.y;
	b.w = r.w;
	b.h = r.h;
	while (i--) {
		int t = b.x;
		b.x = b.y;
		b.y = b.w;
		b.w = b.h;
		b.h = t;
	}
	return b;
}

SDL_Point operator+(const SDL_Point& r, const SDL_Point& p) {
	SDL_Point b;
	b.x = r.x + p.x;
	b.y = r.y + p.y;
	return b;
}
SDL_Point operator+=(SDL_Point& r, const SDL_Point& p) {
	r.x += p.x;
	r.y += p.y;
	return r;
}
SDL_Point operator-(const SDL_Point& r, const SDL_Point& p) {
	SDL_Point b;
	b.x = r.x - p.x;
	b.y = r.y - p.y;
	return b;
}
SDL_Point operator-=(SDL_Point& r, const SDL_Point& p) {
	r.x -= p.x;
	r.y -= p.y;
	return r;
}

SDL_Point abs(const SDL_Point& r) {
	SDL_Point b;
	b.x = std::abs(r.x);
	b.y = std::abs(r.y);
	return b;
}
SDL_Point operator/(const SDL_Point& r, const SDL_Point& p) {
	SDL_Point b = {1,1};
	if(p.x != 0) {
		b.x = r.x / p.x;
	}
	if(p.y != 0) {
		b.y = r.y / p.y;
	}
	return b;
}