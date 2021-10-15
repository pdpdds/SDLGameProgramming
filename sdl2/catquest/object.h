#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "sdlx.h"
#include "gameobject.h"
#include "sprite.h"
#include "animation.h"

class Object : public GameObject {
private:
	static int maxDepth, minDepth;
protected:
	Sprite* sprite;
	int image_index;
	int anim_index;
	Animation* anim;

	Uint8 image_blend[3];
	Uint8 image_alpha;
	double image_angle;
	SDL_Point image_scale;
	SDL_Point spd;
	SDL_Point pos;
	SDL_Rect bbox;
	bool important;
	bool visible;
	int depth;
public:
	Object();
	Object(Sprite*, int = 0, int = 0);
	~Object();
	void init(int, int);
	
	virtual void roomStart();

	void setSprite(Sprite*);
	Sprite* getSprite();

	virtual void step();
	virtual void draw();

	Object* resize(int, int);
	Object* setVisible(bool);

	int getY();
	int getX();
	int getW();
	int getH();
	SDL_Rect get_bbox();
	SDL_Point get_pos();
	SDL_Point get_image_scale();

	bool isImportant();

	void setAnim(int);

	static int getMaxDepth();
	static void setMaxDepth(int);
	static int getMinDepth();
	static void setMinDepth(int);
	static void resetDepth();
	int getDepth();
	void setDepth(int);
};

#include "gamebase.h"

#endif
