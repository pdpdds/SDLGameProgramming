
/* Conversation.cpp
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

#include "Player.h"
#include "ResStr.h"

#include "Conversation.h"

namespace AISDL {

/**
 * Constructor.
 * @param text The dialog text.
 */
Conversation::Conversation(std::shared_ptr<ResStr> text) :
	text(text), pageNum(0)
{
}

/**
 * Start a new conversation between a player and the user.
 * @note This does not actually trigger any dialog lines.  Call Next() to
 *       trigger the first line of conversation.
 * @param player The target player.
 */
void Conversation::Start(std::shared_ptr<Player> player)
{
	this->player = player;
	pageNum = 0;
}

/**
 * Trigger the next line of the conversation.
 * @return @c true if there are more lines in the dialog.
 */
bool Conversation::Next()
{
	size_t numPages = text->GetNumPages();

	if (pageNum < numPages - 1) {
		const std::string &playerLine = (*text)[pageNum++];
		SDL_Log("<PLAYER> %s", playerLine.c_str());
		player->Say(playerLine);
		if (pageNum == numPages) {
			return false;
		}

		const std::string &userLine = (*text)[pageNum++];
		SDL_Log("<*YOU*> %s", userLine.c_str());

		return pageNum != numPages;
	}
	else {
		player->Silence();
		return false;
	}
}

}  // namespace AISDL
