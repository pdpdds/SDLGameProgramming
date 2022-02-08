#ifndef __MODULEPARTICLESYSTEM_H__
#define __MODULEPARTICLESYSTEM_H__

#include "Module.h"
#include "ParticleAnimation.h"
#include "Globals.h"
#include "Application.h"
#include "Point.h"
#include <list>
#include "ModuleCollisions.h"

struct SDL_Texture;

struct Particle
{
	iPoint position;
	Collider* collider;
	SDL_Texture* graphics;
	ParticleAnimation particleAnimation;
	ParticleAnimation particleDestruction;
	int speed;
	bool impact;
	bool erase;
	int counter;
};

class ModuleParticleSystem : public Module
{
public:
	ModuleParticleSystem(bool start_enabled = true);
	virtual ~ModuleParticleSystem();

	virtual bool Start();
	virtual UpdateStatus Update();
	virtual UpdateStatus PostUpdate();

	virtual void OnCollision(Collider* c1, Collider* c2);
	
	bool newParticle(iPoint pos, SDL_Texture* graphics, ParticleAnimation particleAnimation, ParticleAnimation particleDestruction, int speed);

	int GetNumberParticles() const;
	iPoint GetParticlePosition(int id);
	int GetParticleSpeed(int id);

private:
	list<Particle*> particleList;

};

#endif