#include "animation.h"
#include <stdio.h>
Animation::Animation(int b, int e, int s) {
	for(int i = b; i < e; i++) {
		sequence.push_back(i);
	}
	speed = s;
	timer = 0;
}
Animation::Animation(Animation *a) {
	sequence = a->sequence;
	speed = a->speed;
	timer = 0;
}
Animation::~Animation() {}

void Animation::setSpeed(int s) {
	speed = s;
}

Animation Animation::setSequence(std::vector<int> seq) {
	sequence.clear();
	sequence = seq;
	return *this;
}

void Animation::run(int &frame) {
	if(!timer) {
		timer = 30/speed;
		frame = (frame + 1) % sequence.size();
	}
	timer--;
}

int Animation::get(int n) {
	return sequence[n];	
};