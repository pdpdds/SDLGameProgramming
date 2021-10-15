
/* Conversation.h
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
class ResStr;

/**
 * A conversation between the player and the user or NPCs.
 * @author Michael Imamura
 */
class Conversation {
public:
	Conversation(std::shared_ptr<ResStr> text);

public:
	//TODO: Allow participants to be customized.
	void Start(std::shared_ptr<Player> player);
	bool Next();

public:
	unsigned int GetPageNum() const { return pageNum; }

private:
	std::shared_ptr<ResStr> text;
	std::shared_ptr<Player> player;
	unsigned int pageNum;
};

}  // namespace AISDL
