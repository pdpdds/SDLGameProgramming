/*
 * Simple Sokoban -- a (simple) Sokoban game
 * Copyright (C) 2014 Mateusz Viste
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>             /* malloc() */
#include <string.h>             /* memcpy() */
#include <time.h>
#include <SDL2/SDL.h>           /* SDL       */
#include "sok_core.h"
#include "save.h"
#include "data_lev.h"           /* embedded image files */
#include "data_img.h"           /* embedded level files */
#include "data_fnt.h"           /* embedded font files */
#include "data_skn.h"           /* embedded skin files */
#include "data_ico.h"           /* embedded the icon file */
#include "gz.h"
#include "net.h"

#define PVER "v1.0.1"

#define INET_HOST "simplesok.sourceforge.net"
#define INET_PORT 80
#define INET_PATH "/netlevels/"

#define debugmode 0

#define MAXLEVELS 4096
#define SCREEN_DEFAULT_WIDTH 800
#define SCREEN_DEFAULT_HEIGHT 600

#define DISPLAYCENTERED 1
#define NOREFRESH 2

#define DRAWSCREEN_REFRESH 1
#define DRAWSCREEN_PLAYBACK 2
#define DRAWSCREEN_PUSH 4
#define DRAWSCREEN_NOBG 8
#define DRAWSCREEN_NOTXT 16

#define DRAWSTRING_CENTER -1
#define DRAWSTRING_RIGHT -2
#define DRAWSTRING_BOTTOM -3

#define DRAWPLAYFIELDTILE_DRAWATOM 1
#define DRAWPLAYFIELDTILE_PUSH 2

#define BLIT_LEVELMAP_BACKGROUND 1

#define FONT_SPACE_WIDTH 12
#define FONT_KERNING -3

#define SELECTLEVEL_BACK -1
#define SELECTLEVEL_QUIT -2
#define SELECTLEVEL_LOADFILE -3
#define SELECTLEVEL_OK -4

enum normalizedkeys {
  KEY_UP,
  KEY_DOWN,
  KEY_LEFT,
  KEY_RIGHT,
  KEY_CTRL_UP,
  KEY_CTRL_DOWN,
  KEY_ENTER,
  KEY_BACKSPACE,
  KEY_PAGEUP,
  KEY_PAGEDOWN,
  KEY_HOME,
  KEY_END,
  KEY_ESCAPE,
  KEY_F1,
  KEY_F2,
  KEY_F3,
  KEY_F4,
  KEY_F5,
  KEY_F6,
  KEY_F7,
  KEY_F8,
  KEY_F9,
  KEY_F10,
  KEY_FULLSCREEN,
  KEY_F12,
  KEY_S,
  KEY_R,
  KEY_CTRL_C,
  KEY_CTRL_V,
  KEY_UNKNOWN
};

enum leveltype {
  LEVEL_INTERNAL,
  LEVEL_INTERNET,
  LEVEL_FILE
};

struct spritesstruct {
  SDL_Texture *atom;
  SDL_Texture *atom_on_goal;
  SDL_Texture *bg;
  SDL_Texture *black;
  SDL_Texture *cleared;
  SDL_Texture *nosolution;
  SDL_Texture *congrats;
  SDL_Texture *copiedtoclipboard;
  SDL_Texture *playfromclipboard;
  SDL_Texture *snapshottoclipboard;
  SDL_Texture *floor;
  SDL_Texture *goal;
  SDL_Texture *help;
  SDL_Texture *intro;
  SDL_Texture *player;
  SDL_Texture *saved;
  SDL_Texture *loaded;
  SDL_Texture *nosave;
  SDL_Texture *solved;
  SDL_Texture *walls[16];
  SDL_Texture *wallcaps[4];
  SDL_Texture *font[128];
};

struct videosettings {
  int tilesize;
  int nativetilesize;
  int framedelay;
  int framefreq;
};

/* returns the absolute value of the 'i' integer. */
static int absval(int i) {
  if (i < 0) return(-i);
  return(i);
}

/* uncompress a sokoban XSB string and returns a new malloced string with the decompressed version */
char *unRLE(char *xsb) {
  char *res = NULL;
  long resalloc = 16, reslen = 0;
  long x;
  int rlecnt = 1;
  res = malloc(resalloc);
  for (x = 0; xsb[x] != 0; x++) {
    if ((xsb[x] >= '0') && (xsb[x] <= '9')) {
        rlecnt = xsb[x] - '0';
      } else {
        for (; rlecnt > 0; rlecnt--) {
          if (reslen + 4 > resalloc) {
            resalloc *= 2;
            res = realloc(res, resalloc);
          }
          res[reslen++] = xsb[x];
        }
        rlecnt = 1;
    }
  }
  res[reslen] = 0;
  return(res);
}

/* normalize SDL keys to values easier to handle */
static int normalizekeys(SDL_Keycode key) {
  switch (key) {
    case SDLK_UP:
    case SDLK_KP_8:
      if (SDL_GetModState() & KMOD_CTRL) return(KEY_CTRL_UP);
      return(KEY_UP);
      break;
    case SDLK_DOWN:
    case SDLK_KP_2:
      if (SDL_GetModState() & KMOD_CTRL) return(KEY_CTRL_DOWN);
      return(KEY_DOWN);
      break;
    case SDLK_LEFT:
    case SDLK_KP_4:
      return(KEY_LEFT);
      break;
    case SDLK_RIGHT:
    case SDLK_KP_6:
      return(KEY_RIGHT);
      break;
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
      if (SDL_GetModState() & KMOD_ALT) return(KEY_FULLSCREEN);
      return(KEY_ENTER);
      break;
    case SDLK_BACKSPACE:
      return(KEY_BACKSPACE);
      break;
    case SDLK_PAGEUP:
    case SDLK_KP_9:
      return(KEY_PAGEUP);
      break;
    case SDLK_PAGEDOWN:
    case SDLK_KP_3:
      return(KEY_PAGEDOWN);
      break;
    case SDLK_HOME:
    case SDLK_KP_7:
      return(KEY_HOME);
      break;
    case SDLK_END:
    case SDLK_KP_1:
      return(KEY_END);
      break;
    case SDLK_ESCAPE:
      return(KEY_ESCAPE);
      break;
    case SDLK_F1:
      return(KEY_F1);
      break;
    case SDLK_F2:
      return(KEY_F2);
      break;
    case SDLK_F3:
      return(KEY_F3);
      break;
    case SDLK_F4:
      return(KEY_F4);
      break;
    case SDLK_F5:
      return(KEY_F5);
      break;
    case SDLK_F6:
      return(KEY_F6);
      break;
    case SDLK_F7:
      return(KEY_F7);
      break;
    case SDLK_F8:
      return(KEY_F8);
      break;
    case SDLK_F9:
      return(KEY_F9);
      break;
    case SDLK_F10:
      return(KEY_F10);
      break;
    case SDLK_F11:
      return(KEY_FULLSCREEN);
      break;
    case SDLK_F12:
      return(KEY_F12);
      break;
    case SDLK_s:
      return(KEY_S);
      break;
    case SDLK_r:
      return(KEY_R);
      break;
    case SDLK_c:
      if (SDL_GetModState() & KMOD_CTRL) return(KEY_CTRL_C);
      break;
    case SDLK_v:
      if (SDL_GetModState() & KMOD_CTRL) return(KEY_CTRL_V);
      break;
  }
  return(KEY_UNKNOWN);
}

/* loads a gziped bmp image from memory and returns a surface */
static SDL_Surface *loadgzbmp(unsigned char *memgz, long memgzlen) {
  SDL_RWops *rwop;
  SDL_Surface *surface;
  unsigned char *rawimage;
  long rawimagelen;
  if (isGz(memgz, memgzlen) == 0) return(NULL);
  rawimage = ungz(memgz, memgzlen, &rawimagelen);
  rwop = SDL_RWFromMem(rawimage, rawimagelen);
  surface = SDL_LoadBMP_RW(rwop, 0);
  SDL_FreeRW(rwop);
  free(rawimage);
  return(surface);
}

/* trims a trailing newline, if any, from a string */
static void trimstr(char *str) {
  int x, lastrealchar = -1;
  if (str == NULL) return;
  for (x = 0; str[x] != 0; x++) {
    switch (str[x]) {
      case ' ':
      case '\t':
      case '\r':
      case '\n':
        break;
      default:
        lastrealchar = x;
        break;
    }
  }
  str[lastrealchar + 1] = 0;
}

/* returns 0 if string is not a legal solution. non-zero otherwise. */
static int isLegalSokoSolution(char *solstr) {
  if (solstr == NULL) return(0);
  if (strlen(solstr) < 1) return(0);
  for (;;) {
    switch (*solstr) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        if (solstr[1] == 0) return(0); /* numbers are tolerated, but only if followed by something */
        break;
      case 'u':
      case 'U':
      case 'r':
      case 'R':
      case 'd':
      case 'D':
      case 'l':
      case 'L':
        break;
      case 0:
        return(1);
      default:
        return(0);
    }
    solstr += 1;
  }
}

/* a wrapper on the SDL_Delay() call. The difference is that it reminds the last call, so it knows if there is still time to wait or not. This allows to smooth out delays, providing accurate delaying across platforms.
 * Usage: first call it with a '0' parameter to initialize the timer, and then feed it with t microseconds as many times as needed. */
static int sokDelay(long t) {
  static Uint32 timetowait = 0, starttime = 0, irq = 0, irqfreq = 0;
  int res = 0;
  Uint32 curtime = 0;
  if (t <= 0) {
      starttime = SDL_GetTicks();
      timetowait = 0;
      irq = 0;
      irqfreq = 0 - t;
      t = 0;
    } else {
      timetowait += t;
      irq += t;
      if (irq >= irqfreq) {
        irq -= irqfreq;
        res = 1;
      }
  }
  for (;;) {
    curtime = SDL_GetTicks();
    if (starttime > curtime + 1000) break; /* the SDL_GetTicks() wraps after 49 days */
    if (curtime - starttime >= timetowait / 1000) break;
    SDL_Delay(1);
  }
  return(res);
}

static int flush_events(void) {
  SDL_Event event;
  int exitflag = 0;
  while (SDL_PollEvent(&event) != 0) if (event.type == SDL_QUIT) exitflag = 1;
  return(exitflag);
}

static void switchfullscreen(SDL_Window *window) {
  static int fullscreenflag = 0;
  fullscreenflag ^= 1;
  if (fullscreenflag != 0) {
      SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
      SDL_SetWindowFullscreen(window, 0);
  }
  SDL_Delay(50); /* wait for 50ms - the video thread needs some time to set things up */
  flush_events(); /* going fullscreen fires some garbage events that I don't want to hear about */
}

static int char2fontid(char c) {
  if ((c >= '0') && (c <= '9')) return(c - '0');
  if ((c >= 'a') && (c <= 'z')) return(10 + c - 'a');
  if ((c >= 'A') && (c <= 'Z')) return(36 + c - 'A');
  /* handle symbols */
  switch (c) {
    case ':': return(63);
    case ';': return(64);
    case '!': return(65);
    case '$': return(66);
    case '.': return(67);
    case '&': return(68);
    case '*': return(69);
    case ',': return(70);
    case '(': return(71);
    case ')': return(72);
    case '[': return(73);
    case ']': return(74);
    case '-': return(75);
    case '_': return(76);
    case '/': return(77);
    case '"': return(78);
    case '#': return(79);
    case '@': return(80);
    case '\'': return(81);
  }
  /* if anything else, return 'underscore'... */
  return(75);
}

static int getoffseth(struct sokgame *game, int winw, int tilesize) {
  /* if playfield is smaller than the screen */
  if (game->field_width * tilesize <= winw) return((winw / 2) - (game->field_width * tilesize / 2));
  /* if playfield is larger than the screen */
  if (game->positionx * tilesize + (tilesize / 2) > (winw / 2)) {
    int res = (winw / 2) - (game->positionx * tilesize + (tilesize / 2));
    if ((game->field_width * tilesize) + res < winw) res = winw - (game->field_width * tilesize);
    return(res);
  }
  return(0);
}

static int getoffsetv(struct sokgame *game, int winh, int tilesize) {
  /* if playfield is smaller than the screen */
  if (game->field_height * tilesize <= winh) return((winh / 2) - (game->field_height * tilesize / 2));
  /* if playfield is larger than the screen */
  if (game->positiony * tilesize + (tilesize / 2) > winh / 2) {
    int res = (winh / 2) - (game->positiony * tilesize + (tilesize / 2));
    if ((game->field_height * tilesize) + res < winh) res = winh - (game->field_height * tilesize);
    return(res);
  }
  return(0);
}

/* wait for a key up to timeout seconds (-1 = indefintely), while redrawing the renderer screen, if not null */
static int wait_for_a_key(int timeout, SDL_Renderer *renderer) {
  SDL_Event event;
  Uint32 timeouttime;
  timeouttime = SDL_GetTicks() + (timeout * 1000);
  for (;;) {
    SDL_Delay(50);
    if (SDL_PollEvent(&event) != 0) {
      if (renderer != NULL) SDL_RenderPresent(renderer);
      if (event.type == SDL_QUIT) {
          return(1);
        } else if (event.type == SDL_KEYDOWN) {
          return(0);
      }
    }
    if ((timeout > 0) && (SDL_GetTicks() >= timeouttime)) return(0);
  }
}

/* display a bitmap onscreen */
static int displaytexture(SDL_Renderer *renderer, SDL_Texture *texture, SDL_Window *window, int timeout, int flags, int alpha) {
  int winw, winh;
  SDL_Rect rect, *rectptr;
  SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
  SDL_GetWindowSize(window, &winw, &winh);
  if (flags & DISPLAYCENTERED) {
      rectptr = &rect;
      rect.x = (winw - rect.w) / 2;
      rect.y = (winh - rect.h) / 2;
    } else {
      rectptr = NULL;
  }
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  SDL_SetTextureAlphaMod(texture, alpha);
  if (SDL_RenderCopy(renderer, texture, NULL, rectptr) != 0) printf("SDL_RenderCopy() failed: %s\n", SDL_GetError());
  if ((flags & NOREFRESH) == 0) SDL_RenderPresent(renderer);
  if (timeout != 0) return(wait_for_a_key(timeout, renderer));
  return(0);
}

/* provides width and height of a string (in pixels) */
static void get_string_size(char *string, int fontsize, struct spritesstruct *sprites, int *w, int *h) {
  int glyphw, glyphh;
  *w = 0;
  *h = 0;
  while (*string != 0) {
    if (*string == ' ') {
        *w += FONT_SPACE_WIDTH * fontsize / 100;
      } else {
        SDL_QueryTexture(sprites->font[char2fontid(*string)], NULL, NULL, &glyphw, &glyphh);
        *w += glyphw * fontsize / 100 + FONT_KERNING * fontsize / 100;
        if (glyphh * fontsize / 100 > *h) *h = glyphh * fontsize / 100;
    }
    string += 1;
  }
}

/* explode a string into wordwrapped substrings */
void wordwrap(char *string, char **multiline, int maxlines, int maxwidth, int fontsize, struct spritesstruct *sprites) {
  int lastspace, multilineid;
  int x, stringw, stringh;
  char *tmpstring;
  /* set all multiline entries to NULL */
  for (x = 0; x < maxlines; x++) multiline[x] = NULL;

  /* find the next word boundary */
  lastspace = -1;
  multilineid = 0;
  for (;;) { /* loop on every word, and check if we reached the end */
    for (x = lastspace + 1; ; x++) {
      if ((string[x] == ' ') || (string[x] == '\t') || (string[x] == '\n') || (string[x] == 0)) {
        lastspace = x;
        break;
      }
    }
    /* is this word boundary fitting on screen? */
    tmpstring = strdup(string);
    tmpstring[lastspace] = 0;
    get_string_size(tmpstring, fontsize, sprites, &stringw, &stringh);
    if (stringw < maxwidth) {
        if (multiline[multilineid] != NULL) free(multiline[multilineid]);
        multiline[multilineid] = tmpstring;
      } else {
        free(tmpstring);
        if (multiline[multilineid] == NULL) break;
        lastspace = -1;
        string += strlen(multiline[multilineid]) + 1;
        multilineid += 1;
        if (multilineid >= maxlines) {
          int lastlinelen = strlen(multiline[multilineid - 1]);
          /* if text have been truncated, print '...' at the end of the last line */
          if (lastlinelen >= 3) {
            multiline[multilineid - 1][lastlinelen - 3] = '.';
            multiline[multilineid - 1][lastlinelen - 2] = '.';
            multiline[multilineid - 1][lastlinelen - 1] = '.';
          }
          break;
        }
    }
    if ((lastspace >= 0) && (string[lastspace] == 0)) break;
  }
}

/* blits a string onscreen, scaling the font at fontsize percents. The string is placed at starting position x/y */
static void draw_string(char *orgstring, int fontsize, int alpha, struct spritesstruct *sprites, SDL_Renderer *renderer, int x, int y, SDL_Window *window, int maxlines, int pheight) {
  int i, winw, winh;
  char *string;
  SDL_Texture *glyph;
  SDL_Rect rectsrc, rectdst;
  char *multiline[16];
  int multilineid = 0;
  if (maxlines > 16) maxlines = 16;
  /* get size of the window */
  SDL_GetWindowSize(window, &winw, &winh);
  wordwrap(orgstring, multiline, maxlines, winw - x, fontsize, sprites);
  /* loop on every line */
  for (multilineid = 0; (multiline[multilineid] != NULL) && (multilineid < maxlines); multilineid += 1) {
    string = multiline[multilineid];
    if (multilineid > 0) y += pheight;
    /* if centering is requested, get size of the string */
    if ((x < 0) || (y < 0)) {
      int stringw, stringh;
      /* get pixel length of the string */
      get_string_size(string, fontsize, sprites, &stringw, &stringh);
      if (x == DRAWSTRING_CENTER) x = (winw - stringw) >> 1;
      if (x == DRAWSTRING_RIGHT) x = winw - stringw - 10;
      if (y == DRAWSTRING_BOTTOM) y = winh - stringh;
      if (y == DRAWSTRING_CENTER) y = (winh - stringh) / 2;
    }
    rectdst.x = x;
    rectdst.y = y;
    for (i = 0; string[i] != 0; i++) {
      if (string[i] == ' ') {
        rectdst.x += FONT_SPACE_WIDTH * fontsize / 100;
        continue;
      }
      glyph = sprites->font[char2fontid(string[i])];
      SDL_QueryTexture(glyph, NULL, NULL, &rectsrc.w, &rectsrc.h);
      rectdst.w = rectsrc.w * fontsize / 100;
      rectdst.h = rectsrc.h * fontsize / 100;
      SDL_SetTextureAlphaMod(glyph, alpha);
      SDL_RenderCopy(renderer, glyph, NULL, &rectdst);
      rectdst.x += (rectsrc.w * fontsize / 100) + (FONT_KERNING * fontsize / 100);
    }
    /* free the multiline memory */
    free(string);
  }
}

static int getWallCap(struct sokgame *game, int x, int y, SDL_Rect *dstrect, SDL_Rect *orgrect, int checknum) {
  enum {
    TOPLEFT = 0,
    TOPRIGHT = 1,
    BOTTOMLEFT = 2,
    BOTTOMRIGHT = 3
  };
  int width[4];
  int height[4];
  /* precompute width and height of every quarter of the 2x2 quad set */
  width[TOPLEFT]      = orgrect->w >> 1;
  width[TOPRIGHT]     = orgrect->w - width[TOPLEFT];
  width[BOTTOMLEFT]   = orgrect->w >> 1;
  width[BOTTOMRIGHT]  = orgrect->w - width[BOTTOMLEFT];
  height[TOPLEFT]     = orgrect->h >> 1;
  height[TOPRIGHT]    = orgrect->h >> 1;
  height[BOTTOMLEFT]  = orgrect->h - height[TOPLEFT];
  height[BOTTOMRIGHT] = orgrect->h - height[TOPRIGHT];
  /* initialize x/y position to the position of the top left corner */
  dstrect->x = orgrect->x;
  dstrect->y = orgrect->y;
  /* adjust */
  switch (checknum) {
    case TOPLEFT: /* top left corner */
      dstrect->w = width[TOPLEFT];
      dstrect->h = height[TOPLEFT];
      if ((x > 0) && (y > 0) && (game->field[x - 1][y] & game->field[x][y - 1] & game->field[x - 1][y - 1] & field_wall)) return(1);
      return(0);
    case TOPRIGHT: /* top right corner */
      dstrect->w = width[TOPRIGHT];
      dstrect->h = height[TOPRIGHT];
      dstrect->x += width[TOPLEFT];
      if ((y > 0) && (game->field[x + 1][y] & game->field[x][y - 1] & game->field[x + 1][y - 1] & field_wall)) return(1);
      return(0);
    case BOTTOMLEFT: /* bottom left corner */
      dstrect->w = width[BOTTOMLEFT];
      dstrect->h = height[BOTTOMLEFT];
      dstrect->y += height[TOPLEFT];
      if ((x > 0) && (game->field[x - 1][y] & game->field[x][y + 1] & game->field[x - 1][y + 1] & field_wall)) return(1);
      return(0);
    case BOTTOMRIGHT: /* bottom right corner */
      dstrect->w = width[BOTTOMRIGHT];
      dstrect->h = height[BOTTOMRIGHT];
      dstrect->x += width[TOPRIGHT];
      dstrect->y += height[TOPRIGHT];
      if ((game->field[x + 1][y] & game->field[x][y + 1] & game->field[x + 1][y + 1] & field_wall)) return(1);
      return(0);
  }
  return(0);
}

/* get an 'id' for a wall on a given position. this is a 4-bits bitfield that indicates where the wall has neighbors (up/right/down/left). */
static int getwallid(struct sokgame *game, int x, int y) {
  int res = 0;
  if ((y > 0) && (game->field[x][y - 1] & field_wall)) res |= 1;
  if ((x < 63) && (game->field[x + 1][y] & field_wall)) res |= 2;
  if ((y < 63) && (game->field[x][y + 1] & field_wall)) res |= 4;
  if ((x > 0) && (game->field[x - 1][y] & field_wall)) res |= 8;
  return(res);
}

static void draw_playfield_tile(struct sokgame *game, int x, int y, struct spritesstruct *sprites, SDL_Renderer *renderer, int winw, int winh, struct videosettings *settings, int flags, int moveoffsetx, int moveoffsety) {
  SDL_Rect rect;
  int i;
  /* compute the dst rect */
  rect.x = getoffseth(game, winw, settings->tilesize) + (x * settings->tilesize) + moveoffsetx;
  rect.y = getoffsetv(game, winh, settings->tilesize) + (y * settings->tilesize) + moveoffsety;
  rect.w = settings->tilesize;
  rect.h = settings->tilesize;

  if ((flags & DRAWPLAYFIELDTILE_DRAWATOM) == 0) {
      if (game->field[x][y] & field_floor) SDL_RenderCopy(renderer, sprites->floor, NULL, &rect);
      if (game->field[x][y] & field_goal) SDL_RenderCopy(renderer, sprites->goal, NULL, &rect);
      if (game->field[x][y] & field_wall) {
        SDL_Rect srcrect, dstrect;
        srcrect.x = 2;
        srcrect.y = 2;
        srcrect.w = settings->nativetilesize - 2;
        srcrect.h = settings->nativetilesize - 2;
        SDL_RenderCopy(renderer, sprites->walls[getwallid(game, x, y)], &srcrect, &rect);
        /* draw the wall element (in 4 times, to draw caps when necessary) */
        for (i = 0; i < 4; i++) {
          if (getWallCap(game, x, y, &dstrect, &rect, i) != 0) {
            SDL_RenderCopy(renderer, sprites->wallcaps[i], NULL, &dstrect);
          }
        }
      }
    } else {
      int atomongoal = 0;
      if ((game->field[x][y] & field_goal) && (game->field[x][y] & field_atom)) {
        atomongoal = 1;
        if (flags & DRAWPLAYFIELDTILE_PUSH) {
          if ((game->positionx == x - 1) && (game->positiony == y) && (moveoffsetx > 0) && ((game->field[x + 1][y] & field_goal) == 0)) atomongoal = 0;
          if ((game->positionx == x + 1) && (game->positiony == y) && (moveoffsetx < 0) && ((game->field[x - 1][y] & field_goal) == 0)) atomongoal = 0;
          if ((game->positionx == x) && (game->positiony == y - 1) && (moveoffsety > 0) && ((game->field[x][y + 1] & field_goal) == 0)) atomongoal = 0;
          if ((game->positionx == x) && (game->positiony == y + 1) && (moveoffsety < 0) && ((game->field[x][y - 1] & field_goal) == 0)) atomongoal = 0;
        }
      }
      if (atomongoal != 0) {
          SDL_RenderCopy(renderer, sprites->atom_on_goal, NULL, &rect);
        } else if (game->field[x][y] & field_atom) {
          SDL_RenderCopy(renderer, sprites->atom, NULL, &rect);
      }
  }
}

static void draw_player(struct sokgame *game, struct sokgamestates *states, struct spritesstruct *sprites, SDL_Renderer *renderer, int winw, int winh, int tilesize, int offsetx, int offsety) {
  SDL_Rect rect;
  /* compute the dst rect */
  rect.x = getoffseth(game, winw, tilesize) + (game->positionx * tilesize) + offsetx;
  rect.y = getoffsetv(game, winh, tilesize) + (game->positiony * tilesize) + offsety;
  rect.w = tilesize;
  rect.h = tilesize;
  SDL_RenderCopyEx(renderer, sprites->player, NULL, &rect, states->angle, NULL, SDL_FLIP_NONE);
}

/* loads a graphic and returns its width, or -1 on error */
static int loadGraphic(SDL_Texture **texture, SDL_Renderer *renderer, void *memptr, int memlen) {
  int res;
  SDL_Surface *surface;
  surface = loadgzbmp(memptr, memlen);
  if (surface == NULL) {
    puts("loadgzbmp() failed!");
    return(-1);
  }
  res = surface->w;
  *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (*texture == NULL) {
      printf("SDL_CreateTextureFromSurface() failed: %s\n", SDL_GetError());
    } else {
      SDL_SetTextureBlendMode(*texture, SDL_BLENDMODE_BLEND);
  }
  SDL_FreeSurface(surface);
  return(res);
}

static void draw_screen(struct sokgame *game, struct sokgamestates *states, struct spritesstruct *sprites, SDL_Renderer *renderer, SDL_Window *window, struct videosettings *settings, int moveoffsetx, int moveoffsety, int scrolling, int flags, char *levelname) {
  int x, y, winw, winh, offx, offy;
  /* int partialoffsetx = 0, partialoffsety = 0; */
  char stringbuff[256];
  int scrollingadjx = 0, scrollingadjy = 0; /* this is used when scrolling + movement of player is needed */
  int drawtile_flags = 0;
  SDL_GetWindowSize(window, &winw, &winh);
  if (flags & DRAWSCREEN_NOBG) {
      SDL_RenderCopy(renderer, sprites->black, NULL, NULL);
    } else {
      SDL_RenderCopy(renderer, sprites->bg, NULL, NULL);
  }

  if (flags & DRAWSCREEN_PUSH) drawtile_flags = DRAWPLAYFIELDTILE_PUSH;

  if (scrolling > 0) {
    if (moveoffsetx > scrolling) {
      scrollingadjx = moveoffsetx - scrolling;
      moveoffsetx = scrolling;
    }
    if (moveoffsetx < -scrolling) {
      scrollingadjx = moveoffsetx + scrolling;
      moveoffsetx = -scrolling;
    }
    if (moveoffsety > scrolling) {
      scrollingadjy = moveoffsety - scrolling;
      moveoffsety = scrolling;
    }
    if (moveoffsety < -scrolling) {
      scrollingadjy = moveoffsety + scrolling;
      moveoffsety = -scrolling;
    }
  }
  /* draw non-moveable tiles (floors, walls, goals) */
  for (y = 0; y < game->field_height; y++) {
    for (x = 0; x < game->field_width; x++) {
      if (scrolling != 0) {
          draw_playfield_tile(game, x, y, sprites, renderer, winw, winh, settings, drawtile_flags, -moveoffsetx, -moveoffsety);
        } else {
          draw_playfield_tile(game, x, y, sprites, renderer, winw, winh, settings, drawtile_flags, 0, 0);
      }
    }
  }
  /* draw moveable elements (atoms) */
  for (y = 0; y < game->field_height; y++) {
    for (x = 0; x < game->field_width; x++) {
      offx = 0;
      offy = 0;
      if (scrolling == 0) {
          if ((moveoffsetx > 0) && (x == game->positionx + 1) && (y == game->positiony)) offx = moveoffsetx;
          if ((moveoffsetx < 0) && (x == game->positionx - 1) && (y == game->positiony)) offx = moveoffsetx;
          if ((moveoffsety > 0) && (y == game->positiony + 1) && (x == game->positionx)) offy = moveoffsety;
          if ((moveoffsety < 0) && (y == game->positiony - 1) && (x == game->positionx)) offy = moveoffsety;
        } else {
          offx = -moveoffsetx;
          offy = -moveoffsety;
          if ((moveoffsetx > 0) && (x == game->positionx + 1) && (y == game->positiony)) offx = scrollingadjx;
          if ((moveoffsetx < 0) && (x == game->positionx - 1) && (y == game->positiony)) offx = scrollingadjx;
          if ((moveoffsety > 0) && (y == game->positiony + 1) && (x == game->positionx)) offy = scrollingadjy;
          if ((moveoffsety < 0) && (y == game->positiony - 1) && (x == game->positionx)) offy = scrollingadjy;
      }
      draw_playfield_tile(game, x, y, sprites, renderer, winw, winh, settings, DRAWPLAYFIELDTILE_DRAWATOM, offx, offy);
    }
  }
  /* draw where the player is */
  if (scrolling != 0) {
      draw_player(game, states, sprites, renderer, winw, winh, settings->tilesize, scrollingadjx, scrollingadjy);
    } else {
      draw_player(game, states, sprites, renderer, winw, winh, settings->tilesize, moveoffsetx, moveoffsety);
  }
  /* draw text */
  if ((flags & DRAWSCREEN_NOTXT) == 0) {
    sprintf(stringbuff, "%s, level %d", levelname, game->level);
    draw_string(stringbuff, 100, 255, sprites, renderer, 10, DRAWSTRING_BOTTOM, window, 1, 0);
    if (game->solution != NULL) {
        sprintf(stringbuff, "best score: %ld/%ld", sok_history_getlen(game->solution), sok_history_getpushes(game->solution));
      } else {
        sprintf(stringbuff, "best score: -");
    }
    draw_string(stringbuff, 100, 255, sprites, renderer, DRAWSTRING_RIGHT, 0, window, 1, 0);
    sprintf(stringbuff, "moves: %ld / pushes: %ld", sok_history_getlen(states->history), sok_history_getpushes(states->history));
    draw_string(stringbuff, 100, 255, sprites, renderer, 10, 0, window, 1, 0);
  }
  if ((flags & DRAWSCREEN_PLAYBACK) && (time(NULL) % 2 == 0)) draw_string("*** PLAYBACK ***", 100, 255, sprites, renderer, DRAWSTRING_CENTER, 32, window, 1, 0);
  /* Update the screen */
  if (flags & DRAWSCREEN_REFRESH) SDL_RenderPresent(renderer);
}

static int rotatePlayer(struct spritesstruct *sprites, struct sokgame *game, struct sokgamestates *states, enum SOKMOVE dir, SDL_Renderer *renderer, SDL_Window *window, struct videosettings *settings, char *levelname, int drawscreenflags) {
  int srcangle = states->angle;
  int dstangle = 0, dirmotion, winw, winh;
  SDL_GetWindowSize(window, &winw, &winh);
  switch (dir) {
    case sokmoveUP:
      dstangle = 0;
      break;
    case sokmoveRIGHT:
      dstangle = 90;
      break;
    case sokmoveDOWN:
      dstangle = 180;
      break;
    case sokmoveLEFT:
      dstangle = 270;
      break;
  }
  /* figure out how to compute the shortest way to rotate the player... This is not a very efficient way, but it works.. I might improve it in the future... */
  if (srcangle != dstangle) {
    int tmpangle, stepsright = 0, stepsleft = 0;
    for (tmpangle = srcangle; ; tmpangle += 90) {
      if (tmpangle >= 360) tmpangle -= 360;
      stepsright += 1;
      if (tmpangle == dstangle) break;
    }
    for (tmpangle = srcangle; ; tmpangle -= 90) {
      if (tmpangle < 0) tmpangle += 360;
      stepsleft += 1;
      if (tmpangle == dstangle) break;
    }
    if (stepsleft < stepsright) {
        dirmotion = -1;
      } else if (stepsleft > stepsright) {
        dirmotion = 1;
      } else {
        if (rand() % 2 == 0) {
            dirmotion = -1;
          } else {
            dirmotion = 1;
        }
    }
    /* perform the rotation */
    sokDelay(0 - settings->framefreq); /* init my delay timer */
    for (tmpangle = srcangle; ; tmpangle += dirmotion) {
      if (tmpangle >= 360) tmpangle = 0;
      if (tmpangle < 0) tmpangle = 359;
      states->angle = tmpangle;
      if (sokDelay(settings->framedelay / 8)) { /* wait for x ms */
        draw_screen(game, states, sprites, renderer, window, settings, 0, 0, 0, DRAWSCREEN_REFRESH | drawscreenflags, levelname);
      }
      if (tmpangle == dstangle) break;
    }
    return(1);
  }
  return(0);
}

static int scrollneeded(struct sokgame *game, SDL_Window *window, int tilesize, int offx, int offy) {
  int winw, winh, offsetx, offsety, result = 0;
  SDL_GetWindowSize(window, &winw, &winh);
  offsetx = absval(getoffseth(game, winw, tilesize));
  offsety = absval(getoffsetv(game, winh, tilesize));
  game->positionx += offx;
  game->positiony += offy;
  result = offsetx - absval(getoffseth(game, winw, tilesize));
  if (result == 0) result = offsety - absval(getoffsetv(game, winh, tilesize));
  if (result < 0) result = -result; /* convert to abs() value */
  game->positionx -= offx;
  game->positiony -= offy;
  return(result);
}

static void loadlevel(struct sokgame *togame, struct sokgame *fromgame, struct sokgamestates *states) {
  memcpy(togame, fromgame, sizeof(struct sokgame));
  sok_resetstates(states);
}

static char *processDropFileEvent(SDL_Event *event, char **levelfile) {
  if (event->type != SDL_DROPFILE) return(NULL);
  if (event->drop.file == NULL) return(NULL);
  if (*levelfile != NULL) free(*levelfile);
  *levelfile = strdup(event->drop.file);
  return(*levelfile);
}

/* waits for the user to choose a game type or to load an external xsb file and returns either a pointer to a memory chunk with xsb data or to fill levelfile with a filename */
static unsigned char *selectgametype(SDL_Renderer *renderer, struct spritesstruct *sprites, SDL_Window *window, struct videosettings *settings, char **levelfile, long *levelfilelen) {
  int exitflag, winw, winh, stringw, stringh, longeststringw;
  static int selection = 0;
  int oldpusherposy = 0, newpusherposy, x, selectionchangeflag = 0;
  unsigned char *memptr[3] = {levels_microban_xsb_gz, levels_sasquatch_xsb_gz, levels_sasquatch3_xsb_gz};
  long memptrlen[3];
  char *levname[5] = {"Easy (Microban)", "Normal (Sasquatch)", "Hard (Sasquatch III)", "Internet levels", "Quit"};
  int textvadj = 12;
  int selectionpos[16];
  SDL_Event event;
  SDL_Rect rect;

  *levelfilelen = 0;
  memptrlen[0] = levels_microban_xsb_gz_len;
  memptrlen[1] = levels_sasquatch_xsb_gz_len;
  memptrlen[2] = levels_sasquatch3_xsb_gz_len;

  /* compute the pixel width of the longest string in the menu */
  longeststringw = 0;
  for (x = 0; x < 5; x++) {
    get_string_size(levname[x], 100, sprites, &stringw, &stringh);
    if (stringw > longeststringw) longeststringw = stringw;
  }

  for (;;) {
    int refreshnow = 1;
    /* get windows x / y size */
    SDL_GetWindowSize(window, &winw, &winh);
    /* precompute the y-axis position of every line */
    for (x = 0; x < 5; x++) {
      selectionpos[x] = (winh * 0.51) + (winh * 0.06 * x);
      if (x > 2) selectionpos[x] += (winh / 64); /* add a small vertical gap before "Internet levels" */
      if (x > 3) selectionpos[x] += (winh / 64); /* add a small vertical gap before "Quit" */
    }
    /* compute the dst rect of the pusher */
    rect.x = ((winw - longeststringw) >> 1) - 54;
    newpusherposy = selectionpos[selection];
    rect.w = settings->tilesize;
    rect.h = settings->tilesize;
    if (selectionchangeflag == 0) oldpusherposy = newpusherposy;
    /* draw the screen */
    rect.y = oldpusherposy;
    sokDelay(0 - settings->framefreq); /* init my delay timer */
    for (;;) {
      if (refreshnow) { /* wait for x ms */
        displaytexture(renderer, sprites->intro, window, 0, NOREFRESH, 255);
        SDL_RenderCopyEx(renderer, sprites->player, NULL, &rect, 90, NULL, SDL_FLIP_NONE);
        for (x = 0; x < 5; x++) {
          draw_string(levname[x], 100, 255, sprites, renderer, rect.x + 54, textvadj + selectionpos[x], window, 1, 0);
        }
        SDL_RenderPresent(renderer);
        if (rect.y == newpusherposy) break;
      }
      if (newpusherposy < oldpusherposy) {
          rect.y -= 1;
          if (rect.y < newpusherposy) rect.y = newpusherposy;
        } else {
          rect.y += 1;
          if (rect.y > newpusherposy) rect.y = newpusherposy;
      }
      refreshnow = sokDelay(settings->framedelay / 4);
    }
    oldpusherposy = newpusherposy;
    selectionchangeflag = 0;

    /* Wait for an event - but ignore 'KEYUP' and 'MOUSEMOTION' events, since they are worthless in this game */
    for (;;) if ((SDL_WaitEvent(&event) != 0) && (event.type != SDL_KEYUP) && (event.type != SDL_MOUSEMOTION)) break;

    /* check what event we got */
    if (event.type == SDL_QUIT) {
        return(NULL);
      } else if (event.type == SDL_DROPFILE) {
        if (processDropFileEvent(&event, levelfile) != NULL) return(NULL);
      } else if (event.type == SDL_KEYDOWN) {
        switch (normalizekeys(event.key.keysym.sym)) {
          case KEY_UP:
            selection--;
            selectionchangeflag = 1;
            break;
          case KEY_DOWN:
            selection++;
            selectionchangeflag = 1;
            break;
          case KEY_ENTER:
            if (selection == 3) return((unsigned char *)"@");
            if (selection == 4) return(NULL); /* Quit */
            *levelfilelen = memptrlen[selection];
            return(memptr[selection]);
            break;
          case KEY_FULLSCREEN:
            switchfullscreen(window);
            break;
          case KEY_ESCAPE:
            return(NULL);
            break;
        }
        if (selection < 0) selection = 4;
        if (selection > 4) selection = 0;
    }
  }
  if (exitflag != 0) return(NULL);
}

/* blit a level preview */
static void blit_levelmap(struct sokgame *game, struct spritesstruct *sprites, int xpos, int ypos, SDL_Renderer *renderer, int nativetilesize, int tilesize, int alpha, int flags) {
  int x, y, bgpadding = tilesize * 3;
  SDL_Rect rect, bgrect, srcrect;
  rect.w = tilesize;
  rect.h = tilesize;
  bgrect.x = xpos - (game->field_width * tilesize + bgpadding) / 2;
  bgrect.y = ypos - (game->field_height * tilesize + bgpadding) / 2;
  bgrect.w = game->field_width * tilesize + bgpadding;
  bgrect.h = game->field_height * tilesize + bgpadding;
  /* if background enabled, compute coordinates of the background and draw it */
  if (flags & BLIT_LEVELMAP_BACKGROUND) {
    SDL_SetRenderDrawColor(renderer, 0x12, 0x12, 0x12, 255);
    SDL_RenderFillRect(renderer, &bgrect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  }
  for (y = 0; y < game->field_height; y++) {
    for (x = 0; x < game->field_width; x++) {
      /* compute coordinates of the tile on screen */
      rect.x = xpos + (tilesize * x) - (game->field_width * tilesize) / 2;
      rect.y = ypos + (tilesize * y) - (game->field_height * tilesize) / 2;
      rect.w = tilesize;
      rect.h = tilesize;
      /* trim out 4 pixels on every wall tile */
      srcrect.x = 2;
      srcrect.y = 2;
      srcrect.w = nativetilesize - 2;
      srcrect.h = nativetilesize - 2;
      /* draw the tile */
      if (game->field[x][y] & field_floor) SDL_RenderCopy(renderer, sprites->floor, NULL, &rect);
      if (game->field[x][y] & field_wall) {
        int i;
        SDL_Rect dstrect;
        SDL_RenderCopy(renderer, sprites->walls[getwallid(game, x, y)], &srcrect, &rect);
        /* check for neighbors and draw wall cap if needed */
        for (i = 0; i < 4; i++) {
          if (getWallCap(game, x, y, &dstrect, &rect, i) != 0) {
            SDL_RenderCopy(renderer, sprites->wallcaps[i], NULL, &dstrect);
          }
        }
      }
      if ((game->field[x][y] & field_goal) && (game->field[x][y] & field_atom)) { /* atom on goal */
          SDL_RenderCopy(renderer, sprites->atom_on_goal, NULL, &rect);
        } else if (game->field[x][y] & field_goal) { /* goal */
          SDL_RenderCopy(renderer, sprites->goal, NULL, &rect);
        } else if (game->field[x][y] & field_atom) { /* atom */
          SDL_RenderCopy(renderer, sprites->atom, NULL, &rect);
      }
    }
  }
  /* apply alpha filter */
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255 - alpha);
  SDL_RenderFillRect(renderer, &bgrect);
  /* if background enabled, then draw the border */
  if (flags & BLIT_LEVELMAP_BACKGROUND) {
    int i;
    SDL_SetRenderDrawColor(renderer, 0x28, 0x28, 0x28, 255);
    SDL_RenderDrawRect(renderer, &bgrect);
    /* draw a nice fade-out effect around the selected level */
    for (i = 1; i < 20; i++) {
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255 - i * (255 / 20));
      bgrect.x -= 1;
      bgrect.y -= 1;
      bgrect.w += 2;
      bgrect.h += 2;
      SDL_RenderDrawRect(renderer, &bgrect);
    }
    /* set the drawing color to its default, plain black color */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  }
  /* if level is solved, draw a 'complete' tag */
  if (game->solution != NULL) {
    /* SDL_Rect rect; */
    SDL_QueryTexture(sprites->solved, NULL, NULL, &rect.w, &rect.h);
    rect.w = 1.5 * (rect.w * tilesize) / nativetilesize;
    rect.h = 1.5 * (rect.h * tilesize) / nativetilesize;
    rect.x = xpos - (rect.w / 2);
    rect.y = ypos - (rect.h / 2);
    SDL_RenderCopy(renderer, sprites->solved, NULL, &rect);
  }
}

static int fade2texture(SDL_Renderer *renderer, SDL_Window *window, SDL_Texture *texture) {
  int alphaval, exitflag = 0;
  sokDelay(0);  /* init my delay timer */
  for (alphaval = 0; alphaval < 64; alphaval += 4) {
    exitflag = displaytexture(renderer, texture, window, 0, 0, alphaval);
    if (exitflag != 0) break;
    sokDelay(15 * 1000);  /* wait for 15ms */
  }
  if (exitflag == 0) exitflag = displaytexture(renderer, texture, window, 0, 0, 255);
  return(exitflag);
}

static int selectlevel(struct sokgame **gameslist, struct spritesstruct *sprites, SDL_Renderer *renderer, SDL_Window *window, struct videosettings *settings, char *levcomment, int levelscount, int selection, char **levelfile) {
  int i, winw, winh, maxallowedlevel;
  char levelnum[64];
  SDL_Event event;
  /* reload all solutions for levels, in case they changed (for ex. because we just solved a level..) */
  sok_loadsolutions(gameslist, levelscount);

  /* if no current level is selected, then preselect the first unsolved level */
  if (selection < 0) {
    for (i = 0; i < levelscount; i++) {
      if (gameslist[i]->solution != NULL) {
          if (debugmode != 0) printf("Level %d [%08lX] has solution: %s\n", i + 1, gameslist[i]->crc32, gameslist[i]->solution);
        } else {
          if (debugmode != 0) printf("Level %d [%08lX] has NO solution\n", i + 1, gameslist[i]->crc32);
          selection = i;
          break;
      }
    }
  }

  /* if no unsolved level found, then select the first one */
  if (selection < 0) selection = 0;

  /* compute the last allowed level */
  i = 0; /* i will temporarily store the number of unsolved levels */
  for (maxallowedlevel = 0; maxallowedlevel < levelscount; maxallowedlevel++) {
    if (gameslist[maxallowedlevel]->solution == NULL) i++;
    if (i > 3) break; /* user can see up to 3 unsolved levels */
  }

  /* loop */
  for (;;) {
    SDL_GetWindowSize(window, &winw, &winh);

    /* draw the screen */
    SDL_RenderClear(renderer);
    /* draw the level before */
    if (selection > 0) blit_levelmap(gameslist[selection - 1], sprites, winw / 5, winh / 2, renderer, settings->nativetilesize, settings->tilesize / 4, 96, 0);
    /* draw the level after */
    if (selection + 1 < maxallowedlevel) blit_levelmap(gameslist[selection + 1], sprites, winw * 4 / 5,  winh / 2, renderer, settings->nativetilesize, settings->tilesize / 4, 96, 0);
    /* draw the selected level */
    blit_levelmap(gameslist[selection], sprites,  winw / 2,  winh / 2, renderer, settings->nativetilesize, settings->tilesize / 3, 210, BLIT_LEVELMAP_BACKGROUND);
    /* draw strings, etc */
    draw_string(levcomment, 100, 255, sprites, renderer, DRAWSTRING_CENTER, winh / 8, window, 1, 0);
    draw_string("(choose a level)", 100, 255, sprites, renderer, DRAWSTRING_CENTER, winh / 8 + 40, window, 1, 0);
    sprintf(levelnum, "Level %d of %d", selection + 1, levelscount);
    draw_string(levelnum, 100, 255, sprites, renderer, DRAWSTRING_CENTER, winh * 3 / 4, window, 1, 0);
    SDL_RenderPresent(renderer);

    /* Wait for an event - but ignore 'KEYUP' and 'MOUSEMOTION' events, since they are worthless in this game */
    for (;;) if ((SDL_WaitEvent(&event) != 0) && (event.type != SDL_KEYUP) && (event.type != SDL_MOUSEMOTION)) break;

    /* check what event we got */
    if (event.type == SDL_QUIT) {
        return(SELECTLEVEL_QUIT);
      } else if (event.type == SDL_DROPFILE) {
        if (processDropFileEvent(&event, levelfile) != NULL) {
          fade2texture(renderer, window, sprites->black);
          return(SELECTLEVEL_LOADFILE);
        }
      } else if (event.type == SDL_KEYDOWN) {
        switch (normalizekeys(event.key.keysym.sym)) {
          case KEY_LEFT:
            if (selection > 0) selection--;
            break;
          case KEY_RIGHT:
            if (selection + 1 < maxallowedlevel) selection++;
            break;
          case KEY_HOME:
            selection = 0;
            break;
          case KEY_END:
            selection = maxallowedlevel - 1;
            break;
          case KEY_PAGEUP:
            if (selection < 3) {
                selection = 0;
              } else {
                selection -= 3;
            }
            break;
          case KEY_PAGEDOWN:
            if (selection + 3 >= maxallowedlevel) {
                selection = maxallowedlevel - 1;
              } else {
                selection += 3;
            }
            break;
          case KEY_CTRL_UP:
            if (settings->tilesize < 255) settings->tilesize += 4;
            break;
          case KEY_CTRL_DOWN:
            if (settings->tilesize > 6) settings->tilesize -= 4;
            break;
          case KEY_ENTER:
            return(selection);
            break;
          case KEY_FULLSCREEN:
            switchfullscreen(window);
            break;
          case KEY_ESCAPE:
            fade2texture(renderer, window, sprites->black);
            return(SELECTLEVEL_BACK);
            break;
        }
    }
  }
}

/* sets the icon in the aplication's title bar */
static void setsokicon(SDL_Window *window) {
  SDL_Surface *surface;
  surface = loadgzbmp(simplesok_bmp_gz, simplesok_bmp_gz_len);
  if (surface == NULL) return;
  SDL_SetWindowIcon(window, surface);
  SDL_FreeSurface(surface); /* once the icon is loaded, the surface is not needed anymore */
}

/* returns 1 if curlevel is the last level to solve in the set. returns 0 otherwise. */
static int islevelthelastleft(struct sokgame **gamelist, int curlevel, int levelscount) {
  int x;
  if (curlevel < 0) return(0);
  if (gamelist[curlevel]->solution != NULL) return(0);
  for (x = 0; x < levelscount; x++) {
    if ((gamelist[x]->solution == NULL) && (x != curlevel)) return(0);
  }
  return(1);
}

static void dumplevel2clipboard(struct sokgame *game, char *history) {
  char *txt;
  long solutionlen = 0, playfieldsize;
  int x, y;
  if (game->solution != NULL) solutionlen = strlen(game->solution);
  playfieldsize = (game->field_width + 1) * game->field_height;
  txt = malloc(solutionlen + playfieldsize + 4096);
  if (txt == NULL) return;
  sprintf(txt, "; Level id: %lX\n\n", game->crc32);
  for (y = 0; y < game->field_height; y++) {
    for (x = 0; x < game->field_width; x++) {
      switch (game->field[x][y] & ~field_floor) {
        case field_wall:
          strcat(txt, "#");
          break;
        case (field_atom | field_goal):
          strcat(txt, "*");
          break;
        case field_atom:
          strcat(txt, "$");
          break;
        case field_goal:
          if ((game->positionx == x) && (game->positiony == y)) {
              strcat(txt, "+");
            } else {
              strcat(txt, ".");
          }
          break;
        default:
          if ((game->positionx == x) && (game->positiony == y)) {
              strcat(txt, "@");
            } else {
              strcat(txt, " ");
          }
          break;
      }
    }
    strcat(txt, "\n");
  }
  strcat(txt, "\n");
  if ((history != NULL) && (history[0] != 0)) { /* only allow if there actually is a solution */
      strcat(txt, "; Solution\n; ");
      strcat(txt, history);
      strcat(txt, "\n");
    } else {
      strcat(txt, "; No solution available\n");
  }
  SDL_SetClipboardText(txt);
  free(txt);
}

/* reads a chunk of text from memory. returns the line in a chunk of memory that needs to be freed afterwards */
static char *readmemline(char **memptrptr) {
  long linelen;
  char *res;
  char *memptr;
  memptr = *memptrptr;
  if (*memptr == 0) return(NULL);
  /* check how long the line is */
  for (linelen = 0; ; linelen += 1) {
    if (memptr[linelen] == 0) break;
    if (memptr[linelen] == '\n') break;
  }
  /* allocate memory for the line, and copy its content */
  res = malloc(linelen + 1);
  memcpy(res, memptr, linelen);
  /* move the original pointer forward */
  *memptrptr += linelen;
  if (**memptrptr == '\n') *memptrptr += 1;
  /* trim out trailing \r, if any */
  if ((linelen > 0) && (res[linelen - 1] == '\r')) linelen -= 1;
  /* terminate the line with a null terminator */
  res[linelen] = 0;
  return(res);
}

static void fetchtoken(char *res, char *buf, int pos) {
  int tokpos = 0, x;
  /* forward to the position where the token starts */
  while (tokpos != pos) {
    if (*buf == 0) {
        break;
      } else if (*buf == '\t') {
        tokpos += 1;
    }
    buf += 1;
  }
  /* copy the token to buf, until \t or \0 */
  for (x = 0;; x++) {
    if (buf[x] == '\0') break;
    if (buf[x] == '\t') break;
    res[x] = buf[x];
  }
  res[x] = 0;
}

static int selectinternetlevel(SDL_Renderer *renderer, SDL_Window *window, struct spritesstruct *sprites, char *host, long port, char *path, char *levelslist, unsigned char **xsbptr, long *reslen) {
  unsigned char *res = NULL;
  char url[1024], buff[1024], buff2[1024];
  char *inetlist[1024];
  int inetlistlen = 0, i, selected = 0, windowrows, fontheight = 24, winw, winh;
  static int selection = 0, seloffset = 0;
  SDL_Event event;
  *xsbptr = NULL;
  *reslen = 0;
  /* load levelslist into an array */
  for (;;) {
    inetlist[inetlistlen] = readmemline(&levelslist);
    if (inetlist[inetlistlen] == NULL) break;
    inetlistlen += 1;
    if (inetlistlen >= 1024) break;
  }
  if (inetlistlen < 1) return(SELECTLEVEL_BACK); /* if failed to load any level, quit here */
  /* selection loop */
  for (;;) {
    SDL_Rect rect;
    /* compute the amount of rows we can fit onscreen */
    SDL_GetWindowSize(window, &winw, &winh);
    windowrows = (winh / fontheight) - 7;
    /* display the list of levels */
    SDL_RenderClear(renderer);
    for (i = 0; i < windowrows; i++) {
      if (i + seloffset >= inetlistlen) break;
      fetchtoken(buff, inetlist[i + seloffset], 1);
      draw_string(buff, 100, 255, sprites, renderer, 30, i * fontheight, window, 1, 0);
      if (i + seloffset == selection) {
        rect.x = 0;
        rect.y = i * fontheight;
        rect.w = 30;
        rect.h = 30;
        SDL_RenderCopyEx(renderer, sprites->player, NULL, &rect, 90, NULL, SDL_FLIP_NONE);
      }
    }
    /* render backround of level description */
    rect.x = 0;
    rect.y = windowrows * fontheight + fontheight * 0.4;
    rect.w = winw;
    rect.h = winh;
    SDL_SetRenderDrawColor(renderer, 0x30, 0x30, 0x30, 255);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 0xC0, 0xC0, 0xC0, 255);
    SDL_RenderDrawLine(renderer, 0, rect.y, winw, rect.y);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    /* draw level description */
    rect.y += fontheight * 0.5;
    fetchtoken(buff2, inetlist[selection], 1);
    draw_string(buff2, 100, 250, sprites, renderer, DRAWSTRING_CENTER, rect.y, window, 1, 0);
    fetchtoken(buff2, inetlist[selection], 2);
    sprintf(buff, "Copyright (C) %s", buff2);
    draw_string(buff, 65, 200, sprites, renderer, DRAWSTRING_CENTER, rect.y + fontheight * 1.2, window, 1, 0);
    fetchtoken(buff, inetlist[selection], 3);
    draw_string(buff, 100, 210, sprites, renderer, 0, rect.y + fontheight * 2.6, window, 3, fontheight);
    /* refresh screen */
    SDL_RenderPresent(renderer);
    /* Wait for an event - but ignore 'KEYUP' and 'MOUSEMOTION' events, since they are worthless in this game */
    for (;;) {
      SDL_WaitEvent(&event);
      if ((event.type != SDL_KEYUP) && (event.type != SDL_MOUSEMOTION)) break;
    }
    /* check what event we got */
    if (event.type == SDL_QUIT) {
        selected = SELECTLEVEL_QUIT;
      /* } else if (event.type == SDL_DROPFILE) {
        if (processDropFileEvent(&event, &levelfile) != NULL) {
          fade2texture(renderer, window, sprites->black);
          goto GametypeSelectMenu;
        } */
      } else if (event.type == SDL_KEYDOWN) {
        switch (normalizekeys(event.key.keysym.sym)) {
          case KEY_UP:
            if (selection > 0) selection -= 1;
            if ((seloffset > 0) && (selection < seloffset + 2)) seloffset -= 1;
            break;
          case KEY_DOWN:
            if (selection + 1 < inetlistlen) selection += 1;
            if ((seloffset < inetlistlen - windowrows) && (selection >= seloffset + windowrows - 2)) seloffset += 1;
            break;
          case KEY_ENTER:
            selected = SELECTLEVEL_OK;
            break;
          case KEY_ESCAPE:
            selected = SELECTLEVEL_BACK;
            break;
          case KEY_FULLSCREEN:
            switchfullscreen(window);
            break;
          case KEY_HOME:
            selection = 0;
            seloffset = 0;
            break;
          case KEY_END:
            selection = inetlistlen - 1;
            seloffset = inetlistlen - windowrows;
            break;
        }
    }
    if (selected != 0) break;
  }
  /* fetch the selected level */
  if (selected == SELECTLEVEL_OK) {
      fetchtoken(buff, inetlist[selection], 0);
      sprintf(url, "%s%s", path, buff);
      *reslen = http_get(host, port, url, &res);
      *xsbptr = res;
    } else {
      *xsbptr = NULL;
  }
  /* free the list */
  while (inetlistlen > 0) {
    inetlistlen -= 1;
    free(inetlist[inetlistlen]);
  }
  fade2texture(renderer, window, sprites->black);
  return(selected);
}

int main(int argc, char **argv) {
  struct sokgame **gameslist, game;
  struct sokgamestates *states;
  struct spritesstruct spritesdata;
  struct spritesstruct *sprites = &spritesdata;
  int levelscount, curlevel, exitflag = 0, showhelp = 0, x, lastlevelleft;
  int playsolution, drawscreenflags;
  char *levelfile = NULL;
  char *playsource = NULL;
  char *levelslist = NULL;
  #define LEVCOMMENTMAXLEN 32
  char levcomment[LEVCOMMENTMAXLEN];
  struct videosettings settings;
  unsigned char *xsblevelptr = NULL;
  long xsblevelptrlen = 0;
  enum leveltype levelsource = LEVEL_INTERNAL;

  SDL_Window* window = NULL;
  SDL_Renderer *renderer;
  SDL_Event event;

  /* init (seed) the randomizer */
  srand(time(NULL));

  /* init networking stack (required on windows) */
  init_net();

  /* Init SDL and set the video mode */
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("SDL_Init() failed: %s\n", SDL_GetError());
    return(1);
  }
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");  /* this makes scaling nicer */

  window = SDL_CreateWindow("Simple Sokoban " PVER, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_DEFAULT_WIDTH, SCREEN_DEFAULT_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (window == NULL) {
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    return(1);
  }

  setsokicon(window);
  SDL_SetWindowMinimumSize(window, 160, 120);

  renderer = SDL_CreateRenderer(window, -1, 0);
  if (renderer == NULL) {
    SDL_DestroyWindow(window);
    printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
    return(1);
  }

  settings.framedelay = -1;
  settings.framefreq = -1;

  /* Load sprites */
  loadGraphic(&sprites->atom, renderer, skin_atom_bmp_gz, skin_atom_bmp_gz_len);
  loadGraphic(&sprites->atom_on_goal, renderer, skin_atom_on_goal_bmp_gz, skin_atom_on_goal_bmp_gz_len);
  settings.nativetilesize = loadGraphic(&sprites->floor, renderer, skin_floor_bmp_gz, skin_floor_bmp_gz_len);
  loadGraphic(&sprites->goal, renderer, skin_goal_bmp_gz, skin_goal_bmp_gz_len);
  loadGraphic(&sprites->player, renderer, skin_player_bmp_gz, skin_player_bmp_gz_len);
  loadGraphic(&sprites->intro, renderer, img_intro_bmp_gz, img_intro_bmp_gz_len);
  loadGraphic(&sprites->bg, renderer, skin_bg_bmp_gz, skin_bg_bmp_gz_len);
  loadGraphic(&sprites->black, renderer, img_black_bmp_gz, img_black_bmp_gz_len);
  loadGraphic(&sprites->cleared, renderer, img_cleared_bmp_gz, img_cleared_bmp_gz_len);
  loadGraphic(&sprites->help, renderer, img_help_bmp_gz, img_help_bmp_gz_len);
  loadGraphic(&sprites->solved, renderer, img_solved_bmp_gz, img_solved_bmp_gz_len);
  loadGraphic(&sprites->nosolution, renderer, img_nosol_bmp_gz, img_nosol_bmp_gz_len);
  loadGraphic(&sprites->congrats, renderer, img_congrats_bmp_gz, img_congrats_bmp_gz_len);
  loadGraphic(&sprites->copiedtoclipboard, renderer, img_copiedtoclipboard_bmp_gz, img_copiedtoclipboard_bmp_gz_len);
  loadGraphic(&sprites->playfromclipboard, renderer, img_playfromclipboard_bmp_gz, img_playfromclipboard_bmp_gz_len);
  loadGraphic(&sprites->snapshottoclipboard, renderer, img_snapshottoclipboard_bmp_gz, img_snapshottoclipboard_bmp_gz_len);
  loadGraphic(&sprites->saved, renderer, img_saved_bmp_gz, img_saved_bmp_gz_len);
  loadGraphic(&sprites->loaded, renderer, img_loaded_bmp_gz, img_loaded_bmp_gz_len);
  loadGraphic(&sprites->nosave, renderer, img_nosave_bmp_gz, img_nosave_bmp_gz_len);

  /* load walls */
  for (x = 0; x < 16; x++) sprites->walls[x] = NULL;
  loadGraphic(&sprites->walls[0],  renderer, skin_wall0_bmp_gz,  skin_wall0_bmp_gz_len);
  loadGraphic(&sprites->walls[1],  renderer, skin_wall1_bmp_gz,  skin_wall1_bmp_gz_len);
  loadGraphic(&sprites->walls[2],  renderer, skin_wall2_bmp_gz,  skin_wall2_bmp_gz_len);
  loadGraphic(&sprites->walls[3],  renderer, skin_wall3_bmp_gz,  skin_wall3_bmp_gz_len);
  loadGraphic(&sprites->walls[4],  renderer, skin_wall4_bmp_gz,  skin_wall4_bmp_gz_len);
  loadGraphic(&sprites->walls[5],  renderer, skin_wall5_bmp_gz,  skin_wall5_bmp_gz_len);
  loadGraphic(&sprites->walls[6],  renderer, skin_wall6_bmp_gz,  skin_wall6_bmp_gz_len);
  loadGraphic(&sprites->walls[7],  renderer, skin_wall7_bmp_gz,  skin_wall7_bmp_gz_len);
  loadGraphic(&sprites->walls[8],  renderer, skin_wall8_bmp_gz,  skin_wall8_bmp_gz_len);
  loadGraphic(&sprites->walls[9],  renderer, skin_wall9_bmp_gz,  skin_wall9_bmp_gz_len);
  loadGraphic(&sprites->walls[10], renderer, skin_wall10_bmp_gz, skin_wall10_bmp_gz_len);
  loadGraphic(&sprites->walls[11], renderer, skin_wall11_bmp_gz, skin_wall11_bmp_gz_len);
  loadGraphic(&sprites->walls[12], renderer, skin_wall12_bmp_gz, skin_wall12_bmp_gz_len);
  loadGraphic(&sprites->walls[13], renderer, skin_wall13_bmp_gz, skin_wall13_bmp_gz_len);
  loadGraphic(&sprites->walls[14], renderer, skin_wall14_bmp_gz, skin_wall14_bmp_gz_len);
  loadGraphic(&sprites->walls[15], renderer, skin_wall15_bmp_gz, skin_wall15_bmp_gz_len);

  /* load wall caps */
  for (x = 0; x < 4; x++) sprites->wallcaps[x] = NULL;
  loadGraphic(&sprites->wallcaps[0], renderer, skin_wallcap0_bmp_gz, skin_wallcap0_bmp_gz_len);
  loadGraphic(&sprites->wallcaps[1], renderer, skin_wallcap1_bmp_gz, skin_wallcap1_bmp_gz_len);
  loadGraphic(&sprites->wallcaps[2], renderer, skin_wallcap2_bmp_gz, skin_wallcap2_bmp_gz_len);
  loadGraphic(&sprites->wallcaps[3], renderer, skin_wallcap3_bmp_gz, skin_wallcap3_bmp_gz_len);

  /* load font */
  for (x = 0; x < 128; x++) sprites->font[x] = NULL;
  loadGraphic(&sprites->font[char2fontid('0')], renderer, font_0_bmp_gz, font_0_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('1')], renderer, font_1_bmp_gz, font_1_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('2')], renderer, font_2_bmp_gz, font_2_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('3')], renderer, font_3_bmp_gz, font_3_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('4')], renderer, font_4_bmp_gz, font_4_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('5')], renderer, font_5_bmp_gz, font_5_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('6')], renderer, font_6_bmp_gz, font_6_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('7')], renderer, font_7_bmp_gz, font_7_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('8')], renderer, font_8_bmp_gz, font_8_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('9')], renderer, font_9_bmp_gz, font_9_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('a')], renderer, font_a_bmp_gz, font_a_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('b')], renderer, font_b_bmp_gz, font_b_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('c')], renderer, font_c_bmp_gz, font_c_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('d')], renderer, font_d_bmp_gz, font_d_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('e')], renderer, font_e_bmp_gz, font_e_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('f')], renderer, font_f_bmp_gz, font_f_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('g')], renderer, font_g_bmp_gz, font_g_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('h')], renderer, font_h_bmp_gz, font_h_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('i')], renderer, font_i_bmp_gz, font_i_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('j')], renderer, font_j_bmp_gz, font_j_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('k')], renderer, font_k_bmp_gz, font_k_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('l')], renderer, font_l_bmp_gz, font_l_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('m')], renderer, font_m_bmp_gz, font_m_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('n')], renderer, font_n_bmp_gz, font_n_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('o')], renderer, font_o_bmp_gz, font_o_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('p')], renderer, font_p_bmp_gz, font_p_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('q')], renderer, font_q_bmp_gz, font_q_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('r')], renderer, font_r_bmp_gz, font_r_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('s')], renderer, font_s_bmp_gz, font_s_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('t')], renderer, font_t_bmp_gz, font_t_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('u')], renderer, font_u_bmp_gz, font_u_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('v')], renderer, font_v_bmp_gz, font_v_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('w')], renderer, font_w_bmp_gz, font_w_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('x')], renderer, font_x_bmp_gz, font_x_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('y')], renderer, font_y_bmp_gz, font_y_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('z')], renderer, font_z_bmp_gz, font_z_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('A')], renderer, font_aa_bmp_gz, font_aa_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('B')], renderer, font_bb_bmp_gz, font_bb_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('C')], renderer, font_cc_bmp_gz, font_cc_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('D')], renderer, font_dd_bmp_gz, font_dd_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('E')], renderer, font_ee_bmp_gz, font_ee_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('F')], renderer, font_ff_bmp_gz, font_ff_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('G')], renderer, font_gg_bmp_gz, font_gg_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('H')], renderer, font_hh_bmp_gz, font_hh_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('I')], renderer, font_ii_bmp_gz, font_ii_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('J')], renderer, font_jj_bmp_gz, font_jj_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('K')], renderer, font_kk_bmp_gz, font_kk_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('L')], renderer, font_ll_bmp_gz, font_ll_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('M')], renderer, font_mm_bmp_gz, font_mm_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('N')], renderer, font_nn_bmp_gz, font_nn_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('O')], renderer, font_oo_bmp_gz, font_oo_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('P')], renderer, font_pp_bmp_gz, font_pp_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('Q')], renderer, font_qq_bmp_gz, font_qq_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('R')], renderer, font_rr_bmp_gz, font_rr_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('S')], renderer, font_ss_bmp_gz, font_ss_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('T')], renderer, font_tt_bmp_gz, font_tt_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('U')], renderer, font_uu_bmp_gz, font_uu_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('V')], renderer, font_vv_bmp_gz, font_vv_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('W')], renderer, font_ww_bmp_gz, font_ww_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('X')], renderer, font_xx_bmp_gz, font_xx_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('Y')], renderer, font_yy_bmp_gz, font_yy_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('Z')], renderer, font_zz_bmp_gz, font_zz_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid(':')], renderer, font_sym_col_bmp_gz, font_sym_col_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid(';')], renderer, font_sym_scol_bmp_gz, font_sym_scol_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('!')], renderer, font_sym_excl_bmp_gz, font_sym_excl_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('$')], renderer, font_sym_doll_bmp_gz, font_sym_doll_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('.')], renderer, font_sym_dot_bmp_gz, font_sym_dot_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('&')], renderer, font_sym_ampe_bmp_gz, font_sym_ampe_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('*')], renderer, font_sym_star_bmp_gz, font_sym_star_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid(',')], renderer, font_sym_comm_bmp_gz, font_sym_comm_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('(')], renderer, font_sym_par1_bmp_gz, font_sym_par1_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid(')')], renderer, font_sym_par2_bmp_gz, font_sym_par2_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('[')], renderer, font_sym_bra1_bmp_gz, font_sym_bra1_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid(']')], renderer, font_sym_bra2_bmp_gz, font_sym_bra2_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('-')], renderer, font_sym_minu_bmp_gz, font_sym_minu_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('_')], renderer, font_sym_unde_bmp_gz, font_sym_unde_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('/')], renderer, font_sym_slas_bmp_gz, font_sym_slas_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('"')], renderer, font_sym_quot_bmp_gz, font_sym_quot_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('#')], renderer, font_sym_hash_bmp_gz, font_sym_hash_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('@')], renderer, font_sym_at_bmp_gz, font_sym_at_bmp_gz_len);
  loadGraphic(&sprites->font[char2fontid('\'')], renderer, font_sym_apos_bmp_gz, font_sym_apos_bmp_gz_len);

  /* Hide the mouse cursor, disable mouse events and make sure DropEvents are enabled (sometimes they are not) */
  SDL_ShowCursor(SDL_DISABLE);
  SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
  SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

  /* parse the commandline */
  if (argc > 1) {
    int i;
    for (i = 1 ; i < argc ; i++) {
      if (strstr(argv[i], "--framedelay=") == argv[i]) {
          settings.framedelay = atoi(argv[i] + strlen("--framedelay="));
        } else if (strstr(argv[i], "--framefreq=") == argv[i]) {
          settings.framefreq = atoi(argv[i] + strlen("--framefreq="));
        } else if (levelfile == NULL) { /* else assume it is a level file */
          levelfile = strdup(argv[i]);
      }
    }
  }

  /* validate parameters */
  if ((settings.framedelay < 0) || (settings.framedelay > 64000)) settings.framedelay = 10500;
  if ((settings.framefreq < 1) || (settings.framefreq > 1000000)) settings.framefreq = 15000;

  gameslist = malloc(sizeof(struct sokgame *) * MAXLEVELS);
  if (gameslist == NULL) {
    puts("Memory allocation failed!");
    return(1);
  }

  states = sok_newstates();
  if (states == NULL) return(1);

  GametypeSelectMenu:
  if (levelslist != NULL) {
    free(levelslist);
    levelslist = NULL;
  }
  curlevel = -1;
  levelscount = -1;
  settings.tilesize = settings.nativetilesize;
  if (levelfile != NULL) goto LoadLevelFile;
  xsblevelptr = selectgametype(renderer, sprites, window, &settings, &levelfile, &xsblevelptrlen);
  levelsource = LEVEL_INTERNAL;
  if ((xsblevelptr != NULL) && (*xsblevelptr == '@')) levelsource = LEVEL_INTERNET;
  if (exitflag == 0) fade2texture(renderer, window, sprites->black);

  LoadInternetLevels:
  if (levelsource == LEVEL_INTERNET) { /* internet levels */
      int selectres;
      long httpres;
      httpres = http_get(INET_HOST, INET_PORT, INET_PATH, (unsigned char **) &levelslist);
      if ((httpres < 1) || (levelslist == NULL)) {
        SDL_RenderClear(renderer);
        draw_string("Failed to fetch internet levels!", 100, 255, sprites, renderer, DRAWSTRING_CENTER, DRAWSTRING_CENTER, window, 1, 0);
        wait_for_a_key(-1, renderer);
        goto GametypeSelectMenu;
      }
      selectres = selectinternetlevel(renderer, window, sprites, INET_HOST, INET_PORT, INET_PATH, levelslist, &xsblevelptr, &xsblevelptrlen);
      if (selectres == SELECTLEVEL_BACK) goto GametypeSelectMenu;
      if (selectres == SELECTLEVEL_QUIT) exitflag = 1;
      if (exitflag == 0) fade2texture(renderer, window, sprites->black);
    } else if ((xsblevelptr == NULL) && (levelfile == NULL)) { /* nothing */
      exitflag = 1;
  }

  LoadLevelFile:
  if ((levelfile != NULL) && (exitflag == 0)) {
      levelscount = sok_loadfile(gameslist, MAXLEVELS, levelfile, NULL, 0, levcomment, LEVCOMMENTMAXLEN);
    } else if (exitflag == 0) {
      levelscount = sok_loadfile(gameslist, MAXLEVELS, NULL, xsblevelptr, xsblevelptrlen, levcomment, LEVCOMMENTMAXLEN);
  }

  if ((levelscount < 1) && (exitflag == 0)) {
    SDL_RenderClear(renderer);
    printf("Failed to load the level file [%d]: %s\n", levelscount, sok_strerr(levelscount));
    draw_string("Failed to load the level file!", 100, 255, sprites, renderer, DRAWSTRING_CENTER, DRAWSTRING_CENTER, window, 1, 0);
    wait_for_a_key(-1, renderer);
    exitflag = 1;
  }

  /* printf("Loaded %d levels '%s'\n", levelscount, levcomment); */

  LevelSelectMenu:
  settings.tilesize = settings.nativetilesize;
  if (exitflag == 0) exitflag = flush_events();

  if (exitflag == 0) {
    curlevel = selectlevel(gameslist, sprites, renderer, window, &settings, levcomment, levelscount, curlevel, &levelfile);
    if (curlevel == SELECTLEVEL_BACK) {
        if (levelfile == NULL) {
            if (levelsource == LEVEL_INTERNET) goto LoadInternetLevels;
            goto GametypeSelectMenu;
          } else {
            exitflag = 1;
        }
      } else if (curlevel == SELECTLEVEL_QUIT) {
        exitflag = 1;
      } else if (curlevel == SELECTLEVEL_LOADFILE) {
        goto GametypeSelectMenu;
    }
  }
  if (exitflag == 0) fade2texture(renderer, window, sprites->black);
  if (exitflag == 0) loadlevel(&game, gameslist[curlevel], states);

  /* here we start the actual game */

  settings.tilesize = settings.nativetilesize;
  if ((curlevel == 0) && (game.solution == NULL)) showhelp = 1;
  playsolution = 0;
  drawscreenflags = 0;
  if (exitflag == 0) lastlevelleft = islevelthelastleft(gameslist, curlevel, levelscount);

  while (exitflag == 0) {
    if (playsolution > 0) {
        drawscreenflags |= DRAWSCREEN_PLAYBACK;
      } else {
        drawscreenflags &= ~DRAWSCREEN_PLAYBACK;
    }
    draw_screen(&game, states, sprites, renderer, window, &settings, 0, 0, 0, DRAWSCREEN_REFRESH | drawscreenflags, levcomment);
    if (showhelp != 0) {
      exitflag = displaytexture(renderer, sprites->help, window, -1, DISPLAYCENTERED, 255);
      draw_screen(&game, states, sprites, renderer, window, &settings, 0, 0, 0, DRAWSCREEN_REFRESH | drawscreenflags, levcomment);
      showhelp = 0;
    }
    if (debugmode != 0) printf("history: %s\n", states->history);

    /* Wait for an event - but ignore 'KEYUP' and 'MOUSEMOTION' events, since they are worthless in this game */
    for (;;) {
      if (SDL_WaitEventTimeout(&event, 80) == 0) {
        if (playsolution == 0) continue;
        event.type = SDL_KEYDOWN;
        event.key.keysym.sym = SDLK_F10;
      }
      if ((event.type != SDL_KEYUP) && (event.type != SDL_MOUSEMOTION)) break;
    }

    /* check what event we got */
    if (event.type == SDL_QUIT) {
        exitflag = 1;
      } else if (event.type == SDL_DROPFILE) {
        if (processDropFileEvent(&event, &levelfile) != NULL) {
          fade2texture(renderer, window, sprites->black);
          goto GametypeSelectMenu;
        }
      } else if (event.type == SDL_KEYDOWN) {
        int res = 0, movedir = 0;
        switch (normalizekeys(event.key.keysym.sym)) {
          case KEY_LEFT:
            movedir = sokmoveLEFT;
            break;
          case KEY_RIGHT:
            movedir = sokmoveRIGHT;
            break;
          case KEY_UP:
            movedir = sokmoveUP;
            break;
          case KEY_CTRL_UP:
            if (settings.tilesize < 255) settings.tilesize += 2;
            break;
          case KEY_DOWN:
            movedir = sokmoveDOWN;
            break;
          case KEY_CTRL_DOWN:
            if (settings.tilesize > 4) settings.tilesize -= 2;
            break;
          case KEY_BACKSPACE:
            if (playsolution == 0) sok_undo(&game, states);
            break;
          case KEY_R:
            playsolution = 0;
            loadlevel(&game, gameslist[curlevel], states);
            break;
          case KEY_F3: /* dump level & solution (if any) to clipboard */
            dumplevel2clipboard(gameslist[curlevel], gameslist[curlevel]->solution);
            exitflag = displaytexture(renderer, sprites->copiedtoclipboard, window, 2, DISPLAYCENTERED, 255);
            break;
          case KEY_CTRL_C:
            dumplevel2clipboard(&game, states->history);
            exitflag = displaytexture(renderer, sprites->snapshottoclipboard, window, 2, DISPLAYCENTERED, 255);
            break;
          case KEY_CTRL_V:
            {
            char *solFromClipboard;
            solFromClipboard = SDL_GetClipboardText();
            trimstr(solFromClipboard);
            if (isLegalSokoSolution(solFromClipboard) != 0) {
                loadlevel(&game, gameslist[curlevel], states);
                exitflag = displaytexture(renderer, sprites->playfromclipboard, window, 2, DISPLAYCENTERED, 255);
                playsolution = 1;
                if (playsource != NULL) free(playsource);
                playsource = unRLE(solFromClipboard);
                free(solFromClipboard);
              } else {
                if (solFromClipboard != NULL) free(solFromClipboard);
            }
            }
            break;
          case KEY_S:
            if (playsolution == 0) {
              if (game.solution != NULL) { /* only allow if there actually is a solution */
                  if (playsource != NULL) free(playsource);
                  playsource = unRLE(game.solution); /* I duplicate the solution string, because I want to free it later, since it can originate both from the game's solution as well as from a clipboard string */
                  if (playsource != NULL) {
                    loadlevel(&game, gameslist[curlevel], states);
                    playsolution = 1;
                  }
                } else {
                  exitflag = displaytexture(renderer, sprites->nosolution, window, 1, DISPLAYCENTERED, 255);
              }
            }
            break;
          case KEY_F1:
            if (playsolution == 0) showhelp = 1;
            break;
          case KEY_F2:
            if ((drawscreenflags & DRAWSCREEN_NOBG) && (drawscreenflags & DRAWSCREEN_NOTXT)) {
                drawscreenflags &= ~(DRAWSCREEN_NOBG | DRAWSCREEN_NOTXT);
              } else if (drawscreenflags & DRAWSCREEN_NOBG) {
                drawscreenflags |= DRAWSCREEN_NOTXT;
              } else if (drawscreenflags & DRAWSCREEN_NOTXT) {
                drawscreenflags &= ~DRAWSCREEN_NOTXT;
                drawscreenflags |= DRAWSCREEN_NOBG;
              } else {
                drawscreenflags |= DRAWSCREEN_NOTXT;
            }
            break;
          case KEY_F5:
            if (playsolution == 0) {
              exitflag = displaytexture(renderer, sprites->saved, window, 1, DISPLAYCENTERED, 255);
              solution_save(game.crc32, states->history, "sav");
            }
            break;
          case KEY_F7:
            {
            char *loadsol;
            loadsol = solution_load(game.crc32, "sav");
            if (loadsol == NULL) {
                exitflag = displaytexture(renderer, sprites->nosave, window, 1, DISPLAYCENTERED, 255);
              } else {
                exitflag = displaytexture(renderer, sprites->loaded, window, 1, DISPLAYCENTERED, 255);
                playsolution = 0;
                loadlevel(&game, gameslist[curlevel], states);
                sok_play(&game, states, loadsol);
                free(loadsol);
            }
            }
            break;
          case KEY_FULLSCREEN:
            switchfullscreen(window);
            break;
          case KEY_ESCAPE:
            fade2texture(renderer, window, sprites->black);
            goto LevelSelectMenu;
            break;
        }
        if (playsolution > 0) {
          movedir = 0;
          switch (playsource[playsolution - 1]) {
            case 'u':
            case 'U':
              movedir = sokmoveUP;
              break;
            case 'r':
            case 'R':
              movedir = sokmoveRIGHT;
              break;
            case 'd':
            case 'D':
              movedir = sokmoveDOWN;
              break;
            case 'l':
            case 'L':
              movedir = sokmoveLEFT;
              break;
          }
          playsolution += 1;
          if (playsource[playsolution - 1] == 0) playsolution = 0;
        }
        if (movedir != 0) {
          rotatePlayer(sprites, &game, states, movedir, renderer, window, &settings, levcomment, drawscreenflags);
          res = sok_move(&game, movedir, 1, states);
          if (res >= 0) { /* do animations */
            int offset, offsetx = 0, offsety = 0, scrolling;
            int refreshnow = 1;
            if (res & sokmove_pushed) drawscreenflags |= DRAWSCREEN_PUSH;
            /* How will I need to move? */
            if (movedir == sokmoveUP) offsety = -1;
            if (movedir == sokmoveRIGHT) offsetx = 1;
            if (movedir == sokmoveDOWN) offsety = 1;
            if (movedir == sokmoveLEFT) offsetx = -1;
            sokDelay(0 - settings.framefreq);  /* init my delay timer */
            /* Will I need to move the player, or the entire field? */
            for (offset = 0; offset != settings.tilesize * offsetx; offset += offsetx) {
              if (refreshnow) {
                scrolling = scrollneeded(&game, window, settings.tilesize, offsetx, offsety);
                draw_screen(&game, states, sprites, renderer, window, &settings, offset, 0, scrolling, DRAWSCREEN_REFRESH | drawscreenflags, levcomment);
              }
              refreshnow = sokDelay((settings.framedelay * 12) / settings.tilesize); /* wait a moment and check if it's time to refresh */
            }
            for (offset = 0; offset != settings.tilesize * offsety; offset += offsety) {
              if (refreshnow) {
                scrolling = scrollneeded(&game, window, settings.tilesize, offsetx, offsety);
                draw_screen(&game, states, sprites, renderer, window, &settings, 0, offset, scrolling, DRAWSCREEN_REFRESH | drawscreenflags, levcomment);
              }
              refreshnow = sokDelay((settings.framedelay * 12) / settings.tilesize); /* wait a moment and check if it's time to refresh */
            }
          }
          res = sok_move(&game, movedir, 0, states);
          if ((res >= 0) && (res & sokmove_solved)) {
            int alphaval;
            SDL_Texture *tmptex;
            /* display a congrats message */
            if (lastlevelleft != 0) {
                tmptex = sprites->congrats;
              } else {
                tmptex = sprites->cleared;
            }
            flush_events();
            for (alphaval = 0; alphaval < 255; alphaval += 30) {
              draw_screen(&game, states, sprites, renderer, window, &settings, 0, 0, 0, drawscreenflags, levcomment);
              exitflag = displaytexture(renderer, tmptex, window, 0, DISPLAYCENTERED, alphaval);
              SDL_Delay(25);
              if (exitflag != 0) break;
            }
            if (exitflag == 0) {
              draw_screen(&game, states, sprites, renderer, window, &settings, 0, 0, 0, drawscreenflags, levcomment);
              /* if this was the last level left, display a congrats screen */
              if (lastlevelleft != 0) {
                  exitflag = displaytexture(renderer, sprites->congrats, window, 10, DISPLAYCENTERED, 255);
                } else {
                  exitflag = displaytexture(renderer, sprites->cleared, window, 3, DISPLAYCENTERED, 255);
              }
              /* fade out to black */
              if (exitflag == 0) {
                fade2texture(renderer, window, sprites->black);
                exitflag = flush_events();
              }
            }
            /* load the new level and reset states */
            curlevel = -1; /* this will make the selectlevel() function to preselect the next level automatically */
            goto LevelSelectMenu;
          }
        }
        drawscreenflags &= ~DRAWSCREEN_PUSH;
    }

    if (exitflag != 0) break;
  }

  /* free the states struct */
  sok_freestates(states);

  if (levelfile != NULL) free(levelfile);

  /* free all textures */
  if (sprites->atom) SDL_DestroyTexture(sprites->atom);
  if (sprites->atom_on_goal) SDL_DestroyTexture(sprites->atom_on_goal);
  if (sprites->floor) SDL_DestroyTexture(sprites->floor);
  if (sprites->goal) SDL_DestroyTexture(sprites->goal);
  if (sprites->player) SDL_DestroyTexture(sprites->player);
  if (sprites->intro) SDL_DestroyTexture(sprites->intro);
  if (sprites->bg) SDL_DestroyTexture(sprites->bg);
  if (sprites->black) SDL_DestroyTexture(sprites->black);
  if (sprites->nosolution) SDL_DestroyTexture(sprites->nosolution);
  if (sprites->cleared) SDL_DestroyTexture(sprites->cleared);
  if (sprites->help) SDL_DestroyTexture(sprites->help);
  if (sprites->congrats) SDL_DestroyTexture(sprites->congrats);
  if (sprites->copiedtoclipboard) SDL_DestroyTexture(sprites->copiedtoclipboard);
  if (sprites->playfromclipboard) SDL_DestroyTexture(sprites->playfromclipboard);
  if (sprites->snapshottoclipboard) SDL_DestroyTexture(sprites->snapshottoclipboard);
  if (sprites->saved) SDL_DestroyTexture(sprites->saved);
  if (sprites->loaded) SDL_DestroyTexture(sprites->loaded);
  if (sprites->nosave) SDL_DestroyTexture(sprites->nosave);
  for (x = 0; x < 16; x++) if (sprites->walls[x]) SDL_DestroyTexture(sprites->walls[x]);
  for (x = 0; x < 4; x++) if (sprites->wallcaps[x]) SDL_DestroyTexture(sprites->wallcaps[x]);
  for (x = 0; x < 128; x++) if (sprites->font[x]) SDL_DestroyTexture(sprites->font[x]);

  /* clean up SDL */
  flush_events();
  SDL_DestroyWindow(window);
  SDL_Quit();

  return(0);
}
