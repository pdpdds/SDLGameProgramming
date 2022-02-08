#ifndef __APPLICATION_CPP__
#define __APPLICATION_CPP__

#include<list>
#include "Globals.h"
#include "Module.h"

class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleAudio;
class ModuleFadeToBlack;
class ModuleSceneBison;
class ModuleParticleSystem;
class ModuleCollisions;
class ModulePlayerDhalsim;

class Application
{
public:

	Application();
	~Application();

	bool Init();
	UpdateStatus Update();
	bool CleanUp();

public:
	ModuleRender* renderer;
	ModuleWindow* window;
	ModuleTextures* textures;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleFadeToBlack* fade;
	ModuleSceneBison* scene_bison;
	ModuleParticleSystem* particles;
	ModuleCollisions* collisions;
	ModulePlayerDhalsim* player_one;
	ModulePlayerDhalsim* player_two;

private:
	std::list<Module*> modules;

};

extern Application* App;

#endif // __APPLICATION_CPP__