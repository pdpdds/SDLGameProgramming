
/* ResStr.h
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

/**
 * A file-backed string that can be reloaded at runtime.
 * @author Michael Imamura
 */
class ResStr {
	friend const std::string &operator*(const ResStr &str);
public:
	ResStr(const std::string &filename);

public:
	static std::shared_ptr<ResStr> Load(const std::string &filename);
	static void ReloadAll();

public:
	/// Set a listener to be notified when the resource is reloaded.
	template<typename Fn>
	void SetOnReload(Fn fn) { onReload = fn; }

	void Reload();

public:
	size_t GetNumPages() const { return pages.size(); }
	const std::string &operator[](size_t i) const { return pages[i]; }

private:
	static std::list<std::weak_ptr<ResStr>> instances;
	std::string filename;
	std::string s;
	std::function<void()> onReload;
public:
	typedef std::vector<std::string> pages_t;
	pages_t pages;
};

inline const std::string &operator*(const ResStr &str)
{
	return str.s;
}

}  // namespace AISDL

