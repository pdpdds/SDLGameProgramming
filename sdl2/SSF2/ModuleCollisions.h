#ifndef __MODULECOLLISIONS_H__
#define __MODULECOLLISIONS_H__

#include "Module.h"
#include <list>

using namespace std;

struct Collider{

	SDL_Rect rect;
	ColliderType type;
	Module* module;
	bool needDelete;
	int damage;
	DamageType damageType;

	Collider()
	{
		rect = { 0, 0, 0, 0 };
		type = ColliderType_Others;
		module = NULL;
		needDelete = false;
		damage = 0;
		damageType = DamageType_None;
	}

	Collider(SDL_Rect rect, ColliderType type, Module* module)
	{
		this->rect = rect;
		this->type = type;
		this->module = module;
		needDelete = false;
		damage = 0;
		damageType = DamageType_None;
	}

};


class ModuleCollisions :
	public Module
{
public:
	ModuleCollisions(bool start_enabled = true);
	virtual ~ModuleCollisions();

	virtual UpdateStatus PreUpdate();
	virtual UpdateStatus Update();
	virtual UpdateStatus PostUpdate();

	void AddCollider(SDL_Rect rec, ColliderType type, Module* module);
	void AddCollider(Collider* collider);

	bool CanCollide(ColliderType type1, ColliderType type2);
	bool IsCollision(SDL_Rect rec1, SDL_Rect rec2);

private:
	list<Collider*> colliders;
	bool show_colliders;
	bool matrix[8][8];

};

#endif