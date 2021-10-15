#ifndef AH_GCLASS
#define AH_GCLASS

#include "TYPES.H"
#include "OBJECTS.HPP"
#include "SOUND.HPP"
#include "DISKIO.HPP"


#define G_MAXWEAPONS      150  // Max weapons & friendly shots on screen.
#define G_MAXENEMIES      150  // Max enemies on screen.


// Memory management functions.
void initgclass(void);
void shutgclass(void);
int weapon_usage(void);
int enemy_usage(void);


// Weapon Definiton
#pragma pack (1)
// RawWeapon directly represents the data stored on DISK.
// So if you even make little changes to the following structure
// things won't work anymore. Be careful.
// By the way: don't use "int" as a data type. Because this is compiler
// dependent.
struct RawWeapon {
   char     name[20];         // Weapon's name.
   short    type;             // What kind of weapon do we have here?
   long     price;            // price.
   short    slot;             // Where to place the weapon.
   short    next;             // Next weapon in power-up chain.
   short    position;         // Position of weapon in SHOP LIST.
// Actual weapon data goes here.
   short    sprite;           // Sprite representing this weapon.
   short    shadow;           // Shadow sprite if any.
   short    shadowx;          // Shadow x offset.
   short    shadowy;          // Shadow y offset.
   short    sub;              // Number of SUB-SPRITES.
   short    priority;         // Priority level for Z-ordering.
   short    mode;             // Mode of operation. (Shot, homing, ..)
   short    flags;            // Some cool flags (see WEAPON FLAGS below).
   short    staycount;        // Count value for MULTI X weapons.
   short    speed;            // Speed if HOMING missile.
   short    range;            // Number of steps HOMING missile can do.
   short    mass;             // Traegheitsmoment in Homing mode.
   short    power;            // SCHLAGKRAFT.
   short    freq;             // Fire frequency.
   short    npathes;          // Number of PATHES. Must be 3.
   short    data[];           // Path data.
};

#define W_PATHES     3        // Number of Pathes in Weapon Definition.
#define W_1          0        // Base Path.
#define W_FIRE       1        // FIRE Path.
#define W_DEATH      2        // DEATHpath.
struct W_Path {
// Pointer to Instructions.
   short    *root;            // The root of the instruction stream.
   short    *now;             // Position in instruction stream NOW.
// State of the Execution Unit.
   int      flags;            // State flags.
   int      wait;
// Used by MARK & LOOP.
   int      save_x, save_y;   // Saved (x, y) position.
   short    *save_path;       // Saved command path pointer.
};

#pragma pack (4)
// Weapon Types
// Description:
// SHIP    : Can be hit by enemy fire. Usually this is the MOTHER SHIP.
// DRONE   : Can't be destroyed by enemy fire, but DRONE can hit enemies.
// SIDEKICK: Can't be hit & can't hit. VIRTUAL WEAPONS MUST BE SIDEKICKS.
// SHOT    : Can hit enemies.
// LIFE    : Extra Life. Only pricing information is needed.
#define WT_SHIP               0     // HAUPT SCHIFF
#define WT_DRONE              1
#define WT_SIDEKICK           2
#define WT_SHOT               3
#define WT_LIFE               4     // Another LIFE.
// Weapon Flags
#define WF_SHADOW             1     // Weapon has a shadow.
#define WF_MULTIX             2     // Use mutiple sprites for x movement.
#define WF_WINDOWCHECK        4     // Remove weapon if moved out of window.
#define WF_MAIN               8     // Indicating the MAIN weapon.
// Define the weapon class.
// This class holds all data of a currently active (on screen) WEAPON.
class Weapon {
public:
   Object   *object;          // Object representing this WEAPON.
   int      type;             // Weapon type. (Sidekick, Shot, Drone, ...)
   int      mode;             // Weapon mode. (Pathfollower, homing, ...)
//   int      priority;         // Priority of weapon.
   int      player;           // Weapon belongs to player xy.
   ulong    flags;            // Keep an eye on all the enhancments.
   int      power;            // Yeah.
   W_Path   path[W_PATHES];   // Instruction Traces.
// Mirrors of object.x and y.
   int      x, y;
// The next few variables are used to choose the proper sprite.
   int      sprite;           // Base Sprite used.
   int      shadow;           // Base Shadow used.
   int      stay_count;       // Traegheitsmoment.
   int      lastx;            // Last move in x direction.
   int      lastx_count;      // Count till change in movement will reflect in a change of sprite.
// Variables used to fire.
   int      freq;             // Schuss periode.
   int      freq_count;       // Waffenstillstandszaehler.
// Variables used to handle a HOMING missile.
   int      speed;            // Speed of the missile.
   int      range;            // Number of steps it can do until destruction.
   int      angle;            // Current flying angle.
   int      mass;             // Traegheit. Huh! What's this in english ??
   int      mass_count;       // Countdown value of "mass".
   int      alpha;            // Fixed point value.
   int      frames;           // Frames
public:
   Weapon   *prev, *next;     // Link to next\prev weapon.
   Weapon(RawWeapon *w);
   void     *operator new(size_t size);
   void     operator delete(void *p);
};

// Position of data in WeaponPool(WP).
// Assuming data structure is array of long.
#define WP_NWEAPONS           0
#define WP_STARTOFINDEX       1
class WeaponPool {
   ulong    *anchor;
public:
   int      n;                // Number of weapons loaded.
   RawWeapon   **weapon;      // Pointer array to weapon definition arrays.
   void     load(char *file); // Initialize weapon data structure.
   void     unload(void) { unloadfile(anchor); }
};

// Weapons class.
// All data used to keep Weapons in order.
class Weapons: public SpritePool,
               public WeaponPool,
               public WavePool
{
public:
   void     load(char *file);
   void     unload(void);
};


// Enemy definition.
#pragma pack (1)
// RawEnemy directly represents the data stored on DISK.
// So if you even make little changes to the following structure
// things won't work anymore. Be careful.
// By the way: don't use "int" as a data type. Because this is compiler
// dependent.
struct RawEnemy {
   short    sprite;           // Sprite number.
   short    shadow;           // Sprite number representing the shadow.
   short    shadowx;          // Rel.Position of shadow in x.
   short    shadowy;          // rel.Position of shadow in y direction.
   short    sub;              // Number of SUB-SPRITES.
   short    id;               // Enemy's ID code.
   short    subid;            // Enemy's sub ID code.
   short    mode;             // Mode of operation. (Homing, path, ...)
   short    priority;         // graphics Priority level.
   short    flags;            // See list of possible flags below.
   short    rock;             // Set if invincible.
   short    glass;            // Set if transparent.
   short    power;            // Shield power.
   short    speed;            // Speed of shots.
   short    money;            // Money you earn if you shoot enemy.
   short    npathes;          // Number of pathes in structure.
   short    data[];           // Custom data.
};
// Enemy Path structure.
// A path is like a small Computer.
// It holds the program to execute and stores the state of the
// CPU and it's registers.
#define E_PATHES        4     // Number of pathes allowed.
#define E_1             0     // Base Path
#define E_2             3     // Extended Path
#define E_HIT           1     // Hit path.
#define E_DEATH         2     // DEATHpath(tm)
// Path Flags. (EPF = EnemyPathFlag)
#define EPF_CAP0         0x01 // Capability level of execution unit.
struct E_Path {
// Pointer to Instructions.
   short    *root;            // The root of the instruction stream.
   short    *now;             // Position in instruction stream NOW.
// State of the Execution Unit.
   int      flags;            // State flags.
   int      wait;
   int      break_count;      // LOOP_BREAK countdown.
// Used by MARK & LOOP.
   int      save_x, save_y;   // Saved (x, y) position.
   short    *save_path;       // Saved command path pointer.
};

// Enemy flags
#define EF_SHADOW             0x01  // Draw shadow.
#define EF_GROUND             0x02  // This one is a ground unit.
#define EF_BOSS               0x08  // Set if this is a BOSS.
#define EF_WRECK              0x10  // Enemy is wreck. (Not much action anymore)
#define EF_VIRTUAL            0x20  // Enemy is virtual. (no object)
// Define the enemy class.
// This class holds all data of a currently active (on screen) FUZZY.
class Enemy {
public:
// General variables.
   Object   *object;          // Enemy controls this object.
   int      mode;             // (Path follower, Line shot, homing, ...)
   int      id;               // Unique enemy id number.
   int      subid;            // Sub ID.
   int      power;            // Yeah.
   int      money;            // How much is it worth ?
// The FLAGS.
   int      flags;            // Some bit flags.
   int      rock;             // Power counter won't be decreased on a hit.
   int      glass;            // No crash with SHOTS on this.
   int      layer;            // Moving on which layer.
// Execution pathes.
   E_Path   path[E_PATHES];   // All stuff needed to handle a path.
// Used by CRASH command.
   int      crash_steps;      // Steps still to do.
   int      crash_sprite;     // Next sprite to display.
   int      crash_wait;       // Wait between crash frames.
   int      crash_waitcount;  // Wait countdown.
   int      crash_dx, crash_dy;  // Shadow movements.
// May be used to save the position of the object.
   int      x, y;
// Used by SHOT mode. (to draw a line).
   int      speed;            // Speed of enemy if shot.
   int      dx, dy, dz, z0, z1;
   Enemy    *prev, *next;     // Link to next\prev enemy.
// Member Functions.
   Enemy(RawEnemy *e);
   void     *operator new(size_t size);
   void     operator delete(void *p);
};

// Position of data in EnemyPool(EP).
// Assuming data structure is array of long.
#define EP_NENEMIES           0
#define EP_STARTOFINDEX       1
class EnemyPool {
   ulong    *anchor;
public:
   RawEnemy **enemy;          // Pointer array to enemy\weapon definition arrays.
   void     load(char *file);
   void     unload(void) { unloadfile(anchor); }
};

// Attack structure. Defines the way the enemies will attack.
// This is a direct map of the image saved on disk.
// So don't change a single thing of this Attack Structure definiton.
// ..or things easily might go wrong.
struct Attack {
   long     count;            // Frame counter value for enemy release.
   long     command;          // What to do this time ?
   long     level;            // Release at which difficulty level ?
   long     enemy;            // Enemy to release.
   long     id;               // Enemy's unique ID.
   long     wait;             // Initial WAIT for ground units.
   long     layer;            // Ground unit moves on layer 0 or 1.
   long     x, y;             // (x, y) of release.
};

// Definition of AttackPool class and related constants.
// Assume data array is of type long.
#define AP_NATTACKS           0
#define AP_DATA               1
class AttackPool {
   ulong    *anchor;
public:
   int      nattacks;         // Number of attacks in array.
   Attack   *attack;          // Pointer to Attack array.
   void     load(char *file);
   void     unload(void) { unloadfile(anchor); }
};

// Description.
// Structure tells you some general stuff about the LEVEL.
// Structure is an exact image of the data stored on disk.
// Think before you make any changes.
#define DES_TEXT              40
#define DES_WAV               0x01  // Wave base is included.
#define DES_MOD               0x02  // Level has own MOD.
struct Descript {
   long     level;            // Level number.
   char     text[DES_TEXT];   // Level Text (eg: "SPACE INVADERS").
   long     bigboss_count;    // Number of Big Bosses to kill.
   long     done_delay;       // How long to continue after killing BOSS.
   long     money;            // Bonus money after completing level.
   long     flags;            // Some flags. (see DES_xxx for help)
};

// What actually IS a level ?
// Well. This is easy. A level is the following class.
class Level :  public SpritePool,
               public EnemyPool,
               public AttackPool,
               public WavePool
{
public:
   Descript *descript;
   void     *palette;            // Palette used during gameplay.
// Functions.
   void     load(char *file);
   void     unload(void);
};


// Weapon\Enemy mode of operation.
#define M_PATH          0        // Path only follower.
#define M_SHOT          1        // Directed shot.
#define M_HOME          2        // Homing missile.
#define M_WEAPON        3        // Only useful for Main Weapons.

// Array commands.
#define C_BASE          0x1000   // Command BASE value.
#define C_END           0x1001   // End path of enemy. (id)
#define C_RELEASE       0x1002   // Release another enemy (n, dx, dy, level)
#define C_WAIT          0x1003   // Wait (n) frames.
#define C_MARK          0x1004   // Mark current position for LOOP.
#define C_LOOP          0x1005   // Loop back to last MARKed position. (mode)
#define C_RELEASEOBJ    0x1006   // Release Object (spr, dx, dy, flags).
#define C_REMOVEOBJ     0x1007   // Remove object from screen.
#define C_STOPPATH      0x1008   // Stop PATH execution of object. (id, path)
#define C_HIT           0x1009   // Simulate a hit of enemy. (enemy's ID).
#define C_INCGLASS      0x100a   // Inc enemy's GLASS counter. (id)
#define C_DECGLASS      0x100b   // Dec enemy's GLASS counter. (id)
#define C_SOUND         0x100c   // Make some noise. (sound, level)
#define C_CHGSPRITE     0x100d   // Just change object apperance.
#define C_CHGSANDS      0x100e   // Change SPRITE and SHADOW.
#define C_CRASH         0x100f   // Crash object. (sprite, steps, wait)
#define C_KILL          0x1011   // Kill object(ID). (Execute DEATHpath)
#define C_SHADOW        0x1015   // Shadow control (arg1, arg2, arg3) see below.
#define C_ANIMATE       0x1016   // Sprite animation control. (sub command)
#define C_CRISPY        0x1017   // Do crispy effect on object. (effect,speed)
#define C_SETFLAG       0x1019   // Set flag. (flag, value) see below.
#define C_BREAK         0x101a   // Break next loop statement. (cmd, id, path, value)

// Commands used in AttackTable only.
#define C_UNMARK        0x1012   // Unmark
#define C_CYCLEPAL      0x1013   // Cycle Palette (c1, c2, speed)
#define C_MAPSPEED      0x1018   // Set background layer speed (layer, speed)

// Sub Commands.
// C_BREAK
#define BREAK_DO        0x0000
#define BREAK_SETCOUNT  0x0001

// C_LOOP
#define LOOP_FULL       0x0000
#define LOOP_SMALL      0x0001

// C_SETFLAG
#define SETFLAG_WRECK   0x0000

// C_CRISPY
#define CRISPY_SET      0x0000
#define CRISPY_DO       0x0001

// C_SHADOW
#define SHADOW_OFF      0x0000   // Turn off shadow.
#define SHADOW_ON       0x0001   // Turn on shadow.
#define SHADOW_MOVE     0x0002   // Move shadow.

#endif

