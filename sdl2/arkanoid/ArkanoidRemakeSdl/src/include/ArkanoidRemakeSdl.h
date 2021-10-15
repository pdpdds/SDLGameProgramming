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
/// @brief ArkanoidRemakeSDL class definition
/// @file ArkanoidRemakeSDL.h
/// @author Giovanni Paolo Vigano'

#pragma once
#ifndef ARKANOID_REMAKE_SDL_H
#define ARKANOID_REMAKE_SDL_H


#include <ArkanoidRemake.h>
#include <sdltk.h>
#include <map>

/// @addtogroup ArkanoidRemakeGameSdl
/// @{

/// @brief Implementation of ArkanoidRemake using SDL library.
/// @details The ArkanoidRemakeSDL class directly derives from ArkanoidRemake
/// and provides an SDL implementation of that abstract class.
class ArkanoidRemakeSdl : public ArkanoidRemake
{
protected:

  /// @name Drawing surface identifiers
  ///@{
  std::string mShipSurf;
  std::string mBallSurf;
  std::string mPlayAreaSurf;
  std::string mBkgndSurf;
  std::string mSnowfallSurf;
  ///@}

  /// Value controlling the background scroll
  double mBkgndScroll;

  /// @name Timing
  ///@{
  sdltk::Timer mTimer;
  ///@}

  /// Identifier of the container for blocks images
  std::string mBlockMatrixName;
  
  /// SDL application manager, implementing event management, sound and graphics
  sdltk::SdlApp mSdlApp;

  /// Color used for background if an image is not available
  unsigned mBkgndColor;

  /// Methods implemented with SDL framework
  /// @name Framework-specific implementation
  //@{

  /// Get the current time in seconds (definition of the base class abstract method)
  double GetTime();

  /// Handle input events
  virtual void HandleEvents();

  /// Render a frame
  virtual void Frame();
  //@}

  /// Game level management redefined to implement graphics
  /// @name Game events management
  ///@{
  void AddLevel();

  /// Set up the graphics for the current level
  bool SetCurrentLevel(unsigned int n);

  /// Set up the graphics for the next level
  void GoToNextLevel();

  ///@}

  /// Initialize SDL with SDL toolkit
  bool InitSdl();

  /// Load images, sounds, etc. with SDL toolkit
  bool LoadFiles();

  /// Use the given image file to fill the background
  bool SetBackground(const std::string& file_name);

  /// Game objects rendered with graphics
  /// @name Drawing game objects
  ///@{
  void DrawBall();
  void DrawShip();
  void DrawBonus();
  void DrawText();
  void DrawBlocks();
  void DrawBackground();
  void DrawPlayArea();
  ///@}

  /// Game events management redefined to implement sound and graphics
  /// @name Game events management
  ///@{

  void HitBlock(Block& block, int x, int y);
  void HitShip();
  void GetBonus(const Bonus& bonus);
  bool DropBonus(const std::string& name, const VecXy& pos);
  void BonusLost(const Bonus& bonus);
  void BallLost();
  
  void AddScore(int n);

  /// Update the GUI text
  void UpdateScoreText();

  ///@}

  /// Play the named sound
  void PlaySound(const std::string& name);


  /// Functor class for application events
  class AppEventFunctor : public sdltk::EventFunctor
  {
  protected:
    /// Internal reference to the application
    ArkanoidRemakeSdl* mApp;
  public:
    /// Constructor: it requires a reference to the owner application
    AppEventFunctor(ArkanoidRemakeSdl* app);

    /// Functor operator
    virtual bool operator ()(const sdltk::EventData& evt_data);
  };

  /// Let the functor get the full control of the application
  friend class AppEventFunctor;

  /// @brief Game configuration data structure.
  /// It is loaded on startup and saved to disk on exit
  struct Configuration
  {
    bool FullScreen;
    int WindowWidth;
    int WindowHeight;
    bool Audio;
    int MusicVolume;
    int SoundsVolume;
  };
  
  /// Default configuration used in case the configuration fila is not readable
  static Configuration smDefaultConfig;
  
  /// Configuration loaded from the file (see smDefaultConfig)
  Configuration mConfig;

  /// Load the game configuration
  bool LoadConfiguration(const std::string& file_path);

  /// Save the game configuration
  bool SaveConfiguration(const std::string& file_path);

public:

  /// Constructor: initialize members values
  ArkanoidRemakeSdl();

  /// App interface implementation
  /// @name App interface
  ///@{
  bool Init();
  void Start();
  void Stop();
  void CleanUp();

  void Update();
  ///@}
};

///@}

#endif//ARKANOID_REMAKE_SDL_H
