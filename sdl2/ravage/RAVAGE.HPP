
#ifndef AH_RAVAGE
#define AH_RAVAGE

#include "TYPES.H"
#include "GCLASS.HPP"
#include "DISKIO.HPP"
#include "SDL_BASE.HPP"

#ifndef AH_MAINMODULE
#define _ext extern
#else
#define _ext
#endif

// Einige NATURKONSTANTEN.
// Don't expect the game to run properly if you change any of
// the NATURKONSTANTEN.
#define MEMNEEDED       2000     // Needed DPMI memory in KB.
#define CONFIGFILE      "RAVAGE01.INI"
#define SAVEGAMEFILE    "RAVAGE.SAV"
#define MAXPLAYERS      2        // Number of players allowed by game.
#define WEAPONSLOTS     8        // Number of power up slots.

// Umwelt Konstanten.
#define DEEPSHADOW      75       // Wieviel Schatten braucht der Mensch?
#ifdef SHAREWARE
   #define MAXLEVELS    2
#else
   #define MAXLEVELS    9        // Wieviele levels gibt's im game.
#endif

// Some initial conditions.
#define STARTX1         70       // Starting SHIP coordinates Player 1.
#define STARTX2         190      // STarting SHIP coordinates Player 2.
#define STARTY          200
#define STARTSHIPS      3        // Games starts with 3 lives.
#define SHIPSPEED       5        // Speed is 5 pixels/frame.
#define GLASSDELAY      90       // Ship is invincible for 90 frames.
#define CRASHTIME       25       // How many frames to continue if CRASHED.
#define FIRSTSHOT       2        // Number of initial equiped shot.

// Possible cheat modes.
#define CHEAT_LIVES     1        // Unlimited Lives (or was it Lifes ??)
#define CHEAT_INVUL     2        // Invulnerable
#define CHEAT_MONEY     4        // Unlimited money.
#define CHEAT_NOMONEY   8        // Don't add money (used in shop mode).

// Define the GameState structure.
// This one holds data to track the state of the current game being played.
struct GameState {
   int         ready;            // Is a game in progress at the moment?
   int         difficulty;       // Playing at which difficulty level ?
   int         nplayers;         // Number of players in game.
   int         level;            // Which level are we currently playing.
   int         bosses;           // Number of bosses still to blast.
   Weapon      *weapon[MAXPLAYERS]; // All currently active weapons and shots.
   Enemy       *enemy_head;      // Head of enemy queue.
   Enemy       *enemy_tail;      // End of enemy queue.
   int         enemy_count;      // Number of active enemies.
   Attack      *attack;          // Pointer to next attack command in table.
   int         frame;            // Frame counter.
   int         layerspeed[2];    // Layer speed.
   int         layercount[2];    // Layer countdown value.
   struct {
      int      frame;
      Attack   *attack;
      int      layerpos[2];
   }           mark;             // Marked frame position.
};


// PlayerState
// This one keeps track of one PLAYER'S state.
struct PlayerState {
   int         active;           // Player still alive ?
   void        (*control)(int &, int &, int &);
   Weapon      *main;            // Hauptwaffe.
   int         x, y;             // Last known position of ship before crash.
   int         ships;            // Number of spare lives player has got.
   int         glass;            // As long as this is 1. Ship is invincible.
   int         wreck;            // True if ship has been killed.
   int         delay;            // Time till next life gets active.
   int         slot[WEAPONSLOTS];// Which weapon slot holds which weapon.
   int         money;            // All the money player has collected.
};

// SaveGame
// It's exactly this structure which will be saved to disk.
#pragma pack(1)
struct SaveGame {
   int         active;           // Is this slot occupied ?
   char        date[16];         // Date game has been saved.
   char        name[DES_TEXT];   // Name of level.
   int         difficulty;       // Difficulty level.
   int         nplayers;         // 1 or 2 player game ?
   int         level;            // Number of level last completed.
   struct {
      int      active;           // Player is active ?
      int      ships;            // Spare lives.
      int      money;
      int      slot[WEAPONSLOTS];// Equipment list.
   }           player[MAXPLAYERS];
};

// Keep track of several machine settings.
// This structure will be saved to disk.
struct Settings {
// Control settings.
   int      device[MAXPLAYERS];  // Input device for all the players.
// Mouse settings.
   int      msens;               // Mouse sensitivity.
// Keyboard settings.
   int      key[5];              // Defined keys. (lt, rt, up, dn, fire).
// Joystick settings.
	char		joystickName[255];   // Name of last chosen joystick
   int      joypadbutton[5];
// Sound settings.
   int      loop_music;				// Ensure music is looped
   int      mod_vol;             // Volume of MOD Music.
   int      wav_vol;             // Volume of WAVES.
// Graphic settings.
   int      shadows;             // Shadow level. (None, Some or All).
	int		screen_mode;			// 0 - standard, 1 - stretched + linear scaling.
};


// Define some global variables.
_ext FilePool  filepool[4];      // A pool of all the needed data files.

_ext Settings  settings;         // Environment settings.

// --- Prototypes of "stuff.cpp"
#define FADE_PALETTE       1        // Do palette fading.
#define FADE_MUSIC         2        // Do music fading.
#define FADE_ALL           (FADE_PALETTE | FADE_MUSIC)
void fadeout(int);
void fadein(void *p);

// --- Prototypes of "face.cpp"
#define MAINMENU        0
#define SHOPMENU        4
#define SOUNDMENU       8

#define M_PLAY          1
#define M_LOAD          2
#define M_SOUND         3
#define M_NONE          99
#define M_QUIT          -1

void title(void);
int menu(int mode, int p = 0);

_ext Weapons   weapons;          // All possible Weapons.
_ext Level     level;            // All Level Data en bloc.
                                 //                  /\
                                 //                  ||
                                 //       Hey Furtknocker, this is french

_ext GameState gstate;           // Described above.
_ext PlayerState player[MAXPLAYERS];
_ext int       cheatlevel;
_ext int       start_stage;

// --- Prototypes of "gengine.cpp"
Weapon *release_weapon(int player, int n, int x, int y);
void weapon_removeall(int player);
void weaponstep(int player, int x, int y, int fire);

Enemy *release_enemy(int n, int x, int y);
void enemy_removeall(void);
void enemystep(void);
void newwave(void);

int crash(int);

// --- Prototypes of "game.cpp"
void newgame(int stage);
void shutlevel(void);
void newlevel(char *);
void shutgame(void);
void weapon_releaseall(int p, int x0, int y0);
void play(int);
// Recorder
#ifdef RECORDER
void rec_enable(void);
void rec_save(char *);
#endif
void playback_start(char *);
void playback_stop(void);
void playback_device(int &, int &, int &);
void playback_loop(void);
// Save Game I/O
SaveGame *get_savegame(void);
void put_savegame(SaveGame &);

//Shut down program
extern SDLMapLayer sMap[2];
int end_ravage(void);

#endif
