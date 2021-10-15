
/* Exception.h
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

#include <exception>

namespace AISDL {

/**
 * Basic exception.
 * @author Michael Imamura
 */
class Exception : public std::exception {
	typedef std::exception SUPER;
public:
	Exception() : SUPER() { }
	Exception(const std::string &msg) : SUPER(), msg(msg) { }
	Exception(const char *msg) : SUPER(), msg(msg) { }
	Exception(const std::string &msg, const std::string &cause) :
		SUPER(), msg(msg + ": " + cause) { }
	virtual ~Exception() throw() { }

	virtual const char* what() const throw() { return msg.c_str(); }

protected:
	std::string &GetMessage() { return msg; }

private:
	std::string msg;
};

}  // namespace AISDL

