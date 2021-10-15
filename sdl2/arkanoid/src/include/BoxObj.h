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


/// @brief BoxObj class definition
/// @file BoxObj.h
/// @author Giovanni Paolo Vigano'

#pragma once

#ifndef BOXOBJ_H
#define BOXOBJ_H

#include <VecXy.h>

/// @addtogroup ArkanoidRemakeGame
/// @{

/// @brief A box-shaped object.
/// @details A box-shaped object with position and size, using VecXy struct to store
/// 2-dimensional data.
class BoxObj
{
public:
  /// Default constructor with optional intialization list
  BoxObj(double x=0.0, double y=0.0)
    : mPos(x,y)
  {
  }

  // Copy constructor
  BoxObj(const BoxObj& obj)
    : mPos(obj.Pos()), mSize(obj.Size()), mOrigin(obj.Origin())
  {
  }

  /// Position
  VecXy& Pos() { return mPos; }

  /// Position (read only)
  const VecXy& Pos() const { return mPos; }

  /// Origin (min x,y corner)
  VecXy& Origin() { return mOrigin; }

  /// Origin (min x,y corner, read-only)
  const VecXy& Origin() const { return mOrigin; }

  /// Size
  VecXy& Size() { return mSize; }

  /// Size (read only)
  const VecXy& Size() const { return mSize; }


  //VecXy Centre() const { return mPos+mOrigin+mSize/2.0; }

  /// Lower X value
  const double MinX() const { return mPos.X+mOrigin.X; }

  /// Lower Y value
  const double MinY() const { return mPos.Y+mOrigin.Y; }

  /// Upper X value
  const double MaxX() const { return MinX()+mSize.X; }

  /// Upper X value
  const double MaxY() const { return MinY()+mSize.Y; }

  /// Half size
  const VecXy HSize() const { return mSize/2.0; }

  /// Half size along X axis
  const double HSizeX() const { return mSize.X/2.0; }

  /// Half size along Y axis
  const double HSizeY() const { return mSize.Y/2.0; }


protected:

  VecXy mPos;
  VecXy mSize;
  VecXy mOrigin;
};

///@}

#endif//BOXOBJ_H

