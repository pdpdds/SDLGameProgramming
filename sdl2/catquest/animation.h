#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include <vector>
#include "gameobject.h"

class Animation : public GameObject {
	int speed;
	int timer;
	std::vector<int> sequence;
public:
	Animation(int, int, int);
	Animation(Animation*);

	~Animation();

	void setSpeed(int);
	Animation setSequence(std::vector<int>);

	void run(int&);
	int get(int);
};

#endif