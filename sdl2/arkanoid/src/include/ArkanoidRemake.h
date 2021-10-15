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


/// @brief ArkanoidRemake class definition
/// @file ArkanoidRemake.h
/// @author Giovanni Paolo Vigano'

#pragma once

#ifndef ARKANOID_REMAKE_H
#define ARKANOID_REMAKE_H

#include <cmath>
#include <vector>
#include <string>
#include <map>
#include <App.h>
#include <VecXy.h>
#include <BoxObj.h>
#include <DynObj.h>
#include <Ball.h>
#include <Ship.h>

#include <Bonus.h>
#include <Block.h>
#include <BlockMatrix.h>
#include <PlayArea.h>
#include <Level.h>

/// @addtogroup ArkanoidRemakeGame
/// @{





/**
@brief Base class for an implementation of a remake of the old game Arkanoid.
@details The ArkanoidRemake class implements the generic App interface,
basic functions and the logic of the game.
This class must be specialized to implement the visualization on a
specific platform.
This class in an abstract class, because there are some pure virtual
methods that must be defined by specific implementations (e.g. for
graphics, sound, input, ...). These pure virtual methods (see the section
Framework-specific implementation) represent the missing implementation
that must be added by more specialized classes (see ArkanoidRemakeSDL).
The virtual methods must be overridden, calling the base class version and
adding the specific implementation. Some of these methods are pure virtual methods
(see section "Framework-specific implementation").
@note The origin of the coordinates is assumed to be at the lower left corner of the screen.
@note The coordinates of the game objects (ship, ball, blocks) are relative to the play area.
@note \n Even if some libraries provide specific implementations for some features
that are needed by this game, like collision check, bouncing calculation, etc.
in this class a simplified implementation is provided. Anyway it could be
partially or entirely replaced by more specialized classes.
**/
class ArkanoidRemake : public App
{
protected:

  /// Play area definition
  PlayArea mPlayArea;

  /// Ball object
  Ball mBall;

  /// Ship object
  Ship mShip;

  /// Index of the active level
  unsigned int mCurrLevel;

  /// @name Timing
  ///@{
  double mPrevTime;
  double mCurrTime;
  ///@}

  /// Current blocks configuration
  BlockMatrix mBlock;

  /// List of game levels
  std::vector<Level> mLevel;

  /// List of dropped bunuses
  std::vector<Bonus> mBonus;

  /// Bonus definitions (templates for bonus creation)
  std::map<std::string,Bonus> mBonusDef;

  /// Score text buffer
  std::string mScoreText;

  /// Score reached within the current level
  unsigned int mLevelScore;

  /// Total game score
  unsigned int mTotalScore;

  /// Counter for the blocks not yet destroyed
  unsigned int mBlocksLeft;

  /// Level management
  /// @todo implement more game levels
  /// @note only one level is implemented for now
  ///@{

  /// Add a (the only one foer now) new level
  virtual void AddLevel();

  /// Set the current level
  virtual bool SetCurrentLevel(unsigned int n);

  /// Go to the next level
  virtual void GoToNextLevel();

  ///@}

  /// Add the givent points to the score
  virtual void AddScore(int n);

  /// Update the score text
  virtual void UpdateScoreText();

  /// Move the ball along with the ship (when it is attached)
  void MoveBallWithShip();

  /// Launch tha ball (if held by the ship)
  void LaunchBall();

  /// Calculate the ball position according to its velocity and the play area size
  bool CalculateBallPosition();

  /// Calculate the bonuses position according to their velocity and the play area size
  void CalculateBonusPosition();

  /// Calculate the ship position according to the play area size
  bool CalculateShipPosition(int x=-1);
  
  /// Check (and manage) the collision between the ball and a block
  bool CheckBallBlockCollision(const Block& block, int x, int y);

  /// Check (and manage) the collision between the ball and the walls
  bool CheckBallCollision(int x, int y);

public:

  /// Constructor: just initialize data members
  ArkanoidRemake();

  /// App interface implementation
  //@{

  /// Initialize game level(s) and items data
  virtual bool Init();

  /// Initialize the score and go to the first level
  virtual void Start();

  /// Update the application (call it in an iteration loop)
  virtual void Update();
  //@}

  /// Abstract methods to be implemented according to the chosen framework
  /// @name Framework-specific implementation
  //@{

  /// Get the current time in seconds
  virtual double GetTime()=0;

  /// Handle input events
  virtual void HandleEvents()=0;

  /// Render a frame
  virtual void Frame()=0;
  //@}
  
  /// Actions implementation
  //@{

  /// A block was hit
  virtual void HitBlock(Block& block, int x, int y);

  /// The ship hit the ball
  virtual void HitShip();

  /// The ship hit a bonus item
  virtual void GetBonus(const Bonus& bonus);

  /// A bonus item was dropped
  virtual bool DropBonus(const std::string& name, const VecXy& pos);

  /// The ship missed a bonus
  virtual void BonusLost(const Bonus& bonus);

  /// The ship missed the ball
  virtual void BallLost();

  ///@}
};

///@}

#endif//ARKANOID_REMAKE_H