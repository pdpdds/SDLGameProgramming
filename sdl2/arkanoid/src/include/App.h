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


/// @brief App interface definition
/// @file App.h
/// @author Giovanni Paolo Vigano'
/// @details Even if this definition seems to be almost useless within this 
/// project, in an well designed object-oriented software you must provide some
/// mechanisms to ease the extension and the maintenance of the source code.
/// In this view here we define a generic interface that can be accessed by
/// an external program, without the need to define the implementation, that 
/// can be changed and extended in future without affecting the interface 
/// definition.

#pragma once

#ifndef APP_H
#define APP_H

#include <string>

/// @addtogroup AppModule
/// @{

/// Application interface definition.
class App
{
protected:
  /// Flag used in the Run() method while calling Update() in a loop
  bool mQuit;
public:

  /// Constructor
  App();

  /// Initialization (the first method to call)
  virtual bool Init() { return false; }

  /// Start the application (the second method to call)
  virtual void Start() {}

  /// Update the application (call it in an iteration loop)
  virtual void Update() {}

  /// Stop the application
  virtual void Stop() {}

  /// Clean up the application and free memory
  virtual void CleanUp() {}

  /// @brief Run the application
  /// @details Call Init() and Start(), loop calling Update(), call Stop() and CleanUp() on exit
  /// @return Return false if Init() fails, else return true
  bool Run();
};

///@}


#endif//APP_H