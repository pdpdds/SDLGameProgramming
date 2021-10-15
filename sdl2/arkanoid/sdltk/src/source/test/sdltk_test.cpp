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


#include <cstdlib>
#include <iostream>

#include <sdltk.h>


int main(int argc, char* argv[])
{
  std::cout << "Creating the application..." << std::endl;
  sdltk::SdlApp app;

  std::cout << "Initializing SDL..." << std::endl;
  app.InitSdl();

  std::cout << "Creating the application window..." << std::endl;
  if (!app.CreateWindow("SDLTK test", false, 640, 480))
  {
    return EXIT_FAILURE;
  }
  std::cout << "You shuold see an empty black window..." << std::endl;
  app.UpdateWindow();

  // multiple applications not supported for now
  ////app2.UpdateWindow();
  ////sdltk::Delay(2000);
  ////app.Exit();
  ////sdltk::Delay(2000);
  ////app2.Exit();

  app.Delay(2000);
  std::cout << "Deinitializing the application..." << std::endl;
  app.ExitSdl();
  std::cout << "Press ENTER to exit." << std::endl;
  std::getchar();

  return EXIT_SUCCESS;
}

