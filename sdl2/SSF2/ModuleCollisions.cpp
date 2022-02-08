#include "ModuleCollisions.h"
#include "Application.h"
#include "ModuleRender.h"
#include "SDL2\SDL.h"
#include <queue>
#include "ModuleInput.h"
#include <iostream>

ModuleCollisions::ModuleCollisions(bool start_enabled) : Module(start_enabled)
{
	matrix[ColliderType_PlayerOne][ColliderType_PlayerOne] = false;
	matrix[ColliderType_PlayerOne][ColliderType_BodyPlayerOne] = false;
	matrix[ColliderType_PlayerOne][ColliderType_AttackPlayerOne] = false;
	matrix[ColliderType_PlayerOne][ColliderType_PlayerTwo] = false;
	matrix[ColliderType_PlayerOne][ColliderType_BodyPlayerTwo] = false;
	matrix[ColliderType_PlayerOne][ColliderType_AttackPlayerTwo] = true;
	matrix[ColliderType_PlayerOne][ColliderType_Particles] = true;
	matrix[ColliderType_PlayerOne][ColliderType_Others] = false;

	matrix[ColliderType_BodyPlayerOne][ColliderType_PlayerOne] = false;
	matrix[ColliderType_BodyPlayerOne][ColliderType_BodyPlayerOne] = false;
	matrix[ColliderType_BodyPlayerOne][ColliderType_AttackPlayerOne] = false;
	matrix[ColliderType_BodyPlayerOne][ColliderType_PlayerTwo] = false;
	matrix[ColliderType_BodyPlayerOne][ColliderType_BodyPlayerTwo] = true;
	matrix[ColliderType_BodyPlayerOne][ColliderType_AttackPlayerTwo] = false;
	matrix[ColliderType_BodyPlayerOne][ColliderType_Particles] = false;
	matrix[ColliderType_BodyPlayerOne][ColliderType_Others] = false;

	matrix[ColliderType_AttackPlayerOne][ColliderType_PlayerOne] = false;
	matrix[ColliderType_AttackPlayerOne][ColliderType_BodyPlayerOne] = false;
	matrix[ColliderType_AttackPlayerOne][ColliderType_AttackPlayerOne] = false;
	matrix[ColliderType_AttackPlayerOne][ColliderType_PlayerTwo] = true;
	matrix[ColliderType_AttackPlayerOne][ColliderType_BodyPlayerTwo] = false;
	matrix[ColliderType_AttackPlayerOne][ColliderType_AttackPlayerTwo] = true;
	matrix[ColliderType_AttackPlayerOne][ColliderType_Particles] = true;
	matrix[ColliderType_AttackPlayerOne][ColliderType_Others] = false;

	matrix[ColliderType_PlayerTwo][ColliderType_PlayerOne] = false;
	matrix[ColliderType_PlayerTwo][ColliderType_BodyPlayerOne] = false;
	matrix[ColliderType_PlayerTwo][ColliderType_AttackPlayerOne] = true;
	matrix[ColliderType_PlayerTwo][ColliderType_PlayerTwo] = false;
	matrix[ColliderType_PlayerTwo][ColliderType_BodyPlayerTwo] = false;
	matrix[ColliderType_PlayerTwo][ColliderType_AttackPlayerTwo] = false;
	matrix[ColliderType_PlayerTwo][ColliderType_Particles] = true;
	matrix[ColliderType_PlayerTwo][ColliderType_Others] = false;

	matrix[ColliderType_BodyPlayerTwo][ColliderType_PlayerOne] = false;
	matrix[ColliderType_BodyPlayerTwo][ColliderType_BodyPlayerOne] = true;
	matrix[ColliderType_BodyPlayerTwo][ColliderType_AttackPlayerOne] = false;
	matrix[ColliderType_BodyPlayerTwo][ColliderType_PlayerTwo] = false;
	matrix[ColliderType_BodyPlayerTwo][ColliderType_BodyPlayerTwo] = false;
	matrix[ColliderType_BodyPlayerTwo][ColliderType_AttackPlayerTwo] = false;
	matrix[ColliderType_BodyPlayerTwo][ColliderType_Particles] = false;
	matrix[ColliderType_BodyPlayerTwo][ColliderType_Others] = false;

	matrix[ColliderType_AttackPlayerTwo][ColliderType_PlayerOne] = true;
	matrix[ColliderType_AttackPlayerTwo][ColliderType_BodyPlayerOne] = false;
	matrix[ColliderType_AttackPlayerTwo][ColliderType_AttackPlayerOne] = true;
	matrix[ColliderType_AttackPlayerTwo][ColliderType_PlayerTwo] = false;
	matrix[ColliderType_AttackPlayerTwo][ColliderType_BodyPlayerTwo] = false;
	matrix[ColliderType_AttackPlayerTwo][ColliderType_AttackPlayerTwo] = false;
	matrix[ColliderType_AttackPlayerTwo][ColliderType_Particles] = true;
	matrix[ColliderType_AttackPlayerTwo][ColliderType_Others] = false;

	matrix[ColliderType_Particles][ColliderType_PlayerOne] = true;
	matrix[ColliderType_Particles][ColliderType_BodyPlayerOne] = false;
	matrix[ColliderType_Particles][ColliderType_AttackPlayerOne] = true;
	matrix[ColliderType_Particles][ColliderType_PlayerTwo] = true;
	matrix[ColliderType_Particles][ColliderType_BodyPlayerTwo] = false;
	matrix[ColliderType_Particles][ColliderType_AttackPlayerTwo] = true;
	matrix[ColliderType_Particles][ColliderType_Particles] = true;
	matrix[ColliderType_Particles][ColliderType_Others] = false;

	matrix[ColliderType_Others][ColliderType_PlayerOne] = false;
	matrix[ColliderType_Others][ColliderType_BodyPlayerOne] = false;
	matrix[ColliderType_Others][ColliderType_AttackPlayerOne] = false;
	matrix[ColliderType_Others][ColliderType_PlayerTwo] = false;
	matrix[ColliderType_Others][ColliderType_BodyPlayerTwo] = false;
	matrix[ColliderType_Others][ColliderType_AttackPlayerTwo] = false;
	matrix[ColliderType_Others][ColliderType_Particles] = false;
	matrix[ColliderType_Others][ColliderType_Others] = false;

	show_colliders = false;
}


ModuleCollisions::~ModuleCollisions()
{
}

UpdateStatus ModuleCollisions::PreUpdate()
{
	if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN) {
		if (show_colliders)
			show_colliders = false;
		else
			show_colliders = true;
	}
	return UpdateStatus_Continue;
}

UpdateStatus ModuleCollisions::Update()
{

	for (list<Collider*>::iterator it1 = colliders.begin(); it1 != colliders.end(); ++it1)
	{
		list<Collider*>::iterator it2 = it1;
		++it2;
		while (it2 != colliders.end())
		{
			if (CanCollide((*it1)->type, (*it2)->type) && IsCollision((*it1)->rect, (*it2)->rect))
			{
				(*it1)->module->OnCollision((*it1), (*it2));
				(*it2)->module->OnCollision((*it2), (*it1));
				if ((*it1)->type == ColliderType_Particles)
					(*it1)->needDelete = true;
				if ((*it2)->type == ColliderType_Particles)
					(*it2)->needDelete = true;

			}
			++it2;
		}
	}
	
	return UpdateStatus_Continue;
}

UpdateStatus ModuleCollisions::PostUpdate()
{
	if (show_colliders)
	{
		for (list<Collider*>::iterator it = colliders.begin(); it != colliders.end(); ++it)
		{
			if ((*it)->type == ColliderType_Particles || (*it)->type == ColliderType_AttackPlayerOne || (*it)->type == ColliderType_AttackPlayerTwo)
				SDL_SetRenderDrawColor(App->renderer->renderer, 255, 0, 0, 100);
			else if ((*it)->type == ColliderType_PlayerOne || (*it)->type == ColliderType_PlayerTwo)
				SDL_SetRenderDrawColor(App->renderer->renderer, 0, 0, 255, 100);
			else if ((*it)->type == ColliderType_BodyPlayerOne || (*it)->type == ColliderType_BodyPlayerTwo)
				SDL_SetRenderDrawColor(App->renderer->renderer, 0, 255, 0, 100);
			if (!App->renderer->DrawRect(&(*it)->rect))
				return UpdateStatus_Error;
		}
	}

	list<Collider*>::iterator it = colliders.begin();
	while (it != colliders.end())
	{
		if ((*it)->needDelete)
			it = colliders.erase(it);
		else
			++it;
	}
	return UpdateStatus_Continue;
}

void ModuleCollisions::AddCollider(SDL_Rect rec, ColliderType type, Module* module)
{
	Collider collider = { rec, type, module };
	colliders.push_back(&collider);
}

void ModuleCollisions::AddCollider(Collider* collider)
{
	colliders.push_back(collider);
}

bool ModuleCollisions::IsCollision(SDL_Rect rec1, SDL_Rect rec2)
{
	if (!(rec2.x > rec1.x + rec1.w
		|| rec2.x + rec2.w < rec1.x
		|| rec2.y > rec1.y + rec1.h
		|| rec2.y + rec2.h < rec1.y))
		return true;
	else
		return false;
}

bool ModuleCollisions::CanCollide(ColliderType type1, ColliderType type2)
{
	return matrix[type1][type2];
}