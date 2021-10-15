
/* App.h
 *
 * Copyright (C) 2013 Michael Imamura
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

#pragma once

#include "ClockDecor.h"
#include "Display.h"

#include "Director.h"

namespace AISDL {

class Scene;

/**
 * The main scene manager.
 * @author Michael Imamura
 */
class App : public Director {
	typedef Director SUPER;

public:
	App(int startingScene=0);
	~App();

private:
	void AddScene(std::shared_ptr<Scene> scene);
public:
	template<typename Fn>
	void ForEachScene(Fn fn) {
		std::for_each(scenes.begin(), scenes.end(), fn);
	}
	int GetNumScenes() const { return scenes.size(); }

private:
	bool AttachController(int idx);
	bool DetachController(int instanceId);
	void OnControllerButtonDown(SDL_ControllerButtonEvent &evt);
	void OnKeyDown(SDL_KeyboardEvent &evt);
	void OnKeyUp(SDL_KeyboardEvent &evt);
	void OnUserEvent(SDL_UserEvent &evt);

public:
	void Run();
private:
	void RenderFrame(Scene &scene, Uint32 lastTick, Uint32 tick);

public:
	// Director
	virtual void RequestPrevScene();
	virtual void RequestNextScene();
	virtual void RequestShutdown();
	virtual std::shared_ptr<Player> GetMainPlayer() const;
private:
	static float SampleControllerAxis(SDL_GameController *controller,
		SDL_GameControllerAxis axis);
public:
	virtual MovementState SampleMovement() const;

private:
	Display display;

	int startingScene;
	int sceneIdx;
	std::vector<std::shared_ptr<Scene>> scenes;
	std::shared_ptr<Scene> scene;  ///< Current scene.
	std::shared_ptr<Scene> nextScene;

	typedef std::vector<std::shared_ptr<Player>> players_t;
	players_t players;

	ClockDecor clockDecor;

	/// Instance ID -> controller
	std::map<int, SDL_GameController*> gameControllers;
	float keyLeft, keyRight, keyUp, keyDown;
};

}  // namespace AISDL

