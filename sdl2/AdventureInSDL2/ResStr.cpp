
/* ResStr.cpp
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

#include <fstream>

#include "Exception.h"

#include "ResStr.h"

namespace AISDL {

std::list<std::weak_ptr<ResStr>> ResStr::instances;

/**
 * Constructor.
 *
 * @note You probably want to use Load() instead, so the instance will be
 *       reloaded with ReloadAll().
 *
 * @param filename The file to load the string from.
 */
ResStr::ResStr(const std::string &filename) :
	filename(filename)
{
	Reload();
}

/**
 * Create a new "tracked" resource string.
 *
 * Instances created using this function are tracked and can be reloaded by
 * calling ReloadAll().
 *
 * @param filename The file to load the string from.
 * @return The resource string.
 */
std::shared_ptr<ResStr> ResStr::Load(const std::string &filename)
{
	auto ptr = std::make_shared<ResStr>(filename);

	// Keep a list of loaded resources for ReloadAll().
	instances.emplace_back(ptr);

	return ptr;
}

/**
 * Reload all tracked strings loaded using Load().
 */
void ResStr::ReloadAll()
{
	SDL_Log("Reloading all managed resource strings.");

	for (auto iter = instances.begin(); iter != instances.end(); ) {
		// If the instance is still alive, reload it.
		// Otherwise, remove it from the list.
		if (auto ptr = iter->lock()) {
			ptr->Reload();
			++iter;
		}
		else {
			iter = instances.erase(iter);
		}
	}
}

/**
 * Reload the string from the file it came from.
 */
void ResStr::Reload()
{
	pages.clear();

	// Fast byte-for-byte string read.
	// Adapted from:
	// http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in) {
		in.seekg(0, std::ios::end);
		s.resize(static_cast<std::string::size_type>(in.tellg()));
		in.seekg(0, std::ios::beg);
		in.read(&s[0], s.size());
		in.close();
	}
	else {
		throw Exception("Failed to read: " + filename);
	}

	// Split the string into separate pages.
	for (size_t pos = 0; ; ) {
		size_t endpos = s.find("----\n", pos);
		if (endpos == std::string::npos) {
			pages.emplace_back(s.substr(pos, s.size() - pos));
			break;
		}
		pages.emplace_back(s.substr(pos, endpos - pos));
		pos = endpos + 5;
	}

	// Notify listener.
	if (onReload) onReload();
}

}  // namespace AISDL

