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


/// @brief Ship class definition
/// @file Ship.h
/// @author Giovanni Paolo Vigano'

#pragma once

#ifndef SHIP_H
#define SHIP_H

// we don't include DynObj.h because it is already included by Ball.h

#include <Ball.h>

/// @addtogroup ArkanoidRemakeGame
/// @{

/// @brief Game ship object definition.
/// @details The Ship class implements the specific behaviour of a specialized
/// dynamic object, thus it is derived from the DynObj class.
class Ship : public DynObj
{
public:
  Ship();

  /// Drag a ball (when held by this ship)
  void DragBall(Ball& ball);

  /// Launch a ball (when held by this ship)
  void LaunchBall(Ball& ball);
};

///@}

#endif//SHIP_H

