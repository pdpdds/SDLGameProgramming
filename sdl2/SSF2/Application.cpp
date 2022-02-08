#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleFadeToBlack.h"
#include "ModuleSceneBison.h"
#include "ModuleParticleSystem.h"
#include "ModuleCollisions.h"
#include "ModulePlayerDhalsim.h"

Application::Application()
{

	modules.push_back(input = new ModuleInput());
	modules.push_back(window = new ModuleWindow());

	modules.push_back(textures = new ModuleTextures());
	modules.push_back(audio = new ModuleAudio());
	modules.push_back(collisions = new ModuleCollisions());
	modules.push_back(fade = new ModuleFadeToBlack());

	modules.push_back(scene_bison = new ModuleSceneBison(false));
	modules.push_back(particles = new ModuleParticleSystem());
	modules.push_back(player_one = new ModulePlayerDhalsim(PlayerID_One, false));
	modules.push_back(player_two = new ModulePlayerDhalsim(PlayerID_Two, false));

	modules.push_back(renderer = new ModuleRender());

}

Application::~Application()
{
	for (list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
	{
		RELEASE(*it);
	}
}

bool Application::Init()
{
	bool ret = true;

	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Init(); // we init everything, even if not anabled

	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
	{
		if ((*it)->IsEnabled() == true)
			ret = (*it)->Start();
	}

	// Start the first scene --
	fade->FadeToBlack(scene_bison, nullptr, 3.0f);

	return ret;
}

UpdateStatus Application::Update()
{
	UpdateStatus ret = UpdateStatus_Continue;

	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UpdateStatus_Continue; ++it)
	if ((*it)->IsEnabled() == true)
		ret = (*it)->PreUpdate();

	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UpdateStatus_Continue; ++it)
	if ((*it)->IsEnabled() == true)
		ret = (*it)->Update();

	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UpdateStatus_Continue; ++it)
	if ((*it)->IsEnabled() == true)
		ret = (*it)->PostUpdate();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it){
		if ((*it)->IsEnabled() == true)
			ret = (*it)->CleanUp();
	}

	return ret;
}

