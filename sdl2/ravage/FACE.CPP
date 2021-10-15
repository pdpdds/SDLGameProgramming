
/*-------------------------------------------------------*/
/* I N T E R F A C E                                     */
/*                                                       */
/* [c]copyright 1995 by AlphaHelix                       */
/*                                                       */
/*-------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DISKIO.HPP"
#include "ERROR.HPP"
#include "MEMORY.HPP"
#include "SDL_BASE.HPP"
#include "XLIB.HPP"
#include "INPUT.HPP"
#include "SOUND.HPP"
#include "RAVAGE.HPP"
#include "COMPATIBILITY.HPP"

/*---------------------------------------------------------
 Function:

 Description:
 Display Intro & Title screen.
---------------------------------------------------------*/
void title(void)
{
   void  *f;
   Font  *tiny;
   char  *text;
   int   x, y;
	int size = 0;

// Load needed font & picture.
   tiny = new Font("fonts\\tiny.fnt");
   f = loadfile("pic\\title.pcx", &size);

   text = (char *) loadfile("stuff\\reg.txt", NULL);

// Fade out and draw new screen.
   fadeout(FADE_PALETTE);
   showpcx(f, 20, size);

// Print text.
   x = 0; y = 140;
   beauty(tiny, x, y, text);

// Fade in
   fadein(NULL);

   //input_wait(); input_fire(); input_wait();
   SDL_Delay(1500);
   fadeout(FADE_PALETTE);

   //render.clear(0, 0, 0);

   unloadfile(text);
   unloadfile(f);

   delete tiny;
}

/*---------------------------------------------------------
 Function:

 Description:
 Custom MENU definitions.
---------------------------------------------------------*/
// Prototypes of custom menu functions.
static int a_main(int, int);
static int a_quit(int, int);
static int a_options(int, int);
static int a_stuff(int, int);
static int a_tooptions(int, int);
static int a_play1(int, int);
static int a_play2(int, int);
static int a_control(int, int);
static int a_key(int, int);
static int a_joy(int, int);
static int a_sound(int, int);
static int a_graphix(int, int);
static int a_load(int, int);
static int a_save(int, int);
static int a_shop(int, int);
// -

enum Control { END, HEAD, TEXT, STEXT, ADD, ADDSUB, YES, YESNO, UPDOWN, CUSTOM, NONE };

struct Menu {
   Control  type;                // Control Type.
   const char     *sname;              // File name of sprites.
   Sprite   *spr;                // Set to 0.
   int      x, y;                // Position.
   int      ys;                  // ?
   char     text[255];               // Text to display.
   Back     *back;               // Placeholder (set to 0)
   int      (*action)(int, int);
   int      fast;
};

/*
 The meaning of the parameters passed to the action function.
 index    : Which entry in the menu structure gets message.
 subindex : Which subpart of entry "index".
 Special values:
 index == M_INIT: Initialize menu.
 index == M_KEY: Key pressed message.
 index == M_JOYPAD : JOYPAD message.
*/

static Menu mainmenu[] = {
   { CUSTOM,"pic\\main.spr",0,-1,10,8,"",0,a_main,0 },
   { END,"",0,0,0,0,"",0,a_main,0 }
};

static Menu playmenu1[] = {
   { HEAD, "", 0,0,0,0, "NEW GAME",0,0,0 },
   { CUSTOM, "pic\\players.spr",0,-1,55,14,"",0,a_play1,0 },
   { END,"",0,0,0,0,"",0,a_play1,0 }
};

static Menu playmenu2[] = {
   { HEAD, "", 0,0,0,0, "2 PLAYERS",0,0,0 },
   { CUSTOM, "pic\\dif.spr",0,-1,35,14,"",0,a_play2,0 },
   { END,"",0,0,0,0,"",0,a_play2,0 }
};

static Menu quitmenu[] = {
   { YESNO , "", 0, 0, 0, 0, "QUIT",0,a_quit,0 },
   { END,    "", 0, 0, 0, 0, "",0,a_quit,0 }
};

static Menu optionmenu[] = {
   { HEAD, "", 0, 0, 0, 0, "OPTIONS",0,0,0 },
   { CUSTOM, "pic\\options.spr", 0, -1, 30, 8,"",0,a_options,0 },
   { YES, "", 0, 0, 0, 0, "Back To Main Menu",0,a_options,0 },
   { END, "", 0,0,0,0,"",0,a_options,0 }
};

static Menu stuffmenu[] = {
   { HEAD, "",0,0,0,0,"STUFF",0,0,0 },
   { CUSTOM, "pic\\stuff.spr",0,-1,30,8,"",0,a_stuff,0 },
   { YES, "",0,0,0,0,"Back To Main Menu",0,a_stuff,0 },
   { END, "",0,0,0,0,"",0,a_stuff,0 }
};

static Menu controlmenu[] = {
   { HEAD, "", 0,0,0,0,"CONTROLS",0,0,0 },
   { ADD, "",0,0,29,0,"",0,a_control,0 },
   { ADD, "",0,0,47,0,"",0,a_control,0 },
   { ADD, "",0,0,65,0,"",0,a_control,0 },
   { ADD, "",0,0,83,0,"",0,a_control,0 },
   { ADD, "",0,0,101,0,"",0,a_control,0 },
   { YES, "", 0, 0, 0, 0, "To Option Menu",0,a_tooptions,0 },
   { END, "",0,0,0,0,"",0,a_control,0 }
};

static Menu keymenu[] = {
   { HEAD, "", 0, 0, 0, 0, "KEYBOARD", 0,a_key,0 },
   { TEXT, "", 0, -1, 25, 12,"Press Key for Function",0,0,0 },
   { TEXT, "", 0, 5, 50, 15, "Left:",0,0,0 },
   { TEXT, "", 0, 5, 65, 15, "Right:",0,0,0 },
   { TEXT, "", 0, 5, 80, 15, "Up:",0,0,0 },
   { TEXT, "", 0, 5, 95, 15, "Down:",0,0,0 },
   { TEXT, "", 0, 5, 110, 15, "Fire:",0,0,0 },
   { END, "",0,0,0,0,"",0,a_key,0 }
};

/*static Menu joymenu[] = {
   { HEAD,"",0,0,0,0,"JOYPAD",0,a_joy,0 },
   { TEXT,"",0,-1,60,15,"Center Pad and Press Fire.",0,0,0 },
   { TEXT,"",0,-1,70,12,"ESC aborts.",0,0,0 },
   { END,"",0,0,0,0,"",0,a_joy,0 }
};*/

static Menu joymenu[] = {
   { HEAD, "", 0, 0, 0, 0, "JOYPAD", 0,a_joy,0 },
   { TEXT, "", 0, -1, 25, 12,"Press Button for Function",0,0,0 },
   { TEXT, "", 0, 5, 50, 15, "Left:",0,0,0 },
   { TEXT, "", 0, 5, 65, 15, "Right:",0,0,0 },
   { TEXT, "", 0, 5, 80, 15, "Up:",0,0,0 },
   { TEXT, "", 0, 5, 95, 15, "Down:",0,0,0 },
   { TEXT, "", 0, 5, 110, 15, "Fire:",0,0,0 },
   { END, "",0,0,0,0,"",0,a_joy,0 }
};


static Menu soundmenu[] = {
   { HEAD, "", 0,0,0,0,"SOUND",0,0,0 },
   { ADDSUB, "",0,0,40,0,"",0,a_sound,1 },
   { ADDSUB, "",0,0,60,0,"",0,a_sound,1 },
   { ADD, "",0,0,80,0,"",0,a_sound,0 },
   { NONE, "",0,0,100,0,"",0,a_sound,1 },
   { YES, "", 0, 0, 0, 0, "To Option Menu",0,a_tooptions,0 },
   { END, "",0,0,0,0,"",0,a_sound,0 }
};

static Menu graphixmenu[] = {
   { HEAD, "", 0,0,0,0,"GRAPHICS",0,0,0 },
   { ADD, "",0,0,35,0,"",0,a_graphix,0 },
   { ADD, "",0,0,55,0,"",0,a_graphix,0 },
//   { ADD, "",0,0,75,0,"",0,a_graphix,0 },
   { YES, "", 0, 0, 0, 0, "To Option Menu",0,a_tooptions,0 },
   { END, "",0,0,0,0,"",0,a_graphix,0 }
};


static Menu loadmenu[] = {
   { HEAD, "",0,0,0,0,"LOAD GAME",0,0,0 },
   { TEXT, "",0,-1,25,12,"Select Game to Load.",0,0,0 },
   { ADD,  "",0,0,50,0,"",0,a_load,0 },
   { ADD,  "",0,0,73,0,"",0,a_load,0 },
   { ADD,  "",0,0,96,0,"",0,a_load,0 },
   { YES, "",0,0,0,0, "Back to Main Menu",0,a_load,0 },
   { END, "",0,0,0,0,"",0,a_load,0 }
};

static Menu savemenu[] = {
   { HEAD, "",0,0,0,0,"SAVE GAME",0,0,0 },
   { TEXT, "",0,-1,25,12,"Select Slot to Save Game to.",0,0,0 },
   { ADD,  "",0,0,50,0,"",0,a_save,0 },
   { ADD,  "",0,0,73,0,"",0,a_save,0 },
   { ADD,  "",0,0,96,0,"",0,a_save,0 },
   { YES, "",0,0,0,0, "Back to Shop",0,a_save,0 },
   { END, "",0,0,0,0,"",0,a_save,0 }
};

static Menu shopmenu[] = {
   { HEAD, "",0,0,0,0, "PLAYER 1",0,0,0 },  // Must be element 0.
   { STEXT,"",0,0,22,0,"",0,0,0 },
   { ADDSUB, "",0,0,37,0,"",0,a_shop,0 },
   { ADDSUB, "",0,0,59,0,"",0,a_shop,0 },
   { ADDSUB, "",0,0,81,0,"",0,a_shop,0 },
   { UPDOWN, "",0,0,103,0,"",0,a_shop,0 },
   { CUSTOM, "pic\\save.spr",0,16,103,0,"",0,a_shop,0 },
   { YES, "",0,0,0,0, "Play Next Level",0,a_shop,0 },
   { END, "",0,0,0,0,"",0,a_shop,0 }
};

/*---------------------------------------------------------
 Function Group: Menu Functions

 Description:
 These are functions to build and manage the various MENUs.
 A menu is made of an array of Menu structures.
 Each entry in the array defines another button, text or
 whatsoever. A custom function is linked to all the buttons
 which is being executed on a button PRESS event.
---------------------------------------------------------*/
#define DEMOX           0
#define DEMOY           0
#define DEMOXS          144        // x size of demo screen
#define DEMOYS          YMAX        //
#define MENUX           (DEMOX+DEMOXS+6)
#define MENUY           90
#define MENUXS          (XMAX-MENUX)
#define MENUYS          (YMAX-MENUY)
#define MENUX1          (MENUX+MENUXS-1)
#define MENUY1          (MENUY+MENUYS-1)

// Events.
#define M_INIT          99       // Initialize menu.
#define M_KEY           98       // Key has been pressed.
#define M_JOYPAD        97       // Joystick event.

static int     nowplayer;        // Active player for current menu.
static GameState     gsave;
static PlayerState   psave;      // State save for active player.
static PlayerState   psave2;     // State save for player not active in DEMO.
static int     cheatsave;        // Back up variable for cheatvalue.

static Back    *menuback;        // Saved menu screen back ground.
static SDLTexture demoTexture;	// The texture the game demo is rendered to.
static Sprite  *head;            // Part of Title.
static SDLSprite sHead;
static Sprite  *pointer;         // Mouse pointer.
static SDLSprite sPointer;
static Back    *back;            // Pointer background.
static Font    *blue;            // Big Blue font.
static Font    *tiny;            // Tiny font. Used everywhere.
static Sprite  *addsub;          // Add/Sub buttons.
static SDLSprite sAddSub;
static Sprite  *yesno;           // Yes/No buttons.
static SDLSprite sYesNo;
static Menu    *nowmenu;         // Which menu is active now.

// Backoff menu. This menu will be built, if user presses ESC.
static Menu    *backoff;
static int     pointer_enabled;  // RW: 1 if menu cursor is disabled.
static int     keys_enabled;     // RW: 1 if key events are welcome.
static int     joy_enabled;      // RW: 1 if joypad event are enabled.


//-------------------------------------------------------------------
// Button stuff.
//-------------------------------------------------------------------
// Button definition structure.
class Button {
   int      x, y;       // x,y of button.
   int      xs, ys;
   Sprite   *spr;       // Sprite representing this button.
	SDLSprite sSpr;
   int      n;          // Sprite frame used to display released button.
   int      index;      // Return this value if button was pressed.
   Button   *next;
public:
   Button(int dx, int dy, Sprite *sprite, int dn, int idx);
   void     up(void) { render.x_drawsprite(&sSpr, x, y, n); }
   void     down(void) { render.x_drawsprite(&sSpr, x, y, n + 1); }
   friend   Button *checkbuttons(int x0, int y0, int &index, int &subindex);
   friend   void destroybuttons(void);
	friend	void clearmenu(void);
};
static Button  *anchor = 0; //Head of a list of buttons

Button::Button(int dx, int dy, Sprite *sprite, int dn, int idx)
{
   x = dx; y = dy;
   spr = sprite;
	sSpr.load(sprite, false);
   xs = spr->xs; ys = spr->ys;
   n = dn*2;
   index = idx;

	up();

   if (anchor) next = anchor; else next = 0;
   anchor = this;
}




void destroybuttons(void)
{
   Button   *keep;

   while (anchor) {
      keep = anchor->next;
		anchor->sSpr.destroy();
      delete anchor;
      anchor = keep;
   }
}

// Check mouse coordinates and return number of button mouse hits.
Button *checkbuttons(int x0, int y0, int &index, int &subindex)
{
   Button   *b = anchor;
   int      dx, dy;

   if (!b) {
      index = -1;
      return 0;
   }

   do {
      dx = x0 - b->x; dy = y0 - b->y;
      if ((dx >= 0) && (dy >= 0)) {
         if ((dx < b->xs) && (dy < b->ys)) {
            index = b->index; subindex = b->n / 2;
            return b;
         }
      }
   } while ((b = b->next));
   index = -1;
   return 0;
}



/*---------------------------------------------------------
 Function group: MENUS

 Description:
 Build the MENU screen.
 Play game demo & ask player: What's next ?
---------------------------------------------------------*/
static void addtext1(Menu &menu, const char *text1, const char *text2)
{
   int   x, y;

   menu.back->restore();
   x = MENUX + 2;
   y = MENUY + menu.y + (addsub->ys-tiny->height()) / 2;
   tiny->vanilla(x, y, text1, 15);
   x += tiny->textlen(text1);
   tiny->vanilla(x, y, text2, 12);
}

static void addtext2(Menu &menu, const char *text1, const char *text2)
{
   int   x, y;

   menu.back->restore();
   x = MENUX + 2;
   y = MENUY + menu.y + (addsub->ys)/2 - tiny->height() - 1;
   tiny->vanilla(x, y, text1, 15);
   y += tiny->height() + 1;
   tiny->vanilla(x, y, text2, 12);
}

static void addtext1dn(int x, Menu &menu, const char *text1, int color)
{
   int   y;

   y = MENUY + menu.y + (addsub->ys)/2;
   tiny->vanilla(x+MENUX, y, text1, color);
}

static void destroymenu(void)
{
   if (!nowmenu) return;
   destroybuttons();
   while (nowmenu->type != END) {
      if (nowmenu->spr) unloadfile(nowmenu->spr);
      if (nowmenu->back) delete nowmenu->back;
      nowmenu++;
   }
   menuback->restore();
}

void clearmenu(void)
{
	void  *f;
	int size = 0;
	Button   *b = anchor;

	f = loadfile("pic\\themenu.pcx", &size);
	showpcx(f, 0, size);
   unloadfile(f);

   if (!b) {
      return;
   }

   do {
			b->up();
   } while ((b = b->next));

	return;
}


static void buildmenu(Menu *menu)
{
   int      index;
   int      x, y;
   int      i, n;

   destroymenu();
   nowmenu = menu;

// Set event handling to default.
   pointer_enabled = 1;
   keys_enabled = 0;
   joy_enabled = 0;

   index = 0;
   do {
      switch (menu->type) {
      case HEAD:
         blue->print_c(MENUX+(MENUXS+BORDER)/2, MENUY+menu->y, menu->text);
         break;
      case TEXT:
         if (menu->x == -1)
            tiny->vanilla_c(MENUX+(MENUXS+BORDER)/2, MENUY + menu->y, menu->text, menu->ys);
         else
            tiny->vanilla(MENUX + menu->x, MENUY + menu->y, menu->text, menu->ys);
         break;
      case STEXT:
         y = tiny->height() + 1;
         menu->back = new Back(MENUX1-7-MENUX, y);
         menu->back->save(MENUX, MENUY+menu->y);
         break;
      case ADD:
         y = tiny->height()*2 + 1;
         menu->back = new Back(MENUX1-23-MENUX, y);
         y = (addsub->ys - y) / 2;
         new Button(MENUX1-21, MENUY+menu->y, addsub, 0, index);
			anchor->up();
         menu->back->save(MENUX, MENUY+menu->y+y-1);
			anchor->up();
         break;
      case ADDSUB:
         y = tiny->height()*2 + 1;
         menu->back = new Back(MENUX1-27-MENUX, y);
         y = (addsub->ys - y) / 2;
         new Button(MENUX1-45, MENUY+menu->y, addsub, 0, index);
         new Button(MENUX1-21, MENUY+menu->y, addsub, 1, index);
         menu->back->save(MENUX, MENUY+menu->y+y-1);
         break;
      case YESNO:
         blue->print_c(MENUX+(MENUXS+BORDER)/2, 180, menu->text);
         new Button(MENUX+20, 205, yesno, 0, index);
         new Button(XMAX-10-yesno->xs, 205, yesno, 1, index);
			anchor->up();
         break;
      case YES:
         new Button(XMAX-yesno->xs, 215, yesno, 0, index);
         tiny->vanilla(XMAX-yesno->xs-tiny->textlen(menu->text)-10,
                       215+(yesno->ys-tiny->height())/2,
                       menu->text, 15);
         break;
      case UPDOWN:
         menu->spr = (Sprite *)loadfile("pic\\updown.spr", NULL);
         new Button(242, MENUY+menu->y, menu->spr, 0, index);
         new Button(266, MENUY+menu->y, menu->spr, 1, index);
         break;
      case CUSTOM:
         menu->spr = (Sprite *)loadfile(menu->sname, NULL);
         n = menu->spr->frames / 2;
         if (menu->x == -1)
            x = MENUX + (MENUXS+BORDER-menu->spr->xs)/2;
         else
            x = MENUX + menu->x;
         y = MENUY + menu->y;

         for (i = 0; i < n; i++) {
            new Button(x, y, menu->spr, i, index);
            y += menu->ys + menu->spr->ys;
				anchor->up();
         }
         break;
      case END:
         menu->action(M_INIT, 0);
         break;
		case NONE:
			break;
      }
      if (menu->type == END) break;
      menu++; index++;
   } while (1);

   back->invalidate();
}


/*---------------------------------------------------------
 Function:

 Description:
 Operate menu.
 Check if mouse is pressing a button and execute
 "action function" on a press.
 Return values:
 0 : everything is allright. Go ahead.
 1 : play game
 2 : quit game
---------------------------------------------------------*/
int checkmenu(int mb, int x, int y)
{
   static   Button *last;
   Button   *now;
   static   int lindex = 0;
   static   int lsindex = 0;
   int      index = 0, subindex = 0;
   int      ret;

// Set up.
   if (mb == -1) {
      last = 0;
      lindex = -1;
      ret = 1;
      return -1;
   }

   ret = 0;

   if (mb) {
      now = checkbuttons(x, y, index, subindex);
   } else {
      now = 0;
      index = -1;
   }
   if (last) last->up();
   if (now) now->down();
   last = now;

   if (lindex != -1) if (nowmenu[lindex].action) {
      if (nowmenu[lindex].fast) ret = nowmenu[lindex].action(lindex, lsindex);
      else if (!mb) {
         ret = nowmenu[lindex].action(lindex, lsindex);
      }
   }

   lindex = index;
   lsindex = subindex;
   return ret;
}


//-------------------------------------------------------------------
// Play one frame of the DEMO GAME.
void demo(void)
{
   int  dx, dy, fire;

   newwave();
   enemystep();
	s_update();
   player[nowplayer].control(dx, dy, fire);
   weaponstep(nowplayer, dx * SHIPSPEED, dy * SHIPSPEED, fire);
   crash(nowplayer);
   if (gstate.frame < layerlength(1)) {
      gstate.frame++;
      movelayer(0, gstate.frame & 1);
      movelayer(1, 1);
   } else {
      movelayer(0, 0); movelayer(1, 0);
   }

	//Draw game to demo texture
	render.setTarget(&demoTexture);

   process_objects();
   drawscreen();
   postprocess_objects();

	render.setTarget(NULL);
}

static void initmenu(void)
{
// Allocate Menu Background backup.
   menuback = new Back(MENUXS, MENUYS);
// Load menu screen.

   demoTexture.loadPointer(SDL_CreateTexture(render.getBase(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, HWXMAX, YMAX), true);
	clearmenu();

// Take a snapshot of the plain menu screen.
   menuback->save(MENUX, MENUY);
// .. freeze overlapping part of Title.
   head = (Sprite *)loadfile("pic\\ravage.spr", NULL);
	sHead.load(head, false);

// Load pointer.
   pointer = (Sprite *)loadfile("pic\\pointer.spr", NULL);
	sPointer.load(pointer, false);

// Set mouse area.
   //setmouselimits(MENUX, MENUY, MENUX+MENUXS-pointer->xs, MENUY+MENUYS-pointer->ys);
    setmouselimits(-BORDER, 0, 320, 240);
// Initialize POINTER animation buffer.
   back = new Back(pointer->xs, pointer->ys);
// Set events to initial values.
   pointer_enabled = 1;
   keys_enabled = 0;
   joy_enabled = 0;

// Several buttons & fonts.
   blue = new Font("fonts\\blue.fnt");
   tiny = new Font("fonts\\tiny.fnt");

   addsub = (Sprite *)loadfile("pic\\addsub.spr", NULL);
   sAddSub.load(addsub, false);

   yesno = (Sprite *)loadfile("pic\\yesno.spr", NULL);
   sYesNo.load(yesno, false);

// Write version information.
#ifdef SHAREWARE
   tiny->vanilla(215, 65, "shareware 1.1", 10);
#else
   tiny->vanilla(215, 65, "registered 1.1", 10);
#endif

// Save current game state in case the menu is in SHOP mode.
   gsave = gstate;
   psave = player[nowplayer];
   psave2 = player[1-nowplayer];
   player[1-nowplayer].active = 0;     // Deactivate unused player.

// Initialize DEMO GAME.
   cheatsave = cheatlevel;
   cheatlevel |= CHEAT_INVUL | CHEAT_NOMONEY;
   playback_start("demo1\\demo1.rec");
   player[nowplayer].control = playback_device;
   gstate.nplayers = 1; gstate.difficulty = 3;
   newgame(1);
   newlevel("demo1\\demo1");
   weapon_releaseall(nowplayer, STARTX1, STARTY);
// Start playing background music.
   s_loadmod("mods\\menu.uni");
   s_startmod();
// Fade In
   fadein(level.palette);
}

static void shutmenu(void)
{
   fadeout(FADE_ALL);
   s_unloadmod();

// Housekeeping.
   weapon_removeall(nowplayer);

   shutlevel();
   shutgame();
   playback_stop();
   cheatlevel = cheatsave;
// Restore Previous Saved Game State.
   gstate = gsave;
   player[nowplayer] = psave;
   player[1-nowplayer] = psave2;

// release resources.
   unloadfile(yesno);
	sYesNo.destroy();

   unloadfile(addsub);
	sAddSub.destroy();

   delete tiny;
   delete blue;

   delete back;

   unloadfile(pointer);
	sPointer.destroy();

   unloadfile(head);
	sHead.destroy();

	demoTexture.destroy();

   delete menuback;
}

static void testlimits(int &x, int &y)
{
   if (x < -BORDER) x = -BORDER;
   if (x > HWXMAX-pointer->xs) x = HWXMAX-pointer->xs;
   if (y < 0) y = 0;
   if (y > HWYMAX+BORDER-pointer->ys) y = HWYMAX+BORDER-pointer->ys;
}


/*---------------------------------------------------------
 Function: menu

 Description:
 Build and manage menu.
 mode : Menu mode (MAINMENU or SHOPMENU).
 p    : Build menu for PLAYER p. (Especially important for SHOPMENU)
---------------------------------------------------------*/
int menu(int mode, int p)
{
   int      dx, dy, db;    // Accumulated cursor coordinates.
	int		mb;
   int      joy_x, joy_y, joy_fire; // Joystick values.
   int      x, y, b;       // Mouse coordinates.
   int      request;
   static   int key_allowed;

   Uint32 startTime = 0;

// Initialize menu system.
   nowplayer = p;
   initmenu();
   nowmenu = 0;            // No menu active at the moment.
   strcpy(shopmenu[0].text, p ? "PLAYER 2" : "PLAYER 1");
   switch (mode) {
      case MAINMENU:  buildmenu(mainmenu); break;
      case SOUNDMENU: buildmenu(soundmenu); break;
      case SHOPMENU:  buildmenu(shopmenu); break;
   }

// Initialize some variables.
   joy_x = joy_y = joy_fire = 0;

// Initialize "checkmenu".
   checkmenu(-1, 0, 0);

   x = MENUX + MENUXS/2;
   y = MENUY + MENUYS/2;
   do {
      startTime = SDL_GetTicks();
      demo();

// Restore mouse pointer background.
      dx = dy = mb = b = 0;
      input_update();

      if (joypad) {
         getjoypad(joy_x, joy_y, joy_fire);
         x += joy_x*4; y += joy_y*4; b += joy_fire;
      }
      getkeyboard(dx, dy, db); x += dx*4; y += dy*4; b += db;

      if (mouse && !(dx || dy)) {
            getmouse(dx, dy, mb);

            if((dx || dy))
                getmousexy(mb, x, y);
				
				b += mb;
      }

      testlimits(x, y);
      //if (mouse) setmousexy(x, y);
        back->restore();
// Process events.
         if (!keys) key_allowed = 1;
         if (!keys_enabled) key_released = 0;
// Send KEY PRESSED message to menu.
         if (!keys && key_released && key_released!=KEY_ESC) {
            if (nowmenu[0].action) nowmenu[0].action(M_KEY, key_released);
            key_released = 0;
         }
// Send joypad messages.
         if (joy_enabled && button_released != -1) {
            if (nowmenu[0].action) nowmenu[0].action(M_JOYPAD, button_released);
         }

// Send mouse messages.
         if (pointer_enabled) {
            if (request = checkmenu(b, x, y)) break;
         }
// Build backoff menu if ESC has been pressed.
         if (key_allowed && key[KEY_ESC] && backoff) {
            buildmenu(backoff);
            key_allowed = 0;
				escape_wait();
         }


         back->save(x, y);

// Copy demo display to restricted area of screen
        render.drawTexture(&demoTexture, BORDER, 0, 16, 0, 4*32 + 16, YMAX);
// Restore title logo.
        render.x_drawsprite(&sHead, 100, 12, 0);

        if (pointer_enabled) render.x_drawsprite(&sPointer, x, y, 0);

      render.update();

        while(SDL_GetTicks() - startTime < 32)
        {
            SDL_Delay(1);
        }

   } while (!quit_requested);

   destroymenu();
// Shut down menu system.
   shutmenu();
   return quit_requested ? M_QUIT : request;
}


/*---------------------------------------------------------
 Function:

 Description:
---------------------------------------------------------*/
// Custom stuff.

//#pragma off (unreferenced)
static const char *stext[3] = { "None", "Some", "All" };
static const char *mtext[3] = { "Low", "Medium", "High" };
static const char *screenText[4] = { "Standard", "Fuzzy", "Warm", "Warm and Fuzzy" };
static int  sfreq_now;
static int  sfreq[4] = { 11025, 22050, 44100, 99999 };
static int  nplayers;


static void showpicture(char *file)
{
   void  *f;
	int size = 0;
	SDLTexture *restore;

	restore = render.createScreenTexture();

   f = loadfile(file, &size);
   fadeout(FADE_PALETTE);
   render.clear(0, 0, 0);
   showpcx(f, 0, size);
   unloadfile(f);
   fadein(NULL);
   input_wait(); input_fire(); input_wait();
   fadeout(FADE_PALETTE);

	render.drawTexture(restore, 0, 0);

   fadein(level.palette);

	restore->destroy();
	delete restore;
}

static void order(void)
{
   void  *f;
   char  *text;
   int   x, y;
	int size = 0;
	SDLTexture *restore = NULL;

	restore = render.createScreenTexture();

   f = loadfile("pic\\order.pcx", &size);

   text = (char *)loadfile("stuff\\order1.txt", NULL);

   fadeout(FADE_PALETTE);
   render.clear(0, 0, 0);
   showpcx(f, 20, size);
   blue->print_c(XMAX/2, 0, "ORDER RAVAGE NOW");
   x = 0; y = 30;
   beauty(tiny, x, y, text);
   unloadfile(text);

   fadein(NULL);

   input_wait(); input_fire(); input_wait();
   fadeout(FADE_PALETTE);

   text = (char *)loadfile("stuff\\order2.txt", NULL);

   render.clear(0, 0, 0);
   showpcx(f, 20, size);
   blue->print_c(XMAX/2, 0, "HOW TO ORDER");
   x = 0; y = 30;
   beauty(tiny, x, y, text);
   unloadfile(text);

   fadein(NULL);

   unloadfile(f);

   input_wait(); input_fire(); input_wait();
   fadeout(FADE_PALETTE);

	render.drawTexture(restore, 0, 0);
   fadein(level.palette);

	restore->destroy();
	delete restore;
}

static int a_main(int index, int subindex)
{

// Initialize menu.
   if (index == M_INIT) {
      backoff = quitmenu;
      return 0;
   }

   switch (subindex) {
   case 0:  // Play
      buildmenu(playmenu1);
      break;
   case 1:  // Load game
      buildmenu(loadmenu);
      break;
   case 2:  // Option menu
      buildmenu(optionmenu);
      break;
   case 3:  // Stuff
      buildmenu(stuffmenu);
      break;
   case 4:  // Quit already ?
      buildmenu(quitmenu);
      break;
   }
   return 0;
}


static int a_quit(int index, int subindex)
{
// Initialize menu.
   if (index == M_INIT) {
      backoff = mainmenu;
      return 0;
   }

// Action part.
   if (subindex==1) {
      buildmenu(mainmenu);
      return 0;
   } else {
      return M_QUIT;
   }
}

static int a_options(int index, int subindex)
{

   if (index == M_INIT) {
      backoff = mainmenu;
      return 0;
   }

   switch (index) {
   case 1:
      switch (subindex) {
      case 0:    // Control menu
         buildmenu(controlmenu);
         break;
      case 1:     // Sound menu
         buildmenu(soundmenu);
         break;
      case 2:     // graphix menu
         buildmenu(graphixmenu);
         break;
      }
      break;
   case 2:
      buildmenu(mainmenu);
      break;
   }
   return 0;
}

static int a_stuff(int index, int subindex)
{
   if (index == M_INIT) {
      backoff = mainmenu;
      return 0;
   }

   switch (index) {
   case 1:
      switch (subindex) {
      case 0:     // Something about rescuing the girl and saving the world.
         showpicture("pic\\story.pcx");
         break;
      case 1:     // How to order the game.
         order();
         break;
      case 2:     // Credits.
         showpicture("pic\\credit0.pcx");
         break;
      }
      break;
   case 2:
      buildmenu(mainmenu);
      break;
   }
   return 0;
}

static int a_tooptions(int index, int subindex)
{
   buildmenu(optionmenu);
   return 0;
}

static void b_control(int n)
{
   switch (n) {
   case 1:
      addtext1(controlmenu[n], "Player 1: ", getdevicename(settings.device[0]));
      break;
   case 2:
      addtext1(controlmenu[n], "Player 2: ", getdevicename(settings.device[1]));
      break;
   case 3:
      addtext1(controlmenu[n], "Redefine Keys", "");
      break;
   case 4:
		addtext1(controlmenu[n], "Mouse Sensitivity: ", mtext[settings.msens]);
      break;
   case 5:
		addtext1(controlmenu[n], "Joypad Buttons", "");
      break;
   }
}

static int a_control(int index, int subindex)
{
   if (index == M_INIT) {
      b_control(1);
      b_control(2);
      b_control(3);
      b_control(4);
      b_control(5);
      backoff = optionmenu;
      return 0;
   }

   switch (index) {
   case 1:
      unlockdevice(settings.device[0]);
      settings.device[0] = nextdevice(settings.device[0]);
      lockdevice(settings.device[0]);
      b_control(1);
      break;
   case 2:
      unlockdevice(settings.device[1]);
      settings.device[1] = nextdevice(settings.device[1]);
      lockdevice(settings.device[1]);
      b_control(2);
      break;
   case 3:     // Define keys.
      buildmenu(keymenu);
      break;

   case 4:
      if (++settings.msens >= 3) settings.msens = 0;
      setmousesens(settings.msens);
      b_control(4);
      break;
	case 5:     // Define joypad buttons.
		if(joypad)
		{
			buildmenu(joymenu);
		}
      break;
   }
   return 0;
}

static int a_key(int index, int subindex)
{
   static int  k;
   char   text[16];

   if (index == M_INIT) {
      k = 0;      // First key to enter now.
      pointer_enabled = 0;
      keys_enabled = 1;
      backoff = controlmenu;
      return 0;
   }

   if (k < 5 && subindex != SDL_SCANCODE_ESCAPE) {
		strncpy(text, SDL_GetScancodeName((SDL_Scancode)subindex), 15);
      tiny->vanilla(MENUX+80, MENUY+nowmenu[k+2].y, text, 12);
      settings.key[k] = subindex;
      if (k == 4) {
         tiny->vanilla_c(MENUX+(MENUXS+BORDER)/2,
                         MENUY+140, "Press any key to continue", 12);
      }
   }
   if (k==5) {
      setkeys(settings.key[0], settings.key[1], settings.key[2],
              settings.key[3], settings.key[4]);
      buildmenu(controlmenu);
   }
   k++;
   return 0;
}

static int a_joy(int index, int subindex)
{
   static int  k;
   char   text[16];

   if (index == M_INIT) {
      k = 0;      // First button to enter now.
      pointer_enabled = 0;
      joy_enabled = 1;
      backoff = controlmenu;
      return 0;
   }

   if (k < 5) {
        snprintf(text, 10, "%d", subindex);
		//itoa(subindex, text, 10);
		//strncpy(text, SDL_GetScancodeName((SDL_Scancode)subindex), 15);
      tiny->vanilla(MENUX+80, MENUY+nowmenu[k+2].y, text, 12);
      settings.joypadbutton[k] = subindex;
      if (k == 4) {
         tiny->vanilla_c(MENUX+(MENUXS+BORDER)/2,
                         MENUY+140, "Press any button to continue", 12);
      }
   }
   if (k==5) {
      setbuttons(settings.joypadbutton[0], settings.joypadbutton[1], settings.joypadbutton[2],
              settings.joypadbutton[3], settings.joypadbutton[4]);
      buildmenu(controlmenu);
   }
   k++;
   return 0;
}

static void b_sound(int n)
{
   char  text[20];

   switch (n) {
   /*case 1:
      itoa(settings.voices, text, 10);
      addtext1(soundmenu[n], "SFx Voices: ", text);
      break;
   case 2:
      itoa(settings.mix_freq/1000, text, 10);
      strcat(text, "kHz");
      addtext1(soundmenu[n], "Mixing Frequency: ", text);
      break;
	*/
   case 1:
      if (settings.mod_vol) {
        snprintf(text, 16, "%d", settings.mod_vol);
         //itoa(settings.mod_vol, text, 10); strcat(text, "/64");
      } else {
         strcpy(text, "off");
      }
      addtext1(soundmenu[n], "Music Volume: ", text);
      break;
   case 2:
      if (settings.wav_vol) {
         snprintf(text, 16, "%d", settings.wav_vol);
         //itoa(settings.wav_vol, text, 10); strcat(text, "/64");
      } else {
         strcpy(text, "off");
      }
      addtext1(soundmenu[n], "SFx Volume: ", text);
      break;
	case 3:
		if (settings.loop_music) {
			strcpy(text, "on");
      } else {
         strcpy(text, "off");
      }
      addtext1(soundmenu[n], "Loop Music: ", text);
   }
}

static int a_sound(int index, int subindex)
{
   int   i;

   if (index == M_INIT) {
      b_sound(1);
      b_sound(2);
      b_sound(3);
      //b_sound(4);

      backoff = optionmenu;
      return 0;
   }

   switch (index) {
   /*case 1:     // WAV voices.
      if ((subindex == 0) && (settings.voices < 6)) settings.voices++;
      if ((subindex == 1) && (settings.voices > 1)) settings.voices--;
      //s_setvoices(settings.voices);
      b_sound(index);
      break;
   case 2:     // Mixing frequency.
      if (++sfreq_now > 2) sfreq_now = 0;
      settings.mix_freq = sfreq[sfreq_now];
      return M_SOUND;
	*/
   case 1:     // MOD volume.
      if ((subindex == 0) && (settings.mod_vol < 64)) settings.mod_vol++;
      if ((subindex == 1) && (settings.mod_vol > 0)) settings.mod_vol--;
      s_setmodvol(settings.mod_vol);
      b_sound(index);
      break;
   case 2:     // WAV volume.
      if ((subindex == 0) && (settings.wav_vol < 64)) settings.wav_vol++;
      if ((subindex == 1) && (settings.wav_vol > 0)) settings.wav_vol--;
      s_setwavvol(settings.wav_vol);
      b_sound(index);
      break;

	case 3:     // Loop Music
      settings.loop_music = settings.loop_music ? 0 : 1;
		s_startmod();
		input_wait();
      b_sound(index);

      break;
   }

   return 0;
}


static void b_graphix(int n)
{
   switch (n) {

   case 1:
      addtext1(graphixmenu[n], "Shadows: ", stext[settings.shadows]);
      break;
	case 2:
      addtext1(graphixmenu[n], "Scaling: ", screenText[settings.screen_mode]);
      break;
   case 3:
      //addtext1(graphixmenu[n], "Show CPU Usage: ", settings.cpu_usage ? "On" : "Off");
	break;
   }
}

static int a_graphix(int index, int subindex)
{
   if (index == M_INIT) {
      b_graphix(1);
      b_graphix(2);
      b_graphix(3);
      backoff = optionmenu;
      return 0;
   }

   switch (index) {
	case 1:
      if (++settings.shadows > 2) settings.shadows = 0;
      shadow_level(settings.shadows);
      b_graphix(index);
      break;
   case 2:     // Screen mode
      if (++settings.screen_mode > 3) settings.screen_mode = 0;
      b_graphix(index);

		//Reset the current screen
		render.clear(0, 0, 0);
		render.update();

		setScreenMode(settings.screen_mode);

		//Reset the render
		render.update();
		render.clear(0, 0, 0);

		//Redraw the menu
		clearmenu();
		buildmenu(graphixmenu);
		render.update();

      break;

   case 3:     // CPU Usage
      //settings.cpu_usage = !settings.cpu_usage;
      b_graphix(index);
   }

   return 0;
}


static int a_play1(int index, int subindex)
{
   if (index == M_INIT) {
      backoff = mainmenu;
      return 0;
   }

   nplayers = subindex + 1;
// Set proper caption for next menu.
   strcpy(playmenu2[0].text, subindex ? "2 PLAYERS" : "1 PLAYER");
   buildmenu(playmenu2);
   return 0;
}

static int a_play2(int index, int subindex)
{
   if (index == M_INIT) {
      backoff = playmenu1;
      return 0;
   }

// Play a game.
   gsave.nplayers = nplayers;
   gsave.difficulty = subindex;

   return M_PLAY;
}

//---------------------------------------------------------
// Load & Save Game Menus and Routines.
//---------------------------------------------------------
#define MAXSAVEDGAMES      3        // Allowed load\save slots.
static SaveGame   save[MAXSAVEDGAMES];

static void b_loadgame(SaveGame *s)
{
   myFile   myfile;
   int      i;

   for (i = 0; i < MAXSAVEDGAMES; i++) s[i].active = 0;

   if (myfile.open(SAVEGAMEFILE)) {
      for (i = 0; i < MAXSAVEDGAMES; i++)
         myfile.read(&s[i], sizeof(SaveGame));
      myfile.close();
   }
}

static void b_savegame(SaveGame *s)
{
   myFile   myfile;
   int      i;

   if (myfile.creat(SAVEGAMEFILE)) {
      for (i = 0; i < MAXSAVEDGAMES; i++)
         myfile.write(&s[i], sizeof(SaveGame));
      myfile.close();
   }
}

static void b_loadsave(Menu &menu, SaveGame &s, char *special = 0)
{
   char  text[20];

   if (!s.active) {
      addtext1(menu, "-----------", "");
   } else {
      if (s.nplayers == 1) strcpy(text, "1 Player"); else strcpy(text, "2 Players");
      addtext2(menu, s.name, text);
      if (special == NULL) {
         addtext1dn(64, menu, s.date, 7);
      } else {
         addtext1dn(64, menu, special, 10);
      }
   }
}

static int a_load(int index, int subindex)
{
   if (index == M_INIT) {
      b_loadgame(save);
      b_loadsave(loadmenu[2], save[0]);
      b_loadsave(loadmenu[3], save[1]);
      b_loadsave(loadmenu[4], save[2]);
      backoff = mainmenu;
      return 0;
   }

// Player wants to load a game.
   switch (index) {
   case 2:
   case 3:
   case 4:     // Load GAME.
      if (save[index-2].active) {
         put_savegame(save[index-2]);
         return M_LOAD;
      }
      break;
   case 5:     // Back to main menu;
      buildmenu(mainmenu);
      break;
   }

   return 0;
}

static void b_updatesave(SaveGame &s)
{
   int   i, j;

   for (i = 0; i < MAXPLAYERS; i++) {
      s.player[i].ships = player[i].ships;
      s.player[i].money = player[i].money;
      for (j = 0; j < WEAPONSLOTS; j++) s.player[i].slot[j] = player[i].slot[j];
   }
}

static int a_save(int index, int subindex)
{
   if (index == M_INIT) {
      b_loadgame(save);
      b_loadsave(savemenu[2], save[0]);
      b_loadsave(savemenu[3], save[1]);
      b_loadsave(savemenu[4], save[2]);
      backoff = shopmenu;
      return 0;
   }

// Player wants to save a game.
   switch (index) {
   case 2:
   case 3:
   case 4:     // Save game.
      save[index-2] = *get_savegame();
      b_updatesave(save[index-2]);
      b_savegame(save);
      b_loadsave(savemenu[index], save[index-2], "saved");
      break;
   case 5:     // Back to main menu;
      buildmenu(shopmenu);
      break;
   }

   return 0;
}

//-------------------------------------------------------------------
// Shop routines
//-------------------------------------------------------------------

#define MAXWEAPONS         50       // Max number of weapons allowed.
static int  nws;                    // Number of weapons in BUY\SELL list.

static struct {
   int      valid;         // Is this weapon valid.
   int      type;          // What kind of weapon do we have here.
   int      pos;           // Position in Weapon list.
   int      next;          // Next in power-up chain.
   int      prev;          // Previous in power up chain.
   int      price;         // How much value is this weapon.
   int      you_own;       // How many of this type does the player own.
} w[MAXWEAPONS];

static int  wpos;          // Position in Weapon Scroller.
static int  wmark[3];      // Which weapon to show in SHOP SLOTS.
static int  wx, wy;        // Ship position before remove.



static void shop_savepos(void)
{
   wx = player[nowplayer].main->object->x;
   wy = player[nowplayer].main->object->y;
}


/*---------------------------------------------------------
 Function:

 Description:
 Scans all available weapons and prepares information
 to easily sell & buy weapons.
---------------------------------------------------------*/
static void shop_filter(void)
{
   int   i, j;
   int   pos;

   if (weapons.n >= MAXWEAPONS) error("Weapon Filter overflow.");

// Pass 1: Build chaining information.
   for (i = 0; i < weapons.n; i++) {
      w[i].next = weapons.weapon[i]->next;
      w[i].prev = -1;
      w[i].you_own = 0;
      w[i].price = 0;
      w[i].type = weapons.weapon[i]->type;
      w[i].pos = weapons.weapon[i]->position;
   }
   for (i = 0; i < weapons.n; i++) {
      for (j = 0; j < weapons.n; j++) {
         if (w[i].next == j) w[j].prev = i;
      }
   }
// Pass 2: Remove SHOTS & SHIPS. (only keep Sidekicks & Drones & Lives)
   for (i = 0; i < weapons.n; i++) {
      if (weapons.weapon[i]->type==WT_SHOT || weapons.weapon[i]->type==WT_SHIP)
         w[i].valid=0; else w[i].valid=1;
   }
// Pass 3: Remove unused chains.
   for (i = 0; i < weapons.n; i++) {
      for (j = 0; j < WEAPONSLOTS; j++) {
         if (i == player[nowplayer].slot[j]) break;
      }
      if (j < WEAPONSLOTS) {
         j = i;
         while ((j = w[j].prev) != -1) w[j].valid = 0;
      } else if (w[i].prev != -1) w[i].valid = 0;
   }
// Pass 3.5: Count weapons available for BUY\SELL.
   nws = 0;
   for (i = 0; i < weapons.n; i++) {
      if (w[i].valid) nws++;
   }
// Pass 4: Count weapons player already owns.
   for (i = 0; i < WEAPONSLOTS; i++) {
      if ((j = player[nowplayer].slot[i]) != -1) {
         do { w[player[nowplayer].slot[i]].you_own++; } while ((j = w[j].prev) != -1);
      }
   }
// Pass 5: Mark weapons which will be displayed in shop.
   wmark[0] = wmark[1] = wmark[2] = -1;
   for (i = 0; i < 3; i++) {
      for (j = 0; j < weapons.n; j++) {
         if (w[j].valid && w[j].pos==wpos+i) {
            wmark[i] = j;
            break;
         }
      }
   }
// Pass 6: Determine Price.
   for (i = 0; i < weapons.n; i++) {
      if ((w[i].next != -1) && (w[i].you_own > 0)) {
         w[i].price = weapons.weapon[w[i].next]->price;
         w[i].price -= weapons.weapon[i]->price;
      } else if (w[i].prev == -1) {
         w[i].price = weapons.weapon[i]->price;
      }
   }
}

static void shop_display(void)
{
   int   i, j;
   char  text1[20], text2[10];

   for (i = 0; i < 3; i++) {
      if (wmark[i] != -1) {
         j = wmark[i];
         strcpy(text1, "Price: ");
// If weapon is last in a power chain, we display "----".
         if (w[j].price) {
            snprintf(text2, 9, "%d", w[j].price);
            //itoa(w[j].price, text2, 10);
         } else {
            strcpy(text2, "----");
         }
         strcat(text1, text2);
         addtext2(shopmenu[i+2], weapons.weapon[j]->name, text1);
         strcpy(text1, "Lvl: ");
         if (w[j].type == WT_LIFE)
            snprintf(text2, 9, "%d", player[nowplayer].ships-1);
            //itoa(player[nowplayer].ships-1, text2, 10);
         else
         {
             snprintf(text2, 9, "%d", w[j].you_own);
             //itoa(w[j].you_own, text2, 10);
         }

         strcat(text1, text2);
         addtext1dn(64, shopmenu[i+2], text1, 7);
      }
   }
}

static int find_slot(int slot, int find)
{
   if (slot == 3) {
// If weapon uses slot 3 then we can actually choose among slot
// 3, 4, 5 & 6 respectively. (A sidekick can go into any free Sidekick slot).
      if (player[nowplayer].slot[6] == find) slot = 6;
      if (player[nowplayer].slot[5] == find) slot = 5;
      if (player[nowplayer].slot[4] == find) slot = 4;
      if (player[nowplayer].slot[3] == find) slot = 3;
   }
   return player[nowplayer].slot[slot] != find ? -1 : slot;
}

static void your_money(int money)
{
   char  text[20];
   int   y;

   shopmenu[1].back->restore();
   y = MENUY + shopmenu[1].y;
   tiny->vanilla(MENUX+35, y, "Your money:", 15);
   snprintf(text, 19, "%d", money);
   //itoa(money, text, 10);
   tiny->vanilla(MENUX+95, y, text, 12);
}

static void shop_error(char *text)
{
   int   y;

   shopmenu[1].back->restore();
   y = MENUY + shopmenu[1].y;
   tiny->vanilla_c(MENUX+(MENUXS+BORDER)/2, y, text, 10);
}

/*---------------------------------------------------------
 Function:

 Description:
 Weapon "n" has to be bought.
 Looks if there is a free slot to place weapon.
---------------------------------------------------------*/
static void buy(int n)
{
   int   slot;

// Um welche waffe handelt es sich denn nun ?
   n = wmark[n];

// Where's the weapon positioned we want to sell\buy.
   slot = weapons.weapon[n]->slot;

// B U Y.
// Does the player have enough bucks to buy
   if (player[nowplayer].money < w[n].price) {
      shop_error("Not enough money.");
      return;
   }
   switch (w[n].type) {

// EXTRA LIFE.
   case WT_LIFE:
      player[nowplayer].money -= w[n].price;
      your_money(player[nowplayer].money);
      player[nowplayer].ships++;    // You get another life.
      shop_display();
      break;

// ALL OTHER WEAPONS.
   default:
// If this weapon is part of a power up chain, we can always equip.
// Else we have to check if a free slot is available.
      if ((w[n].next != -1) && w[n].you_own) {
// Credit player's account.
         player[nowplayer].money -= w[n].price;
         your_money(player[nowplayer].money);
// Upgrade weapon.
         player[nowplayer].slot[slot] = w[n].next;
         shop_savepos();
         weapon_removeall(nowplayer);
         weapon_releaseall(nowplayer, wx, wy);
         shop_filter();
         shop_display();
      } else {
// Check if free slot is available.
         slot = find_slot(slot, -1);
         if (slot != -1) {
// Slot is free so lets go ahead.
// Credit player's account.
            player[nowplayer].money -= w[n].price;
            your_money(player[nowplayer].money);
            player[nowplayer].slot[slot] = n;
            shop_savepos();
            weapon_removeall(nowplayer);
            weapon_releaseall(nowplayer, wx, wy);
            shop_filter();
            shop_display();
         } else {
// No slot available. Notify player.
            shop_error("Slot already occupied.");
            return;
         }
      }
   }
}

static void sell(int n)
{
   int   slot;

// Um welche waffe handelt es sich denn nun ?
   n = wmark[n];

// Where's the weapon positioned we want to sell.
   slot = weapons.weapon[n]->slot;

// S E L L.
   switch (w[n].type) {

// Try to sell a LIFE.
   case WT_LIFE:
      if (player[nowplayer].ships <= 1) return;
      player[nowplayer].ships--;
      shop_display();
      break;

// All other WEAPONS.
   default:
      if (w[n].you_own == 0) return;   // You don't own any -> nothing to do.
// If this weapon is part of a power up chain: Go back to weaker option.
      if (w[n].prev != -1) {
         n = w[n].prev;
         player[nowplayer].slot[slot] = n;
         shop_savepos();
         weapon_removeall(nowplayer);
         weapon_releaseall(nowplayer, wx, wy);
         shop_filter();
         shop_display();
      } else {
/// Find a slot which holds the weapon player wants to sell.
         slot = find_slot(slot, n);
         if ((slot != -1) && (weapons.weapon[n]->type != WT_SHIP)) {
// Add some money to player's account.
            player[nowplayer].slot[slot] = -1;
            shop_savepos();
            weapon_removeall(nowplayer);
            weapon_releaseall(nowplayer, wx, wy);
            shop_filter();
            shop_display();
         }
      }
   }
// Add some money to player's account.
   player[nowplayer].money += w[n].price;
   your_money(player[nowplayer].money);
}

static int a_shop(int index, int subindex)
{
   int   i;

// If index==99 then just build the menu. (No action).
   if (index == 99) {
      if (cheatlevel & CHEAT_MONEY) player[nowplayer].money = 1000000;
      wpos = 0;
      your_money(player[nowplayer].money);
      shop_filter();
      shop_display();
      backoff = 0;   // No backoff allowed this time.
      return 0;
   }

   your_money(player[nowplayer].money);

   switch (index) {
   case 2:     // Buy-sell given weapon.
   case 3:
   case 4:
      if (subindex == 0) buy(index-2); else sell(index-2);
      break;
   case 5:     // Up down pressed.
      if ((subindex==0) && (wpos > 0)) wpos--;
      if ((subindex==1) && (wpos < nws-3)) wpos++;
      shop_filter();
      shop_display();
      break;
   case 6:     // Save game
      buildmenu(savemenu);
      break;
   case 7:     // Play next level.
// Reflect changes made to weapon in PlayerState structure.
      psave.ships = player[nowplayer].ships;
      psave.money = player[nowplayer].money;
      for (i = 0; i < WEAPONSLOTS; i++)
         psave.slot[i] = player[nowplayer].slot[i];
      return 1;
   }

   return 0;
}


