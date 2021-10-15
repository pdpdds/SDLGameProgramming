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

#if !defined(__R_DRAW_H__)
#define __R_DRAW_H__

// [BH] Compensate for rounding errors in DOOM's renderer by stretching wall
//  columns by 1px. This eliminates the randomly-colored pixels ("sparkles")
//  that appear at the bottom of some columns.
#define SPARKLEFIX      64

#define NOTEXTURECOLOR  80

extern lighttable_t     *dc_colormap[2];
extern int              dc_x;
extern int              dc_yl;
extern int              dc_yh;
extern fixed_t          dc_iscale;
extern fixed_t          dc_texturemid;
extern fixed_t          dc_texheight;
extern fixed_t          dc_texturefrac;
extern byte             *dc_blood;
extern byte             *dc_brightmap;
extern int              dc_baseclip;
extern int              dc_floorclip;
extern int              dc_ceilingclip;

// first pixel in a column
extern byte             *dc_source;

extern byte             *tinttab;
extern byte             *tinttab25;
extern byte             *tinttab33;
extern byte             *tinttab40;
extern byte             *tinttab66;
extern byte             *tranmap;
extern byte             *tinttabred;
extern byte             *tinttabredwhite1;
extern byte             *tinttabredwhite2;
extern byte             *tinttabgreen;
extern byte             *tinttabblue;
extern byte             *tinttabred33;
extern byte             *tinttabredwhite50;
extern byte             *tinttabgreen33;
extern byte             *tinttabblue25;

// The span blitting interface.
// Hook in assembler or system specific BLT here.
void R_DrawColumn(void);
void R_DrawColorColumn(void);
void R_DrawWallColumn(void);
void R_DrawBrightMapWallColumn(void);
void R_DrawSkyColumn(void);
void R_DrawFlippedSkyColumn(void);
void R_DrawSkyColorColumn(void);
void R_DrawTranslucentColumn(void);
void R_DrawTranslucent50Column(void);
void R_DrawDitheredColumn(void);
void R_DrawTranslucent33Column(void);
void R_DrawTranslucentGreenColumn(void);
void R_DrawTranslucentRedColumn(void);
void R_DrawTranslucentRedWhiteColumn1(void);
void R_DrawTranslucentRedWhiteColumn2(void);
void R_DrawTranslucentRedWhite50Column(void);
void R_DrawTranslucentBlueColumn(void);
void R_DrawTranslucentGreen33Column(void);
void R_DrawTranslucentRed33Column(void);
void R_DrawTranslucentBlue25Column(void);
void R_DrawRedToBlueColumn(void);
void R_DrawTranslucentRedToBlue33Column(void);
void R_DrawRedToGreenColumn(void);
void R_DrawTranslucentRedToGreen33Column(void);
void R_DrawPlayerSpriteColumn(void);
void R_DrawSuperShotgunColumn(void);
void R_DrawTranslucentSuperShotgunColumn(void);
void R_DrawShadowColumn(void);
void R_DrawFuzzyShadowColumn(void);
void R_DrawSolidShadowColumn(void);
void R_DrawSolidFuzzyShadowColumn(void);
void R_DrawBloodSplatColumn(void);
void R_DrawSolidBloodSplatColumn(void);
void R_DrawMegaSphereColumn(void);
void R_DrawSolidMegaSphereColumn(void);

// The Spectre/Invisibility effect.
void R_DrawFuzzColumn(void);
void R_DrawPausedFuzzColumn(void);
void R_DrawFuzzColumns(void);
void R_DrawPausedFuzzColumns(void);

// Draw with color translation tables,
//  for player sprite rendering,
//  Green/Red/Blue/Indigo shirts.
void R_DrawTranslatedColumn(void);

void R_VideoErase(unsigned int ofs, int count);

extern int          ds_y;
extern int          ds_x1;
extern int          ds_x2;

extern lighttable_t *ds_colormap;

extern fixed_t      ds_xfrac;
extern fixed_t      ds_yfrac;
extern fixed_t      ds_xstep;
extern fixed_t      ds_ystep;

// start of a 64*64 tile image
extern byte         *ds_source;

extern byte         *translationtables;
extern byte         *dc_translation;

// Span blitting for rows, floor/ceiling.
// No Spectre effect needed.
void R_DrawSpan(void);
void R_DrawColorSpan(void);

void R_InitBuffer(int width, int height);

// Initialize color translation tables,
//  for player rendering etc.
void R_InitTranslationTables(void);

// Rendering function.
void R_FillBackScreen(void);

// If the view size is not full screen, draws a border around it.
void R_DrawViewBorder(void);

#endif
