#include "object.h"

int Object::maxDepth = 0;
int Object::minDepth = 0;
void Object::setMaxDepth(int n) {
	maxDepth = n;
}
int Object::getMaxDepth() {
	return maxDepth;
}
void Object::setMinDepth(int n) {
	minDepth = n;
}
int Object::getMinDepth() {
	return minDepth;
}
void Object::resetDepth() {
	minDepth = 0;
	maxDepth = 0;
}

int Object::getDepth() {
	return depth;
}
void Object::setDepth(int n) {
	depth = n;
	if (depth < minDepth) {
		minDepth = depth;
	} else if(depth > maxDepth) {
		maxDepth = depth;
	}
}

Object::Object() {}
Object::Object(Sprite* n, int x, int y) {
	sprite = n;
	init(x, y);
}
Object::~Object() {
	if(anim_index != 0) {
		delete anim;
	}
	sprite = nullptr;
	anim_index = 0;
}

void Object::init(int x, int y) {
	pos.x = x;
	pos.y = y;
	if(sprite != NULL) {
		bbox.x = 0;
		bbox.y = 0;
		bbox.w = sprite->getWidth();
		bbox.h = sprite->getHeight();
	}
	image_index = 0;
	anim_index = -1;
	anim = NULL;
	image_blend[0] = 255;
	image_blend[1] = 255;
	image_blend[2] = 255;
	image_alpha = 255;
	image_angle = 0;
	image_scale.x = 1;
	image_scale.y = 1;
	spd.x = 0;
	spd.y = 0;
	important = false;
	visible = true;
	depth = 0;
}

void Object::roomStart() {}

void Object::setSprite(Sprite* n) {
	sprite = n;
}
Sprite* Object::getSprite() {
	return sprite;
}

void Object::step() {}

void Object::draw() {
	if (visible) {
		if (sprite != NULL) {
			int frame = 0;
			if (anim != NULL) {
				anim->run(image_index);
				frame = anim->get(image_index);
			} else {
				frame = image_index;
			}
			sprite->colorMod(image_blend);
			sprite->alphaMod(image_alpha);
			SDL_Rect wh = { 0, 0, sprite->getWidth(), sprite->getHeight() };
			sprite->draw_ext(frame, (wh | pos) * image_scale, image_angle);
		}
	}
}

Object* Object::resize(int w, int h) {
	image_scale.x = w / bbox.w;
	image_scale.y = h / bbox.h;
	return this;
}
int Object::getY() {
	return pos.y;
}
int Object::getX() {
	return pos.x;
}
int Object::getW() {
	return bbox.w * image_scale.x;
}
int Object::getH() {
	return bbox.h * image_scale.y;
}

Object* Object::setVisible(bool v) {
	visible = v;
	return this;
}

SDL_Rect Object::get_bbox() {
	return bbox;
}
SDL_Point Object::get_pos() {
	return pos;
}
SDL_Point Object::get_image_scale() {
	return image_scale;
}

bool Object::isImportant() {
	return important;
}

void Object::setAnim(int n) {
	if(anim_index != n) {
		delete anim;
		anim = NULL;
		anim_index = n;
		if(n != -1) {
			anim = new Animation(sprite->getAnim(n));
		}
	}
}
