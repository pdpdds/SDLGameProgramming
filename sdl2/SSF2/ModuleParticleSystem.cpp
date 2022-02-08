#include "ModuleParticleSystem.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "SDL2/SDL.h"
#include "ModuleCollisions.h"
#include <queue>
#include "ParticleAnimation.h"



ModuleParticleSystem::ModuleParticleSystem(bool start_enabled) : Module(start_enabled)
{
}

ModuleParticleSystem::~ModuleParticleSystem()
{
}

bool ModuleParticleSystem::Start()
{
	return true;
}

UpdateStatus ModuleParticleSystem::Update()
{
	for (list<Particle*>::iterator it = particleList.begin(); it != particleList.end(); ++it)
	{		
		++(*it)->counter;
		if (!(*it)->impact && (*it)->counter % 4 != 0)
		{
			(*it)->position.x += (*it)->speed;
			(*it)->collider->rect.x = (*it)->position.x + (*it)->particleAnimation.GetCurrentCollider().x;
			(*it)->collider->rect.y = (*it)->position.y + (*it)->particleAnimation.GetCurrentCollider().y;
			(*it)->collider->rect.w = (*it)->particleAnimation.GetCurrentCollider().w;
			(*it)->collider->rect.h = (*it)->particleAnimation.GetCurrentCollider().h;
			if ((*it)->speed > 0){
				App->renderer->Blit((*it)->graphics, (*it)->position.x - (*it)->particleAnimation.GetCurrentPivot(), (*it)->position.y - (*it)->particleAnimation.GetCurrentFrame().h, &((*it)->particleAnimation.GetCurrentFrame()));
			}
			else if ((*it)->speed < 0){
				App->renderer->Blit((*it)->graphics, (*it)->position.x - (*it)->particleAnimation.GetCurrentPivot(), (*it)->position.y - (*it)->particleAnimation.GetCurrentFrame().h, &((*it)->particleAnimation.GetCurrentFrame()), 1.0f, SDL_FLIP_HORIZONTAL);
			}
			(*it)->particleAnimation.NextFrame();
		}
		else if ((*it)->impact && !(*it)->erase)
		{
			if (!(*it)->particleDestruction.IsEnded())
			{
				App->renderer->Blit((*it)->graphics, (*it)->position.x - (*it)->particleDestruction.GetCurrentPivot(), (*it)->position.y - (*it)->particleDestruction.GetCurrentFrame().h, &((*it)->particleDestruction.GetCurrentFrame()));
				(*it)->particleDestruction.NextFrame();
			}
			else
				(*it)->erase = true;
		}
	}

	return UpdateStatus_Continue;
}

UpdateStatus ModuleParticleSystem::PostUpdate()
{
	for (list<Particle*>::iterator it = particleList.begin(); it != particleList.end(); )
	{
		if ((*it)->erase)
		{
			delete (*it)->collider;
			delete (*it);
			it = particleList.erase(it);
		}
		else
			++it;
	}

	return UpdateStatus_Continue;
}

bool ModuleParticleSystem::newParticle(iPoint pos, SDL_Texture* graphics, ParticleAnimation particleAnimation, ParticleAnimation particleDestruction, int speed)
{
	SDL_Rect rec_collider = { pos.x + particleAnimation.GetCurrentCollider().x, pos.y + particleAnimation.GetCurrentCollider().y, particleAnimation.GetCurrentCollider().w, particleAnimation.GetCurrentCollider().h };
	Collider* collider = new Collider( rec_collider, ColliderType_Particles, this );
	Particle* particle = new Particle{ pos, collider, graphics, particleAnimation, particleDestruction, speed, false, 0 };
	App->collisions->AddCollider(collider);
	particleList.push_back(particle);
	return true;
}

void ModuleParticleSystem::OnCollision(Collider* c1, Collider* c2)
{
	for (list<Particle*>::iterator it1 = particleList.begin(); it1 != particleList.end(); ++it1)
	{
		if ((*it1)->collider == c1)
		{
				(*it1)->impact = true;
		}
	}
}

int ModuleParticleSystem::GetNumberParticles() const
{
	return particleList.size();
}

iPoint ModuleParticleSystem::GetParticlePosition(int id)
{
	list<Particle*>::iterator it1 = particleList.begin();
	for (unsigned int i = 1; i < particleList.size(); ++i)
		++it1;
	return (*it1)->position;
}

int ModuleParticleSystem::GetParticleSpeed(int id)
{
	list<Particle*>::iterator it1 = particleList.begin();
	for (unsigned int i = 1; i < particleList.size(); ++i)
		++it1;
	return (*it1)->speed;
}