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


/// @brief Block class definition
/// @file Block.h
/// @author Giovanni Paolo Vigano'

#pragma once

#ifndef BLOCK_H
#define BLOCK_H

#include <BoxObj.h>
#include <string>

/// @addtogroup ArkanoidRemakeGame
/// @{

/// @brief Wall block.
/// @details The Block class implements the specific behaviour of a specialized
/// box-shaped object, thus it is derived from the BoxObj class.
class Block : public BoxObj
{
  int mState;
  int mType;
  int mScore;
  std::string mBonusType;

public:
  /// Constructor: initialize data members
  Block();

  /// Check if the block was well defined
  bool Valid() const { return mType>=0; }

  /// Check if the block is still visible
  bool Visible() const { return mState>=0; }

  /// Hide (destroy only visually) the block
  void Hide() { mState=-1; }

  /// Hit the block, return the related score
  int Hit();

  /// Get the current block state (decreased when hit, 0=destroyed)
  int State() const { return mState; }

  /// Get the score bonus related to this block
  const std::string& Bonus() const { return mBonusType; }

  /// Set the attributes of this block
  /// @param typ the type of this block (identifier of the block type)
  /// @param state initial state (decreased when hit, -1 if destroyed)
  /// @param score score when hit
  /// @param bonus bouns dropped when hit
  void Set(int typ, int state=0, int score=1, const std::string& bonus = "");
  
  /// Get the type of this block (identifier of the block type)
  int Type() const { return mType; }
};


///@}

#endif//BLOCK_H

