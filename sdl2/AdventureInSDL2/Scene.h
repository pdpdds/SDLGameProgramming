
/* Scene.h
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

#include "Director.h"
#include "Display.h"

namespace AISDL {

/**
 * Base class for individual scenes.
 * @author Michael Imamura
 */
class Scene {
public:
	Scene(Director &director, Display &display, const std::string &title);
	virtual ~Scene();

public:
	virtual void OnInteract() { }
	virtual void OnAction() { }
	virtual void OnCancel() { }

public:
	/** Preload assets used specifically by this scene. */
	virtual void Preload() { }

	/** Reload scene-specific assets. */
	virtual void Reload() { }

	/** Reset the scene to the initial state. */
	virtual void Reset() { }

	/** Clean up any temp resources not needed when scene is not in focus. */
	virtual void Cleanup() { }

	/**
	 * Define custom handling of when an SDL event arrives.
	 * @param evt The SDL event.
	 */
	virtual void HandleEvent(SDL_Event &evt) { }

	/**
	 * Prepare the next frame before rendering.
	 * @param lastTick The clock tick of the previous frame (this is equal
	 *                 to @p tick for the first frame).
	 * @param tick The clock tick (milliseconds since app start).
	 */
	virtual void Advance(Uint32 lastTick, Uint32 tick) = 0;

	virtual void Render();

protected:
	virtual void RenderContent() = 0;

protected:
	Director &director;
	Display &display;
public:
	const std::string title;
};

}  // namespace AISDL

