
/* App.cpp
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

#include "StdAfx.h"

#include "Exception.h"
#include "FinalScene.h"
#include "ImageScene.h"
#include "InitScene.h"
#include "IntroScene.h"
#include "MainLoopScene.h"
#include "MiscScene.h"
#include "Player.h"
#include "PreloadScene.h"
#include "RenderScene.h"
#include "ResStr.h"
#include "Scene.h"
#include "TextInputScene.h"
#include "TtfScene.h"
#include "TitleScene.h"

#include "App.h"

namespace AISDL {

namespace {
	struct UserEvent {
		enum type {
			FLASH_TIMER,
		};
	};

	// Called by SDL_AddTimer.
	Uint32 HandleFlashTimer(Uint32 interval, void *params)
	{
		// SDL_AddTimer runs callbacks on a separate thread, so to be safe
		// we push an event so we actually handle it on the main thread.
		// See App::OnUserEvent().
		SDL_Event evt;
		evt.type = SDL_USEREVENT;
		evt.user.code = UserEvent::FLASH_TIMER;
		SDL_PushEvent(&evt);

		// Continue firing events on the same interval.
		return interval;
	}
}

/**
 * Constructor.
 * @param startingScene Index of the scene to start.
 */
App::App(int startingScene) :
	SUPER(),
	display(),
	startingScene(startingScene), sceneIdx(-1),
	clockDecor(display),
	keyLeft(0), keyRight(0), keyUp(0), keyDown(0)
{
	AddScene(std::make_shared<TitleScene>(*this, display));
	AddScene(std::make_shared<IntroScene>(*this, display));
	AddScene(std::make_shared<InitScene>(*this, display));
	AddScene(std::make_shared<MainLoopScene>(*this, display));
	AddScene(std::make_shared<TextInputScene>(*this, display));
	AddScene(std::make_shared<RenderScene>(*this, display));
	AddScene(std::make_shared<ImageScene>(*this, display));
	AddScene(std::make_shared<TtfScene>(*this, display));
	AddScene(std::make_shared<MiscScene>(*this, display));
	AddScene(std::make_shared<FinalScene>(*this, display));

	// Create the main player.
	players.emplace_back(std::make_shared<Player>());
}

App::~App()
{
}

/**
 * Add a scene to the end of the scene list.
 * @param scene The scene (may not be @c nullptr).
 */
void App::AddScene(std::shared_ptr<Scene> scene)
{
	scenes.emplace_back(std::move(scene));
}

/**
 * Attempt to attach a game controller.
 * @param idx The joystick index.
 * @return @c true if attached successfully, @c false otherwise.
 */
bool App::AttachController(int idx)
{
	if (!SDL_IsGameController(idx)) return false;

	SDL_GameController *controller = SDL_GameControllerOpen(idx);
	if (!controller) {
		SDL_Log("Could not open controller %d: %s", idx, SDL_GetError());
		return false;
	}

	// Keep track of the instance ID of each controller so we can detach it
	// later -- the detach event returns the instance ID, not the index!
	int instanceId = SDL_JoystickInstanceID(
		SDL_GameControllerGetJoystick(controller));
	gameControllers[instanceId] = controller;

	SDL_Log("Attached controller index=%d instance=%d (%s).",
		idx, instanceId, SDL_GameControllerName(controller));

	return true;
}

/**
 * Attempt to detach a game controller.
 * @param instanceId The joystick instance ID (not the index!).
 * @return @c true if detached successfully, @c false otherwise.
 */
bool App::DetachController(int instanceId)
{
	auto iter = gameControllers.find(instanceId);
	if (iter == gameControllers.end()) {
		SDL_Log("Unable to find controller for instance ID: %d", instanceId);
		return false;
	}
	else {
		SDL_GameControllerClose(iter->second);
		gameControllers.erase(iter);
		SDL_Log("Detached controller instance=%d.", instanceId);
		return true;
	}
}

/**
 * Handle when a controller button is pressed.
 * @param evt The button press event.
 */
void App::OnControllerButtonDown(SDL_ControllerButtonEvent &evt)
{
	const SDL_GameControllerButton btn =
		static_cast<SDL_GameControllerButton>(evt.button);

	switch (btn) {
	case SDL_CONTROLLER_BUTTON_A:
		scene->OnAction();
		break;
	case SDL_CONTROLLER_BUTTON_B:
		scene->OnCancel();
		break;
	case SDL_CONTROLLER_BUTTON_X:
		scene->OnInteract();
		break;
	case SDL_CONTROLLER_BUTTON_LEFTSTICK:
		ResStr::ReloadAll();
		break;
	case SDL_CONTROLLER_BUTTON_BACK:
		clockDecor.Flash();
		break;
	case SDL_CONTROLLER_BUTTON_START:
		//TODO: Switch to TOC (last scene in list).
		break;
	case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
		RequestPrevScene();
		break;
	case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
		RequestNextScene();
		break;
	default:
		// Ignore.
		break;
	}
}

/**
 * Handle when a key is pressed.
 * @param evt The key pressed event.
 */
void App::OnKeyDown(SDL_KeyboardEvent &evt)
{
	switch (evt.keysym.sym) {
	case SDLK_w:
	case SDLK_UP:
		keyUp = -1.0f;
		break;
	case SDLK_d:
	case SDLK_RIGHT:
		keyRight = 1.0f;
		break;
	case SDLK_s:
	case SDLK_DOWN:
		keyDown = 1.0f;
		break;
	case SDLK_a:
	case SDLK_LEFT:
		keyLeft = -1.0f;
		break;
	case SDLK_RETURN:
		if (evt.keysym.mod & KMOD_ALT) {
			// Alt+Enter - Toggle fullscreen.
			display.ToggleFullscreen();
		} else {
			scene->OnAction();
		}
		break;
	case SDLK_ESCAPE:
		scene->OnCancel();
		break;
	case SDLK_e:
		scene->OnInteract();
		break;
	case SDLK_q:
		RequestShutdown();
		break;
	case SDLK_F5:
		ResStr::ReloadAll();
		scene->Reload();
		break;
	case SDLK_TAB:
		clockDecor.Flash();
		break;
	case SDLK_HOME:
		//TODO: Switch to TOC (last scene in list).
		break;
	case SDLK_PAGEUP:
		RequestPrevScene();
		break;
	case SDLK_PAGEDOWN:
		RequestNextScene();
		break;
	default:
		// Ignore.
		break;
	}
}

/**
 * Handle when a key is released.
 * @param evt The key released event.
 */
void App::OnKeyUp(SDL_KeyboardEvent &evt)
{
	switch (evt.keysym.sym) {
	case SDLK_w:
	case SDLK_UP:
		keyUp = 0;
		break;
	case SDLK_d:
	case SDLK_RIGHT:
		keyRight = 0;
		break;
	case SDLK_s:
	case SDLK_DOWN:
		keyDown = 0;
		break;
	case SDLK_a:
	case SDLK_LEFT:
		keyLeft = 0;
		break;
	}
}

/**
 * Handle when a user event is triggered.
 * @param evt The event.
 */
void App::OnUserEvent(SDL_UserEvent &evt)
{
	switch (evt.code) {
	case UserEvent::FLASH_TIMER:
		clockDecor.Flash();
		break;
	default:
		SDL_Log("Unhandled user event: %d", evt.code);
	}
}

/**
 * Main loop.
 */
void App::Run()
{
	bool quit = false;
	SDL_Event evt;
	Uint32 lastTick = 0;

	// SDL automatically enables text input at startup.
	// We only want to use it in TextInputScene, so we turn it off here.
	SDL_StopTextInput();

	// Always start with the preload scene.
	// When the preloader finishes, it'll request to switch to the next scene.
	scene = std::make_shared<PreloadScene>(*this, display);

	// Attach all already-plugged-in controllers.
	for (int i = 0; i < SDL_NumJoysticks(); i++) {
		AttachController(i);
	}

	// Flash the clock every 15 minutes.
	SDL_AddTimer(15 * 60 * 1000, &HandleFlashTimer, nullptr);

	while (!quit) {
		// Process all events that have been triggered since the last
		// frame was rendered.
		while (SDL_PollEvent(&evt) && !quit) {
			switch (evt.type) {
			case SDL_KEYDOWN:
				OnKeyDown(evt.key);
				break;
			case SDL_KEYUP:
				OnKeyUp(evt.key);
				break;

			// SDL_ControllerButtonEvent
			case SDL_CONTROLLERBUTTONDOWN:
				OnControllerButtonDown(evt.cbutton);
				break;

			// SDL_ControllerDeviceEvent
			case SDL_CONTROLLERDEVICEADDED:
				AttachController(evt.cdevice.which);
				break;
			case SDL_CONTROLLERDEVICEREMOVED:
				DetachController(evt.cdevice.which);
				break;
			case SDL_CONTROLLERDEVICEREMAPPED:
				//TODO
				SDL_Log("Remapped controller: %d", evt.cdevice.which);
				break;

			// SDL_UserEvent
			case SDL_USEREVENT:
				OnUserEvent(evt.user);
				break;

			case SDL_QUIT:
				quit = true;
				break;
			}
			// Let the current scene have a chance at handling the event.
			scene->HandleEvent(evt);
		}
		if (quit) break;

		auto tick = SDL_GetTicks();
		if (lastTick == 0) {
			lastTick = tick;
		}
		RenderFrame(*scene, lastTick, tick);
		lastTick = tick;

		// If a new scene was requested, switch to it.
		if (nextScene) {
			// Shut down the old scene.
			scene->Cleanup();

			scene = nextScene;
			nextScene.reset();

			// Set up the new scene.
			players.front()->Reset();
			scene->Reset();

			lastTick = 0;
		}
	}

	// Detach all controllers.
	while (!gameControllers.empty()) {
		DetachController(gameControllers.begin()->first);
	}

	SDL_Log("Shutting down.");
}

void App::RenderFrame(Scene &scene, Uint32 lastTick, Uint32 tick)
{
	scene.Advance(lastTick, tick);
	clockDecor.Advance(tick);

	// We let the scene decide how to clear the frame.
	scene.Render();
	clockDecor.Render();

	SDL_RenderPresent(display.renderer);
}

void App::RequestPrevScene()
{
	if (sceneIdx > 0) {
		sceneIdx--;

		nextScene = scenes[sceneIdx];
		SDL_Log("Switching to %d: %s", sceneIdx, nextScene->title.c_str());
	}
}

void App::RequestNextScene()
{
	if (sceneIdx <= -1) {
		// The scene at -1 is the preload scene; it's followed by the
		// requested starting scene.
		sceneIdx = startingScene;
		if (startingScene < 0 || startingScene >= (int)scenes.size()) {
			std::ostringstream oss;
			oss << "Invalid starting scene index [" << startingScene << "] -- "
				"Starting scene must be in range 0.." << (scenes.size() - 1) <<
				" (inclusive)";
			throw Exception(oss.str());
		}
	}
	else if ((unsigned)sceneIdx == scenes.size() - 1) {
		// At the final scene.
		return;
	}
	else {
		sceneIdx++;
	}

	nextScene = scenes[sceneIdx];
	SDL_Log("Switching to %d: %s", sceneIdx, nextScene->title.c_str());
}

void App::RequestShutdown()
{
	// Handle the shutdown in a calm and orderly fashion, when the
	// main loop gets around to it.
	SDL_Event evt;
	evt.type = SDL_QUIT;
	SDL_PushEvent(&evt);
}

std::shared_ptr<Player> App::GetMainPlayer() const
{
	// The player at index zero is always the main player.
	return players.front();
}

namespace {
	const float DEAD_ZONE_MIN = -0.25f;
	const float DEAD_ZONE_MAX = 0.25f;
}

float App::SampleControllerAxis(SDL_GameController *controller,
	SDL_GameControllerAxis axis)
{
	// Axis is reported in range -38768 to 38767.
	auto a = static_cast<float>(
		SDL_GameControllerGetAxis(controller, axis));

	// Scale the axis to -1.0 to 1.0.
	a = ((2.0f * (a + 32768.0f)) / 65535.0f) - 1.0f;

	// Ignore if the axis is in the dead (resting) zone.
	if (a > DEAD_ZONE_MIN && a < DEAD_ZONE_MAX) {
		a = 0;
	}

	return a;
}

Director::MovementState App::SampleMovement() const
{
	// For now, we just take the average of all controllers which are actually
	// pushed in a direction.
	float x = 0, y = 0;
	float active = 0;

	for (auto iter = gameControllers.cbegin();
		iter != gameControllers.cend(); ++iter)
	{
		auto controller = iter->second;
		auto ax = SampleControllerAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
		auto ay = SampleControllerAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
		if (ax != 0 || ay != 0) {
			x += ax;
			y += ay;
			active++;
		}
	}

	if (keyUp != 0 || keyRight != 0 || keyDown != 0 || keyLeft != 0) {
		x += keyLeft + keyRight;
		y += keyUp + keyDown;
		active++;
	}

	if (active == 0) {
		x = 0;
		y = 0;
	}
	else {
		x /= active;
		y /= active;
	}

	return MovementState(x, y);
}

}  // namespace AISDL
