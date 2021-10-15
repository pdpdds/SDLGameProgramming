/*-------------------------------------------------------*/
/*                                                       */
/* Some STUFF nobody knows where to put.                 */
/* This STUFF is [c]copyright by AlphaHelix.             */
/* In the year of 1996 (actually in May).                */
/*                                                       */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL_BASE.HPP"
#include "SOUND.HPP"
#include "RAVAGE.HPP"


#define FADESPEED       2
#define FADETIME			3000 //ms


/*---------------------------------------------------------
 Function:

 Description:
 Fadeout screen and\or sound.
 No game update will be done during fadeout.
---------------------------------------------------------*/
void fadeout(int mode)
{
   int   i;
   int   mod_vol;
	const int alphaStep = 4;

	SDLTexture *originalTexture;

	originalTexture = render.createScreenTexture();

// Setup palette fading if required.
   if(mode & FADE_PALETTE) {
   }
// Get current music volume,
   mod_vol = s_getmodvol() - 1;
   if (mod_vol <= 0) mode &= ~FADE_MUSIC;

// Do fading.
   for (i = 64; i >= 0; i-=FADESPEED) {

		render.fade(originalTexture, (64-i) * alphaStep);

		render.update();

// Do sound fading.
      if (mode & FADE_MUSIC) s_setmodvol((mod_vol*i)/64 + 1);
		SDL_Delay(FADETIME/64);
   }

// Completly stop playing mode and restore old music volume.
   if (mode & FADE_MUSIC) {
      s_stopmod();
      s_setmodvol(mod_vol + 1);
   }

	originalTexture->destroy();
	delete originalTexture;
}

void fadein(void *p)
{
	int   i;
	const int alphaStep = 4;

	SDLTexture *originalTexture = NULL;

	originalTexture = render.createScreenTexture();

// Do fading.
   for (i = 64; i >= 0; i-=FADESPEED) {

		render.fade(originalTexture, i * alphaStep);

		render.update();

		SDL_Delay(FADETIME/64);
   }

	originalTexture->destroy();
	delete originalTexture;

	if(p) palette.set(p);
}


//--------------------------------------------------------
// Beautiful write.
//--------------------------------------------------------

void beauty(Font *font, int &x, int &y, char *text)
{
   static char     word[81];
   int    color;
   int    tmp;

   while (*text) {
      sscanf(text, "%80s", word);
      if (font->textlen(word)+x > XMAX-10) {
         if (*text == ' ') text++;
         x = 0; y += font->height() + 1;
      }
      switch (*text) {
      case '^':
         switch (text[1]) {
         case 'c':
            color = (text[2]-'0')*100 + (text[3]-'0')*10 + (text[4]-'0');
            text += 5;
            break;
         case 's':
            tmp = (text[2]-'0')*10 + (text[3]-'0');
            x += tmp * (font->charlen(' ') + 1);
            text += 4;
            break;
         case 'e':
            return;
         }
         break;
      case 0x0a:
      case 0x0d:
         x = 0; y += font->height() + font->height()/2;
         text += 2;
         break;
      default:
         font->vanilla_char(x, y, *text, color);
         x += font->charlen(*text);
         text++;
      }
   }
}


