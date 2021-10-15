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


/// @brief 2-dimensional vector
/// @file VecXy.h
/// @author Giovanni Paolo Vigano'

#pragma once

#ifndef VECXY_H
#define VECXY_H

/// @addtogroup ArkanoidRemakeGame
/// @{

/// @brief Vector with two (double precision) coordinate values.
/// @details This data structure is an example of a C++ hybrid implementation.
/// The struct defines only the data members and a default constructor,
/// with an optional initialization list, but the implementation of the
/// operations on the struct is demanded to the overridden operators.
/// While a pure object-oriented approach requires the implementation of
/// the operations defining methods inside a class, in C++ this hybrid
/// solution allows to extend classes and structures that were not designed
/// to be extended (e.g. primitive types and classes in the standard library).
/// In the case of the VecXy struct we have at least two operations that
/// cannot be implemented as class members: the multiplication and the division
/// of a number with a VecXy object.
/// @see ArkanoidRemakeGame, section Operations
struct VecXy
{
  double X; ///< Horizontal coordinate
  double Y; ///< Vertical coordinate
  
  /// Constructor
  VecXy(double vx=0.0, double vy=0.0)
    : X(vx), Y(vy)
  {
  }
};

/// @name Operations
/// @{
/// Due to their simplicity these operations are declared inline.
inline VecXy& operator +=(VecXy& v1,const VecXy& v2)
{
  v1.X += v2.X;
  v1.Y += v2.Y;
  return v1;
}

inline VecXy& operator -=(VecXy& v1,const VecXy& v2)
{
  v1.X += v2.X;
  v1.Y += v2.Y;
  return v1;
}

inline VecXy operator +(const VecXy& v1,const VecXy& v2)
{
  return VecXy(v1.X + v2.X, v1.Y + v2.Y);
}

inline VecXy operator -(const VecXy& v1,const VecXy& v2)
{
  return VecXy(v1.X - v2.X, v1.Y - v2.Y);
}

inline VecXy operator *(const VecXy& v1,const VecXy& v2)
{
  return VecXy(v1.X * v2.X, v1.Y * v2.Y);
}

inline VecXy operator /(const VecXy& v1,const VecXy& v2)
{
  return VecXy(v1.X / v2.X, v1.Y / v2.Y);
}

inline VecXy operator *(const VecXy& v1,const double& v2)
{
  return VecXy(v1.X * v2, v1.Y * v2);
}

inline VecXy operator /(const VecXy& v1,const double& v2)
{
  return VecXy(v1.X / v2, v1.Y / v2);
}

inline VecXy operator *(const double& v2,const VecXy& v1)
{
  return VecXy(v1.X * v2, v1.Y * v2);
}

inline VecXy operator /(const double& v2,const VecXy& v1)
{
  return VecXy(v1.X / v2, v1.Y / v2);
}

inline VecXy operator -(const VecXy& v1)
{
  return VecXy(-v1.X, -v1.Y);
}

///@}

///@}


#endif//VECXY_H