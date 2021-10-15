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


/// @brief DynObj class definition
/// @file DynObj.h
/// @author Giovanni Paolo Vigano'

#pragma once

#ifndef DYNOBJ_H
#define DYNOBJ_H

#include <BoxObj.h>
#include <cmath> //sqrt

/// @addtogroup ArkanoidRemakeGame
/// @{

/// @brief Dynamic object with velocity control.
/// @details The DynObj class implements the specific behaviour of a specialized
/// box-shaped object, thus it is derived from the BoxObj class.
class DynObj : public BoxObj
{
public:
  /// Default constructor
  DynObj();

  /// Copy contructor
  DynObj(const DynObj& obj);

  /// Set the velocity along the axes
  void SetVel(double x, double y)
  {
    mVel.X=x;
    mVel.Y=y;
    mSpeedChanged = true;
  }

  /// Set the velocity along the X axis
  void SetVelX(double x)
  {
    mVel.X=x;
    mSpeedChanged = true;
  }

  /// Set the velocity along the Y axis
  void SetVelY(double y)
  {
    mVel.Y=y;
    mSpeedChanged = true;
  }

  /// Get the velocity vector
  const VecXy& Vel() const
  {
    if (mSpeedChanged) UpdateSpeed();
    return mVel;
  }

  /// Get the velocity component along the X axis
  const double VelX() const { return mVel.X; }

  /// Get the velocity component along the Y axis
  const double VelY() const { return mVel.Y; }
  
  /// Invert the velocity component along the X axis
  void InvertVelX() { mVel.X = -mVel.X; }

  /// Invert the velocity component along the Y axis
  void InvertVelY() { mVel.Y = -mVel.Y; }

  /// Get the speed (recalculate it if needed)
  const double Speed() const
  {
    if (mSpeedChanged) UpdateSpeed();
    return mSpeed;
  }

  /// Get the square speed (recalculate it if needed)
  const double SquareSpeed() const
  {
    if (mSpeedChanged) UpdateSpeed();
    return mSquareSpeed;
  }

  /// Get the previuos position before Move() was called
  ///@{
  const VecXy& PrevPos() const { return mPrevPos; }
  VecXy& PrevPos() { return mPrevPos; }
  ///@}

  /// Move according to the velocity
  void Move( double delta_time );
  
  /// Stop: reset the velocity
  void Stop() { SetVel(0.0,0.0); }

protected:

  VecXy mVel;
  VecXy mPrevPos;

  /// Update the scala speed values based on the velocity
  void UpdateSpeed() const
  {
    mSquareSpeed = mVel.X*mVel.X + mVel.Y*mVel.Y;
    mSpeed = sqrt(mSquareSpeed);
  }

private:

  // mutable attributes do not affect the state of this object,
  // they are just computed on demand based on that state

  mutable double mSquareSpeed;
  mutable double mSpeed;
  mutable bool mSpeedChanged;
};

/// @}

#endif//DYNOBJ_H

