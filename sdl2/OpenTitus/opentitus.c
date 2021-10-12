/*   
 * Copyright (C) 2008 - 2011 The OpenTitus team
 *
 * Authors:
 * Eirik Stople
 *
 * "Titus the Fox: To Marrakech and Back" (1992) and
 * "Lagaf': Les Aventures de Moktar - Vol 1: La Zoubida" (1991)
 * was developed by, and is probably copyrighted by Titus Software,
 * which, according to Wikipedia, stopped buisness in 2005.
 *
 * OpenTitus is not affiliated with Titus Software.
 *
 * OpenTitus is  free software; you can redistribute  it and/or modify
 * it under the  terms of the GNU General  Public License as published
 * by the Free  Software Foundation; either version 3  of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the GNU
 * General Public License for more details.
 */

/* opentitus.c
 * Main source file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
//#include "opentitus.h"

//Probably not the best way, but it works...
#define HAVE_CONFIG_H 1

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef AUDIO_ENABLED
#include "audio.h"
#endif

/*
#ifdef AUDIO_MIKMOD_SINGLETHREAD
#include <mikmod.h>
#endif

#ifdef AUDIO_SDL_MIXER
#include "SDL/SDL_mixer.h"
#endif
*/

#include "tituserror.h"
#include "sqz.h"
#include "settings.h"
#include "malloc.h"
#include "sprites.h"
#include "backbuffer.h"
#include "viewimage.h"
#include "fonts.h"
#include "menu.h"
#include "engine.h"
#include "original.h"
#include "objects.h"

extern int init();

int main(int argc, char *argv[]) {

    int retval;
    int state = 1; //View the menu when the main loop starts
    retval = init();
    if (retval < 0)
        state = 0;

    if (state) {
        retval = viewintrotext();
        if (retval < 0)
            state = 0;
    }

    if (state) {
        retval = viewimage(tituslogofile, tituslogoformat, 0, 4000);
        if (retval < 0)
            state = 0;
    }

#ifdef AUDIO_ENABLED
    SELECT_MUSIC(15);
#endif

    if (state) {
        retval = viewimage(titusintrofile, titusintroformat, 0, 6500);
        if (retval < 0)
            state = 0;
    }

    while (state) {
        retval = viewmenu(titusmenufile, titusmenuformat);

        if (retval <= 0)
            state = 0;

        if (state && (retval <= levelcount)) {
            retval = playtitus(retval - 1);
            if (retval < 0)
                state = 0;
        }
    }
    
    freefonts();

#ifdef AUDIO_ENABLED
    freeaudio();
#endif

    SDL_Quit();

    checkerror();

    if (retval == -1)
        retval = 0;

    return retval;
}

SDL_Surface* iconsurface;     // Declare an SDL_Surface to be filled in with pixel data from an image file
Uint16 pixels[16 * 16] = {  // ...or with raw pixel data:
  0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
  0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
  0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
  0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
  0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
  0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
  0x0fff, 0x0aab, 0x0789, 0x0bcc, 0x0eee, 0x09aa, 0x099a, 0x0ddd,
  0x0fff, 0x0eee, 0x0899, 0x0fff, 0x0fff, 0x1fff, 0x0dde, 0x0dee,
  0x0fff, 0xabbc, 0xf779, 0x8cdd, 0x3fff, 0x9bbc, 0xaaab, 0x6fff,
  0x0fff, 0x3fff, 0xbaab, 0x0fff, 0x0fff, 0x6689, 0x6fff, 0x0dee,
  0xe678, 0xf134, 0x8abb, 0xf235, 0xf678, 0xf013, 0xf568, 0xf001,
  0xd889, 0x7abc, 0xf001, 0x0fff, 0x0fff, 0x0bcc, 0x9124, 0x5fff,
  0xf124, 0xf356, 0x3eee, 0x0fff, 0x7bbc, 0xf124, 0x0789, 0x2fff,
  0xf002, 0xd789, 0xf024, 0x0fff, 0x0fff, 0x0002, 0x0134, 0xd79a,
  0x1fff, 0xf023, 0xf000, 0xf124, 0xc99a, 0xf024, 0x0567, 0x0fff,
  0xf002, 0xe678, 0xf013, 0x0fff, 0x0ddd, 0x0fff, 0x0fff, 0xb689,
  0x8abb, 0x0fff, 0x0fff, 0xf001, 0xf235, 0xf013, 0x0fff, 0xd789,
  0xf002, 0x9899, 0xf001, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0xe789,
  0xf023, 0xf000, 0xf001, 0xe456, 0x8bcc, 0xf013, 0xf002, 0xf012,
  0x1767, 0x5aaa, 0xf013, 0xf001, 0xf000, 0x0fff, 0x7fff, 0xf124,
  0x0fff, 0x089a, 0x0578, 0x0fff, 0x089a, 0x0013, 0x0245, 0x0eff,
  0x0223, 0x0dde, 0x0135, 0x0789, 0x0ddd, 0xbbbc, 0xf346, 0x0467,
  0x0fff, 0x4eee, 0x3ddd, 0x0edd, 0x0dee, 0x0fff, 0x0fff, 0x0dee,
  0x0def, 0x08ab, 0x0fff, 0x7fff, 0xfabc, 0xf356, 0x0457, 0x0467,
  0x0fff, 0x0bcd, 0x4bde, 0x9bcc, 0x8dee, 0x8eff, 0x8fff, 0x9fff,
  0xadee, 0xeccd, 0xf689, 0xc357, 0x2356, 0x0356, 0x0467, 0x0467,
  0x0fff, 0x0ccd, 0x0bdd, 0x0cdd, 0x0aaa, 0x2234, 0x4135, 0x4346,
  0x5356, 0x2246, 0x0346, 0x0356, 0x0467, 0x0356, 0x0467, 0x0467,
  0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
  0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
  0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
  0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff
};


int init() {

    int retval;

    retval = readconfig(OPENTITUS_CONFIG_FILE);
    if (retval < 0)
        return retval;

#ifdef AUDIO_ENABLED
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0) {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        return TITUS_ERROR_SDL_ERROR;
    }
#else
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        return TITUS_ERROR_SDL_ERROR;
    }
#endif

#ifdef _DINGUX
    //fullscreen
    SDL_ShowCursor(SDL_DISABLE);
    screen = SDL_SetVideoMode(reswidth, resheight, bitdepth, SDL_SWSURFACE);
#else
    switch (videomode) 
    {
	case 0: //window mode
	{
		//screen = SDL_SetVideoMode(reswidth, resheight, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);

        pWindow = SDL_CreateWindow(OPENTITUS_WINDOW_TEXT,
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            640, 480,
            0);

        pRenderer = SDL_CreateRenderer(pWindow, -1, 0);
		
		SDL_RenderSetLogicalSize(pRenderer, reswidth, resheight);
#ifdef _WIN32
        //SDL_WM_SetCaption(OPENTITUS_WINDOW_TEXT, 0);
#endif
		// if all this hex scares you, check out SDL_PixelFormatEnumToMasks()!
		screen = SDL_CreateRGBSurface(SDL_SWSURFACE, reswidth, resheight, 32,
			0,
			0,
			0,
			0);

		if (screen == 0)
		{
			return TITUS_ERROR_SDL_ERROR;
		}

		texture = SDL_CreateTexture(pRenderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STREAMING,
			reswidth, resheight);

		if (texture == 0)
		{
			return TITUS_ERROR_SDL_ERROR;
		}

#ifdef _WIN32
        iconsurface = SDL_CreateRGBSurfaceFrom(pixels, 16, 16, 16, 16 * 2, 0x0f00, 0x00f0, 0x000f, 0xf000);

        // The icon is attached to the window pointer
        SDL_SetWindowIcon(pWindow, iconsurface);

        // ...and the surface containing the icon pixel data is no longer required.
        SDL_FreeSurface(iconsurface);
#endif
	}
        break;
    case 1: //fullscreen
        SDL_ShowCursor(SDL_DISABLE);
        //screen = SDL_SetVideoMode(reswidth, resheight, bitdepth, SDL_DOUBLEBUF | SDL_FULLSCREEN);
		if (SDL_CreateWindowAndRenderer(reswidth, resheight, 0, &pWindow, &pRenderer) < 0)
		{
			printf("SDL_CreateWindowAndRenderer Error\n");
			return TITUS_ERROR_SDL_ERROR;
		}
        break;
    }
#endif

    if (screen == NULL) {
        printf("Unable to set video mode: %s\n", SDL_GetError());
        return TITUS_ERROR_SDL_ERROR;
    }

    

    //SDL_EnableUNICODE (1);

/*
#ifdef AUDIO_MIKMOD_SINGLETHREAD
    MikMod_RegisterAllDrivers();
    MikMod_RegisterAllLoaders();
    md_mode = AUDIO_MIKMOD_MODE;
    if (MikMod_Init("")) {
        printf("Could not initialize sound, reason: %s\n", MikMod_strerror(MikMod_errno));
        return TITUS_ERROR_AUDIO_ERROR;
    }
#endif

#ifdef AUDIO_MIKMOD_MULTITHREAD
    retval = startmodulethread();
    if (retval < 0)
        return retval;
#endif

#ifdef AUDIO_SDL_MIXER
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024)==-1) {
        printf("Mix_OpenAudio: %s\n", Mix_GetError());
        return TITUS_ERROR_AUDIO_ERROR;
    }
#endif
*/

	initaudio();

    initoriginal();

    initcodes();

    initleveltitles();

    loadfonts();

    return 0;

}

void checkerror(void) {
    printf("%s\n", lasterror);
}

