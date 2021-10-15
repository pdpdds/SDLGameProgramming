
#ifndef AH_XLIB
#define AH_XLIB
#include <SDL2/SDL.h>
#include "COMPATIBILITY.HPP"

// -- Hardware screen definition.
#define HWXMAX          320
#define HWYMAX          240

#define XRATIO          ((float)HWXMAX/XSCALE)
#define YRATIO          ((float)HWYMAX/YSCALE)

#define XFACTOR			((float)XSCALE/HWXMAX)
#define YFACTOR			((float)YSCALE/HWYMAX)

#define XSTRETCH			1.5f + XSCALE/YFACTOR
#define YSTRETCH			1.5f + YSCALE/XFACTOR

// -- Virtual Screen dimensions.
#define XMAX            288         // Size of screen in x-direction.
#define YMAX            240         // And y direction.
#define LINEBYTES       80          // Bytes per line per page.
#define PLANESIZE       (LINEBYTES*YMAX)
#define SCREENSIZE      (PLANESIZE*4)
#define BORDER          16          // Size of border left and right of vscreen.
                                    // (used only for clipping, border will not be drawn).

// -- Palette Constants.
#define SCREENCOLORS    256
#define PALETTESIZE     (3*SCREENCOLORS)

// -- Tile constants.
#define XTILES          9           // Number of tiles in x screen direction.
#define TILEXS          32          // x-size of tiles in pixels.
#define TILEYS          32          // y-size of tiles in pixels.

// -- Rest of the world.
#define CRISPYSTEPS     8


// Raw Sprite definitions.
// This structure directly represents data on disk.
// So think twice before changing anything.
// I know this is not Real & Proper Object thinking.
// However it's enough for this game and it's fast and I want to go ahead.
#pragma pack (1)
struct Sprite {
   long     header;           // Header.
   long     xs_data;          // x-size of sprite padded to 16 pixels.
   long     ys_data;          // y-size of sprite.
   long     fsize;            // Size of one frame (totalsize/nframes).
   long     xs;               // Size of original sprite. (not padded)
   long     ys;               // same as SD_YSDATA.
   long     frames;           // Number of frames in sprite.
   long     speed;            // Speed needed to animate.
   long     x_core;           // x-ofs of Sprite CORE. (used for crash test).
   long     y_core;           // y-ofs of Sprite CORE. (crash test).
   long     xs_core;          // x-size of CORE.
   long     ys_core;          // y-size of CORE.
   long     data[];           // Start of data area.

};

// Position of data in SpritePool(SP) File.
// Array must be of type long.
#define SP_NSPRITES           0
#define SP_STARTOFINDEX       1
class SpritePool {
   unsigned long    *anchor;
public:
   Sprite   **sprite;         // Pointer array to sprites.
   void     load(char *file, bool createFlash);
   void     unload(void);
};


// Palette class.
#pragma pack(4)
class Palette {
   signed char    rgb[PALETTESIZE]; // RGB values of the whole palette.
   unsigned char    shadow[SCREENCOLORS];   // Shadow look up table.

public:
	SDL_Color sdl_rgb[SCREENCOLORS];
   void set(void *p);
   void set(int r, int g, int b);
};

// Font Definition.
// A font is some FONT DATA followed by a SPRITE.
// Font data is of type LONG.
#define FD_ID           0
#define FD_NCHARS       1
#define FD_BASE         2
#define FD_PITCH        3
#define FD_XSDATA       4


// Global variables.
extern Palette    palette;    // Allmaechtige ATOMPALETTE.

// ----- Prototypes.
void initgraphics(int screen_mode);
void shutgraphics(void);

// Misc.
void setScreenMode(int screen_mode);
void showpcx(void *pic, int line, int size);
void *getpcxpal(void);

#endif


