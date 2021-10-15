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


/// @brief Ball class definition
/// @file Ball.h
/// @author Giovanni Paolo Vigano'

#pragma once

#ifndef BALL_H
#define BALL_H

#include <DynObj.h>

/// @addtogroup ArkanoidRemakeGame
/// @{

/// @brief Game ball definition.
/// @details The Ball class implements the specific behaviour of a specialized
/// dynamic object, thus it is derived from the DynObj class.
class Ball : public DynObj
{
protected:
  /// Default speed, used by the ship to launch the ball
  float mDefaultSpeed;
  /// Flag that save the condition of the ball as "held"
  bool mHeld;
public:

  /// Constructor
  Ball(float default_speed = 0.0f)
    : mDefaultSpeed(default_speed)
  {
  }

  /// Get the default speed
  float DefaultSpeed() { return mDefaultSpeed; }

  /// Set a default speed
  void SetDefaultSpeed(float default_speed) { mDefaultSpeed=default_speed; }
  
  /// Mark this ball as held
  void Hold() { mHeld=true; }

  /// Mark this ball as not held
  void Release() { mHeld=false; }

  /// Check if this ball was marked as held
  bool Held() { return mHeld; }
};

///@}

#endif//BALL_H

