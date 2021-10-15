/*
========================================================================

                           D O O M  R e t r o
         The classic, refined DOOM source port. For Windows PC.

========================================================================

  Copyright © 1993-2012 id Software LLC, a ZeniMax Media company.
  Copyright © 2013-2018 Brad Harding.

  DOOM Retro is a fork of Chocolate DOOM. For a list of credits, see
  <https://github.com/bradharding/doomretro/wiki/CREDITS>.

  This file is part of DOOM Retro.

  DOOM Retro is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation, either version 3 of the License, or (at your
  option) any later version.

  DOOM Retro is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with DOOM Retro. If not, see <https://www.gnu.org/licenses/>.

  DOOM is a registered trademark of id Software LLC, a ZeniMax Media
  company, in the US and/or other countries and is used without
  permission. All other trademarks are the property of their respective
  holders. DOOM Retro is in no way affiliated with nor endorsed by
  id Software.

========================================================================
*/

#if !defined(__HU_LIB_H__)
#define __HU_LIB_H__

// We are referring to patches.
#include "r_defs.h"

// font stuff
#define HU_MAXLINES         4
#define HU_MAXLINELENGTH    512

#define HU_ALTHUDMSGX       30
#define HU_ALTHUDMSGY       12

//
// Typedefs of widgets
//

// Text Line widget
//  (parent of Scrolling Text and Input Text widgets)
typedef struct
{
    // left-justified position of scrolling text window
    int             x, y;

    patch_t         **f;                            // font
    int             sc;                             // start character
    char            l[HU_MAXLINELENGTH + 1];        // line of text
    int             len;                            // current line length

    // whether this line needs to be updated
    int             needsupdate;
} hu_textline_t;

// Scrolling Text window widget
//  (child of Text Line widget)
typedef struct
{
    hu_textline_t   l[HU_MAXLINES];                 // text lines to draw
    int             h;                              // height in lines
    int             cl;                             // current line number

    // pointer to dboolean stating whether to update window
    dboolean        *on;
    dboolean        laston;                         // last value of *->on.
} hu_stext_t;

//
// Widget creation, access, and update routines
//

//
// textline code
//

void HUlib_initTextLine(hu_textline_t *t, int x, int y, patch_t **f, int sc);

// returns success
dboolean HUlib_addCharToTextLine(hu_textline_t *t, char ch);

// draws text line
void HUlib_drawTextLine(hu_textline_t *l, dboolean external);

// erases text line
void HUlib_eraseTextLine(hu_textline_t *l);

//
// Scrolling Text window widget routines
//

void HUlib_initSText(hu_stext_t *s, int x, int y, int h, patch_t **font, int startchar, dboolean *on);

void HUlib_addMessageToSText(hu_stext_t *s, char *prefix, char *msg);

// draws stext
void HUlib_drawSText(hu_stext_t *s, dboolean external);

// erases all stext lines
void HUlib_eraseSText(hu_stext_t *s);

extern dboolean STCFN034;
extern dboolean idbehold;
extern dboolean s_STSTR_BEHOLD2;
extern byte     *tempscreen;
extern byte     *tinttab50;

#endif
