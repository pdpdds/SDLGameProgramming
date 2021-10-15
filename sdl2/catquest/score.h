#ifndef __SCORE_H__
#define __SCORE_H__

#include "object.h"
#include "draw.h"
#include <string>

class Score : public Object {
	int val;
	int timer;
public:
	Score(int, int, Sprite* = NULL);

	void setScore(int);
	void step();
	void draw();
};

#endif