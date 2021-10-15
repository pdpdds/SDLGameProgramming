
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "DISKIO.HPP"
#include "MEMORY.HPP"
#include "ERROR.HPP"
#include "INPUT.HPP"
#include "SDL_BASE.HPP"
#include "SOUND.HPP"
#include "SOUND_SDL.HPP"
#include "RAVAGE.HPP"
#include "COMPATIBILITY.HPP"


// A save game structure. Used to exchange data to\from game loader\saver.
static   SaveGame savegame;

// Game RECORDER variables.

struct Record {
   signed char    dx;
   signed char    dy;
   signed char    fire;
};
static   Record   *record;       // Pointer to recording buffer.
static   int      maxrecords;    // Size of allocated recording buffer.
static   int      rec_count;     // How many snapshots already taken ?
static   Record   *now_record;   // Pointer to current buffer element.
static   int      rec_enabled = 0;



/*---------------------------------------------------------
 Function: PlayerInfo class

 Description:
 This class is only used within this module.
 It displays player information such as
 LIVES, MONEY or the like.
---------------------------------------------------------*/
class PlayerInfo {
   SDLSprite   label;        // Player score LABELS.
   SDLSprite   life;         // Small LIFE symbol.
   Font     *digit;        // 0123456789.
   SDLSprite   cpubanner;    // "CPU USAGE (%)" banner.
#ifdef STATS
   Font     *tiny;         // Use for statistics
#endif

public:
// Initialize needed sprites & fonts.
   void init(void)
   {
		label.load((Sprite *)loadfile("pic\\pinfo.spr", NULL), false);
      life.load((Sprite *)loadfile("pic\\life.spr", NULL), false);
      digit = new Font("fonts\\yellow.fnt");
      cpubanner.load((Sprite *)loadfile("pic\\cpu.spr", NULL), false);
      #ifdef STATS
         tiny = new Font("fonts\\tiny.fnt");
      #endif
   }
// Undo the effect of a previous load.
   void shut(void)
   {
      unloadfile(label.getBase());
      unloadfile(life.getBase());
      delete digit;
      unloadfile(cpubanner.getBase());
		label.destroy();
		life.destroy();
		cpubanner.destroy();
      #ifdef STATS
         delete tiny;
      #endif
   }
// Display SCORE and LIVES information for player p.
   void display(int p)
   {
      int   i;

      if (p == 0) {
         render.x_drawsprite(&label, 6, 4, p);
         for (i = 0; i < player[p].ships-1; i++) {
            if (i > 4) break;
            render.x_drawsprite(&life, 6 + i*8, 14, 0);
         }
      } else {
         render.x_drawsprite(&label, 240, 4, p);
         for (i = 0; i < player[p].ships-1; i++) {
            if (i > 4) break;
            render.x_drawsprite(&life, 275 - i*8, 14, 0);
         }
      }
   }

#ifdef STATS
// Display DEBUG information during development.
   void debug(void)
   {
      char  text[20];

      itoa(gstate.enemy_count, text, 10);
      tiny->vanilla(200, 200, "Enemies", 15);
      tiny->vanilla(200, 210, text, 12);
   }
#endif
};
static   PlayerInfo  pinfo;

/*---------------------------------------------------------
 Function:

 Description:
 This function group allows for store and restoring
 of a GAME.
---------------------------------------------------------*/
// Prepare the SaveGame structure.
// Store all te needed data to restore game later.
static void storegame(SaveGame &s)
{
   int      i, j;
   time_t   time_now;

// Set this slot to active.
   s.active = 1;
// Set current date.
   time_now = time(NULL);
   strftime(s.date, 14, "%d. %b. %Y", localtime(&time_now));
// Copy level name.
   strcpy(s.name, level.descript->text);
// Copy several global game settings.
   s.difficulty = gstate.difficulty;
   s.nplayers = gstate.nplayers;
   s.level = gstate.level;
// Copy player information.
   for (i = 0; i < MAXPLAYERS; i++) {
      s.player[i].active = player[i].active;
      s.player[i].ships = player[i].ships;
      s.player[i].money = player[i].money;
      for (j = 0; j < WEAPONSLOTS; j++) s.player[i].slot[j] = player[i].slot[j];
   }
}

static void restoregame(SaveGame &s)
{
   int   i, j;

   if (!s.active) error("Tried to load inactive game.");

// Restore Game state.
   gstate.difficulty = s.difficulty;
   gstate.nplayers = s.nplayers;
   gstate.level = s.level;
// restore player state.
   for (i = 0; i < MAXPLAYERS; i++) {
      player[i].active = s.player[i].active;
      player[i].ships = s.player[i].ships;
      player[i].money = s.player[i].money;
      for (j = 0; j < WEAPONSLOTS; j++) player[i].slot[j] = s.player[i].slot[j];
   }
}

// returns pointer to SaveGame structure.
SaveGame *get_savegame(void)
{
   return &savegame;
}

// Copy SaveGame structure and prepare system to restore a game.
void put_savegame(SaveGame &s)
{
   savegame = s;
}


/*---------------------------------------------------------
 Function: newgame

 Description: Initialize a new game.
 Starting at given stage.
---------------------------------------------------------*/
void newgame(int stage)
{
   int   i, j;

   if (gstate.ready++) return;   // Don't do anything if already initialized.

// Initilialize Game Status.
   gstate.level = stage;         // Start with given stage.

   for (j = 0; j < MAXPLAYERS; j++) {
      gstate.weapon[j] = 0;      // Reset weapon queue.
   }

// Load weapons.
   weapons.load("weapons\\weapons");

// Initialize player structure.
   for (j = 0; j < MAXPLAYERS; j++) {
// Clear Weapon Slots.
      for (i = 0; i < WEAPONSLOTS; i++) {
         player[j].slot[i] = -1;
      }
      player[j].main = 0;           // No weapon defined at this moment.
      player[j].wreck = 0;          // Player ist gesund und fit.
      player[j].money = 0;          // No money for nothing and goofy.
      player[j].slot[0] = j;        // Define BASE weapon for each player.
      player[j].slot[1] = FIRSTSHOT;// Define First shot.
      player[j].active = 0;         // Disable player by default.
      player[j].ships = STARTSHIPS; // Number of ships you start with.
   }
// Activate players.
   player[0].active = 1;            // Player 0 is always active.
   if (gstate.nplayers == 2) player[1].active = 1;

// Prearm ship if not starting game with level 1.
   for (i = 0; i < MAXPLAYERS; i++) {
      switch (stage) {
      case 2:
         player[i].slot[1] = 13; // front 3
         break;
      case 3:
         player[i].slot[1] = 16; // spread 2
         player[i].slot[2] = 43; // side 1
         break;
      case 4:
         player[i].slot[1] = 14; // front 4
         player[i].slot[3] = 39; // homing
         break;
      case 5:
         player[i].slot[1] = 15; // front 5
         player[i].slot[3] = 21; // beer
         player[i].slot[2] = 27; // miss 1
         break;
      case 6:
         player[i].slot[1] = 18; // spread 4
         player[i].slot[2] = 47; // side 3
         player[i].slot[7] = 36; // rear 3
         break;
      case 7:
      case 99:
         player[i].slot[1] = 18; // spread 4
         player[i].slot[3] = 19; // laser
         player[i].slot[2] = 30; // miss 2
         break;
      case 8:
         player[i].slot[1] = 18; // spread 4
         player[i].slot[3] = 19; // laser
         player[i].slot[4] = 23; // cos
         player[i].slot[5] = 39; // homing
         break;
      case 9:
         player[i].slot[1] = 18; // spread 4
         player[i].slot[3] = 19; // laser
         player[i].slot[4] = 19; // laser
         player[i].slot[5] = 39; // homing
         player[i].slot[6] = 37; // electro
         break;
      }
   }
}

void shutgame(void)
{
   int   i;

   if (--gstate.ready) return;
   for (i = 0; i < MAXPLAYERS; i++) {
      weapon_removeall(i);
   }
// Get rid of all the loaded weapons.
   weapons.unload();
}

/*---------------------------------------------------------
 Function: newlevel

 Description:
 Loads and prepares LEVEL n.
---------------------------------------------------------*/
void newlevel(char *name)
{
   level.load(name);

   gstate.bosses = level.descript->bigboss_count;
   gstate.frame = 0;             // Reset frame counter.
// Set initial layer speeds.
   gstate.layerspeed[0] = gstate.layercount[0] = 2;
   gstate.layerspeed[1] = gstate.layercount[1] = 1;
   gstate.enemy_head = gstate.enemy_tail = 0;  // Empty enemy queue.
   gstate.attack = level.attack;               // Prepare attack table.
// Set map to start position.
   setlayer(0, 0); setlayer(1, 0);
// Fill in MARK structure.
   gstate.mark.frame = gstate.frame;
   gstate.mark.attack = gstate.attack;
   gstate.mark.layerpos[0] = 0;
   gstate.mark.layerpos[1] = 0;
}

void shutlevel(void)
{
// Remove any active NOPS first.
   enemy_removeall();

// Remove all objects.
   killallobjects();

// Unload level data.
   level.unload();
}

/*---------------------------------------------------------
 Function: *

 Description:
 Define all WEAPONS player "p" has bought, stolen or otherwise
 obtained.
---------------------------------------------------------*/
void weapon_releaseall(int p, int x0, int y0)
{
   int      i;
   int      xl, xr, y;
   int      xs, ys;
   Weapon   *w;
   Sprite   *spr;

   xl = xr = x0; y = y0; ys = 0;
   for (i = 0; i < WEAPONSLOTS; i++) {
      if (player[p].slot[i] == -1) continue;
      switch (i) {
      case 0:     // Main slot (HAUPTSCHIFF).
         w = release_weapon(p, player[p].slot[i], x0, y0);
         w->flags |= WF_MAIN;      // Say this is the most important weapon.
         player[p].main = gstate.weapon[p];
         xs = player[p].main->object->sprite->xs;
         ys = player[p].main->object->sprite->ys;
         xl -= 2; xr += xs + 2;
         break;
      case 1:     // Front Gun.
         spr = player[p].main->object->sprite;
         release_weapon(p, player[p].slot[i], x0 + spr->xs/2, y0);
         break;
      case 2:     // Center Gun (usually MISSILE LAUNCHER)
         spr = player[p].main->object->sprite;
         release_weapon(p, player[p].slot[i], x0 + spr->xs/2, y0 + spr->ys/2);
         break;
      case 3:     // Left Sidekicks.
      case 5:
         spr = weapons.sprite[weapons.weapon[player[p].slot[i]]->sprite];
         xl -= spr->xs;
         release_weapon(p, player[p].slot[i], xl, y + ys-spr->ys);
         xl -= 2;
         break;
      case 4:     // Right Sidekicks.
      case 6:
         spr = weapons.sprite[weapons.weapon[player[p].slot[i]]->sprite];
         release_weapon(p, player[p].slot[i], xr, y + ys-spr->ys);
         xr += spr->xs + 2;
         break;
      case 7:     // Rear gun
         spr = player[p].main->object->sprite;
         release_weapon(p, player[p].slot[i], x0 + spr->xs/2, y0 + spr->ys);
         break;
      }
   }
}


/*---------------------------------------------------------
 Function: RECORDER functions

 Description:
 These function are used to manage the game play RECORDER.
 Recording GamePlay is only allowed in RECORDER Mode.
---------------------------------------------------------*/
#ifdef RECORDER
void rec_enable(void)
{
// Enable recorder.
   rec_enabled = 1;
}

// Initialize recorder and allocate space for "len" records.
static void rec_init(int len)
{
   now_record = record = new Record[len];
   maxrecords = len;
   rec_count = 0;
}

// Save recorder game to disk.
static void rec_save(char *file)
{
   myFile   myfile;

// Try to open file for write access.
   if (!myfile.creat(file)) error("(rec_save): Can't write to disk.");

// Write recorded data.
   myfile.write(&maxrecords, sizeof(maxrecords));
   myfile.write(record, maxrecords*sizeof(Record));

   myfile.close();

// Disable further recording.
   rec_enabled = 0;
   delete []record;
}
#endif
void playback_start(char *file)
{
   myFile   myfile;

   if (!myfile.open(file)) error("Playback file not found: ", file);
   myfile.read(&maxrecords, sizeof(maxrecords));
   record = new Record[maxrecords];
   myfile.read(record, maxrecords*sizeof(Record));
   myfile.close();

   now_record = record;
   rec_count = 0;
}

void playback_stop(void)
{
   delete []record;
}

void playback_device(int &dx, int &dy, int &fire)
{
   if (rec_count >= maxrecords) error("(rec_control): Strange !!");
   dx = now_record->dx;
   dy = now_record->dy;
   fire = now_record->fire;
   now_record++; rec_count++;
}

void playback_loop(void)
{
   if (rec_enabled) return;
   now_record = record;
   rec_count = 0;
}

/*---------------------------------------------------------
 Function: playlevel

 Description:
 This one plays one level and returns:
   -1  : Abort request through ESC.
    1  : Player has been killed.
    2  : Player has successfully killed Mr. OberMotz.
---------------------------------------------------------*/
static volatile int cpu;
static int playlevel(void)
{
   int      dx, dy, fire;     // Player commands.
   int      delay = 0;            // Delay value.
   int      master = 0;       // Cause of game abort.
   int      fast = 1;             // Used in determining frame speed.
   int      tmp;              // Some really unimportant temporary values.
   Uint32	startTime = 0;
   SDL_Rect borderLeft = {0, 0, BORDER, YMAX};
   SDL_Rect borderRight = {HWXMAX - BORDER, 0, BORDER, YMAX};

#ifdef DEBUG
   int   singlestep = 0;
#endif

// Used during level design only.
/*
    {
      gstate.frame = 5500;
      movelayer(0, 2750); movelayer(1, 5500);
      process_objects();
      drawscreen();
      while (gstate.attack->count < gstate.frame) gstate.attack++;
   }
*/

// Start a nearly endless loop.
   do {

   fast = 1;
   startTime = SDL_GetTicks();

   render.clear(0, 0, 0);

// Release new enemies.
   newwave();
// Calculate enemy's next step.
   enemystep();

   input_update();
   s_update();

// Spend some CPU time to handle the brave fighters.
      if (player[0].active) {
// Get control commands. (left, right, up, down, fire).
         player[0].control(dx, dy, fire);

#ifdef RECORDER
// Record player movement if RECORDER is enabled.
         if (rec_enabled && (rec_count<maxrecords)) {
            now_record->dx = dx;
            now_record->dy = dy;
            now_record->fire = fire;
            now_record++; rec_count++;
         }
#endif
// Move ship according to given commands.
         weaponstep(0, dx * SHIPSPEED, dy * SHIPSPEED, fire);
// Have a closer look at the HAUPTSCHIFF: Is it really still invincible ?
         if (player[0].glass > 0) {
            if (--player[0].glass & 1) player[0].main->object->flash();
         }
// Only do a crash test if ship is not already wrecked.
         if (!player[0].wreck && !master) {
            tmp = crash(0);      // Crash test. 0=ok 1=ship gone 2=boss gone.
            if (tmp == 1) {
// Ship has been destroyed. Bad luck, eh ?
               player[0].wreck = 1;
               player[0].delay = CRASHTIME;
               weapon_removeall(0);
            } else if (tmp == 2) {
// Mr OberMotz has bitten the dust. Yeah.
               delay = player[0].glass = player[1].glass = level.descript->done_delay;
               master = 2;
            }
         } else {
// Look if another ship is ready.
            if (--player[0].delay == 0) {
               if (--player[0].ships) {
// Allright: Release new ship.
                  weapon_releaseall(0, player[0].x, player[0].y);
                  player[0].wreck = 0;
                  player[0].glass = GLASSDELAY;
               } else {
// No more lives available. Player is GAME OVER.
                  player[0].active = 0;
// If both players are gone: The whole game is game over. (Sic!)
                  if (!player[1].active) {
                     delay = CRASHTIME;
                     master = 1;
                  }
               }
            }
         }
      }
// This is the same story as for player 0: No comments inserted here.
      if (player[1].active) {
         player[1].control(dx, dy, fire);
         weaponstep(1, dx * SHIPSPEED, dy * SHIPSPEED, fire);
         if (player[1].glass > 0) {
            if (--player[1].glass & 1) player[1].main->object->flash();
         }
         if (!player[1].wreck && !master) {
            tmp = crash(1);      // Crash test. 0=ok 1=ship gone 2=boss gone.
            if (tmp == 1) {
               player[1].wreck = 1;
               player[1].delay = CRASHTIME;
               weapon_removeall(1);
            } else if (tmp == 2) {
               delay = player[0].glass = player[1].glass = level.descript->done_delay;
               master = 2;
            }
         } else {
            if (--player[1].delay == 0) {
               if (--player[1].ships) {
                  weapon_releaseall(1, player[1].x, player[1].y);
                  player[1].wreck = 0;
                  player[1].glass = GLASSDELAY;
               } else {
                  player[1].active = 0;
                  if (!player[0].active) {
                     delay = CRASHTIME;
                     master = 1;
                  }
               }
            }
         }
      }

// Move map a little bit.
      gstate.frame++;
      if (gstate.frame < layerlength(1)) {
         if (--gstate.layercount[0] <= 0) {
            gstate.layercount[0] = gstate.layerspeed[0];
            movelayer(0, 1);
         } else movelayer(0, 0);
         if (--gstate.layercount[1] <= 0) {
            gstate.layercount[1] = gstate.layerspeed[1];
            movelayer(1, 1);
         } else movelayer(1, 0);
      } else {
         movelayer(0, 0); movelayer(1, 0);
      }

// Start building new screen.
// Process objects.
   process_objects();
// Draw Background and Sprites on virtual screen.
   drawscreen();

   postprocess_objects();

// Write Player information on top of the whole mess.
     if (player[0].active) pinfo.display(0);
     if (player[1].active) pinfo.display(1);

    // Start fading if we are just in time.
    if (delay > 0 && delay <= 32) {
        // Fade to white if BOSS has been killed.
        if (master == 2)
            {
                SDL_SetRenderDrawColor(render.getBase(), 255, 255, 255, (32 - delay)*SDL_ALPHA_OPAQUE/32);
                SDL_RenderFillRect(render.getBase(), NULL);
        }
    }
    //Draw borders
    render.drawRect(&borderLeft, 0, 0, 0);
    render.drawRect(&borderRight, 0, 0, 0);

   render.update();


      if (delay) --delay;

      if (master && (delay == 0)) return master;
      if (key[KEY_ESC]) return -1;

		while(SDL_GetTicks() - startTime < 32)
		{
				SDL_Delay(1);
		}

		//Debug step the game loop
		//input_wait();
		//input_fire();

   } while (!quit_requested);

    return 1;
}


// Show 'GET READY'
static void showstartlogo(void)
{
   Sprite *ready;
	SDLSprite sReady;
   Font   *font;

// Clear screen.
   render.clear(0, 0, 0);

// Load needed stuff into memory.
   ready = (Sprite *)loadfile("pic\\ready.spr", NULL);
	sReady.load(ready, false);
   font = new Font("fonts\\blue.fnt");
// Draw gamescreen.
   drawscreen();
   font->print_c(XMAX/2, 40, level.descript->text);
   render.x_drawsprite(&sReady, (XMAX - ready->xs)/2, 100, 0);
   font->print_c(XMAX/2, 200, "HIT FIRE");
//   pixelate(1);
// 'GET READY' is no longer used.
   delete font;
   unloadfile(ready);
// Fade in palette.
   fadein(level.palette);

   sReady.destroy();

// Wait for fire button being released & pressed again.
   input_wait(); input_fire(); input_wait();
}

// Show GAME OVER.
static void showendlogo(void)
{
   Sprite *gover;
	SDLSprite sGover;

// Load needed stuff into memory.
   gover = (Sprite *)loadfile("pic\\gover.spr", NULL);
	sGover.load(gover, false);
   render.x_drawsprite(&sGover, (XMAX - gover->xs)/2, 100, 0);
	render.update();
   unloadfile(gover);
	sGover.destroy();

// Wait for fire button being released & pressed again.
   input_wait();
   input_fire();

}

// Show HAPPY END screen.
static void showhappyend(void)
{
   void  *f;
	int size = 0;

// Load MOD.
   s_loadmod("mods\\happy.uni");
// Start music and fade in screen.
   s_startmod();
// Show HAPPY picture
   f = loadfile("pic\\happy.pcx", &size);
   render.clear(0, 0, 0);
   showpcx(f, 0, size);
   unloadfile(f);

// Some delay before we fadein the picture.
   SDL_Delay(8000);

   fadein(NULL);
   input_wait(); input_fire(); input_wait();
   fadeout(FADE_ALL);
   s_unloadmod();
}

/*---------------------------------------------------------
 Function:

 Description:
 Show a small abort request box.
 If player wants to abort the game -1 is returned.
 0 otherwise.
---------------------------------------------------------*/
static int ask_abort(void)
{
   Font     *tiny;
   Sprite   *escbox;
   SDLSprite sEscbox;
   int      x, y, fire0 = 0, fire1 = 0;

   tiny = new Font("fonts\\tiny.fnt");
   escbox = (Sprite *) loadfile("pic\\escbox.spr", NULL);
   sEscbox.load(escbox, false);

   y = (YMAX-escbox->ys)/2;
   render.x_drawsprite(&sEscbox, (XMAX-escbox->xs)/2, y, 0);
   y += 15;
   tiny->vanilla_c(XMAX/2, y, "Press ESC again to abort the game.", 12);
   y += 15;
   tiny->vanilla_c(XMAX/2, y, "Fire to continue.", 15);
   render.update();

   unloadfile(escbox);
   delete tiny;
   sEscbox.destroy();

   input_wait();

   do {
		input_update();
      if (player[0].active) player[0].control(x, y, fire0);
      if (player[1].active) player[1].control(x, y, fire1);
   } while (!fire0 && !fire1 && !key[KEY_ESC]);
   return key[KEY_ESC] ? -1 : 0;
}

static void getlevelname(int n, char *name)
{
   switch (n) {
      case 1 : strcpy(name, "level1\\level1"); break;
      case 2 : strcpy(name, "level2\\level2"); break;
      case 3 : strcpy(name, "level3\\level3"); break;
      case 4 : strcpy(name, "level4\\level4"); break;
      case 5 : strcpy(name, "level5\\level5"); break;
      case 6 : strcpy(name, "level6\\level6"); break;
      case 7 : strcpy(name, "level7\\level7"); break;
      case 8 : strcpy(name, "level8\\level8"); break;
      case 9 : strcpy(name, "level9\\level9"); break;
      case 99: strcpy(name, "m\\m"); break;
      default: strcpy(name, "level1\\level1"); break;
   }
}

static void addbonus(void)
{
   int   i;
   int   money;

// Decide how much money we go to distribute among players.
   money = (int)level.descript->money;

// Distribute money.
   for (i = 0; i < MAXPLAYERS; i++) {
      if (player[i].active) player[i].money += money;

		if(player[i].active && player[i].money < 0) printf("Warning player money < 0: %d, level money %d\n", player[i].money, (int)level.descript->money);
   }


}

/*---------------------------------------------------------
 Function:

 Description:
 YEEES. Call this function and you will face ACTION.
 Arguments:
 load : 0 = Start a brand new game (level 1)
 load : 1 = Load a game previously prepared with
        "put_savegame".
---------------------------------------------------------*/
void play(int load)
{
   int   feedback;
   int   i;
   char  name[20];          // Name of current level.

/*
   The meaning of feedback:
   == -1: Abort request through ESC.
   ==  1: Ship has been destroyed (game over).
   ==  2: Level has been finished.
*/

// Get active input device.
   for (i = 0; i < MAXPLAYERS; i++) {
      player[i].control = getdevice(settings.device[i]);
   }

// Load player STATUS display.
   pinfo.init();

// Initialize new game at given difficulty level.
   newgame(start_stage);

// Load game if user want to.
   if (load) {
      restoregame(savegame);
// Go to shop once more.
      if (player[0].active) menu(SHOPMENU, 0);
      if (player[1].active) menu(SHOPMENU, 1);
      gstate.level++;
   }

// Start actual game.
   do {

		//Don't let the mouse escape.
		SDL_SetRelativeMouseMode(SDL_TRUE);

      weapons.unload();
		destroy_SpritePools();
      weapons.load("weapons\\weapons");

      getlevelname(gstate.level, name);
      newlevel(name);
#ifdef RECORDER
// Initialize RECORDER if user wants to record gameplay.
      if (rec_enabled) rec_init(layerlength(1));
#endif
// Display "GET READY".
      showstartlogo();
// Start background music.
      s_startmod();

// First both players are invincible.
      player[0].glass = player[1].glass = GLASSDELAY;
// Release weapons of active players.
      if (player[0].active) weapon_releaseall(0, STARTX1, STARTY);
      if (player[1].active) weapon_releaseall(1, STARTX2, STARTY);
// Play level and get feedback.
      do {
         feedback = playlevel();
         if (feedback == -1) feedback = ask_abort();
      } while (feedback == 0);


// Restore old shadow level.
      shadow_level(settings.shadows);
// Remove all weapons.
      weapon_removeall(0);
      weapon_removeall(1);

// Save RECORDED data if recorder is active.
#ifdef RECORDER
     if (rec_enabled) rec_save("rec.dat");
#endif
// You failed ?   Show GAME OVER.
   if (feedback == 1) {
      showendlogo();
   }
      if(feedback == 2)
      {
         // Add bonus money.
         addbonus();
      }
// Do a beautiful FADEOUT.
      fadeout(FADE_ALL);
// Store complete game state to structure savegame.
      storegame(savegame);
      shutlevel();

// Let the mouse escape.
		SDL_SetRelativeMouseMode(SDL_FALSE);

// If this is a abort request: Quietly quit the game.
      if (feedback == -1) break;

// Restore pallete
		void *p = (void *)loadfile("demo1\\DEMO1.CPA", NULL);
		palette.set(p);
		delete p;
// Enter shop for both players.
      if ((feedback == 2) && (gstate.level < MAXLEVELS)) {
         if (player[0].active) menu(SHOPMENU, 0);
         if (player[1].active) menu(SHOPMENU, 1);
      }
      gstate.level++;
   } while ((feedback == 2) && (gstate.level <= MAXLEVELS));

   shutgame();
   pinfo.shut();

	destroy_SpritePools();

// Rescued your BELOVED ?
   if (feedback == 2) {
         if (start_stage == 1) showhappyend();
   }

}



