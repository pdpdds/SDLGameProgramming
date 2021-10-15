
/* Director.h
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

namespace AISDL {

class Player;

/**
 * Interface for scene manager.
 * @author Michael Imamura
 */
class Director {
public:
	Director() { }
	virtual ~Director() { }

public:
	/**
	 * Stop the current scene and switch to the prev scene.
	 */
	virtual void RequestPrevScene() = 0;

	/**
	 * Stop the current scene and switch to the next scene.
	 */
	virtual void RequestNextScene() = 0;

	/**
	 * Exit the application.
	 */
	virtual void RequestShutdown() = 0;

	/**
	 * Retrieve the main player character.
	 * @return The player (never @c nullptr).
	 */
	virtual std::shared_ptr<Player> GetMainPlayer() const = 0;

	struct MovementState {
		MovementState(float x, float y) : x(x), y(y) { }

		float x;  ///< Horizontal axis (-1.0 to 1.0).
		float y;  ///< Vertical axis (-1.0 to 1.0).
	};

	/**
	 * Sample combined the movement state from all enabled controllers.
	 * @return The current movement state.
	 */
	virtual MovementState SampleMovement() const = 0;
};

}  // namespace AISDL

