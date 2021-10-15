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
#include <ArkanoidRemakeSdl.h>
#include <sdltk.h>

#include <iostream>

int main(int argc, char* args[])
{
  // Crea il gioco
  ArkanoidRemakeSdl arkanoid_remake;

  std::cout << "\nArkanoid Remake SDL\nby GPV (2015-2016)\n"
            << "\nHints:\n"
            << " press A to switch audio on/off\n"
            << " press ESC to exit\n"
            << std::endl;
  // Esegui il gioco
  if(!arkanoid_remake.Run())
  {
    std::cout << "\nPress the Enter to continue." << std::endl;
    std::cin.get();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
