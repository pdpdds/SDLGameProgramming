/*
ArkanoidRemakeSDL
a sample of object-oriented game programming with C++. 
version 1.0, May 2016

Copyright (c) 2016 Giovanni Paolo Vigano'

The source code of the SDL library used by ArkanoidRemakeSDL can be found here:
https://www.libsdl.org/
---
ArkanoidRemakeSDL source code is licensed with the same zlib license:
(http://www.gzip.org/zlib/zlib_license.html)

This software is provided 'as-is', without any express or implied warranty. In 
no event will the authors be held liable for any damages arising from the use of
this software.

Permission is granted to anyone to use this software for any purpose, including 
commercial applications, and to alter it and redistribute it freely, subject to 
the following restrictions:

    1. The origin of this software must not be misrepresented; you must not 
    claim that you wrote the original software. If you use this software in a 
    product, an acknowledgment in the product documentation would be appreciated
    but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
*/


/// @brief Bonus class definition
/// @file Bonus.h
/// @author Giovanni Paolo Vigano'

#pragma once

#ifndef BONUS_H
#define BONUS_H

#include <DynObj.h>
#include <string>

/// @addtogroup ArkanoidRemakeGame
/// @{

/// @brief Bonus item, dropped when a wall block is hit.
/// @details The Bonus class implements the specific behaviour of a specialized
/// dynamic object, thus it is derived from the DynObj class.
class Bonus : public DynObj
{
  std::string mTypeName;
  int mScore;
public:
  /// Default constructor
  Bonus();

  /// Construcxtor with initialization list
  Bonus(const std::string& type, const VecXy& vel, int score);
  
  /// Copy constructor
  Bonus(const Bonus& bonus);

  /// Get the bonus type
  const std::string& Type() const { return mTypeName; }

  /// Get the bonus score
  const int Score() const { return mScore; }
};


///@}

#endif//BONUS_H

