/*-------------------------------------------------------*/
/* GameManager [c]copyright by AlphaHelix                */
/*                                                       */
/*                                                       */
/*                                                       */
/*-------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>

#include "MEMORY.HPP"
#include "XLIB.HPP"
#include "GCLASS.HPP"
#include "ERROR.HPP"

// Memory Management
static Memory<Weapon>   wmem;
static Memory<Enemy>    emem;

Palette *mainPalette = &palette;

void initgclass(void)
{
// Initialize Memory system.
   wmem.init(G_MAXWEAPONS);
   emem.init(G_MAXENEMIES);
}

void shutgclass(void)
{
   wmem.shut();
   emem.shut();
}

int weapon_usage(void)
{
   return wmem.usage();
}
int enemy_usage(void)
{
   return emem.usage();
}


// ------------------------------------------------------------------
//  Weapon members.
// ------------------------------------------------------------------
Weapon::Weapon(RawWeapon *w)
{
   int   i;
   int   ptr;

// First thing to do is to copy data.
   sprite = w->sprite;
   shadow = w->shadow;

// We copy some values now.
   type = w->type;
   mode = w->mode;
   flags = w->flags;
   stay_count = w->staycount;
   power = w->power;
   freq = w->freq;
   lastx_count = stay_count;

// Initialize a few variables.
   player = 0;       // Default to player 0. (first player).
   freq_count = 0;
   lastx = 0;

// Initialize a homing missile.
   if (mode == M_HOME) {
      frames = w->sub;
      speed = w->speed;
      range = w->range;
      mass_count = mass = w->mass;
   }

#ifdef DEBUG
   if (w->npathes != W_PATHES) error("Incorrect number of pathes in Weapon::Weapon.");
#endif

// Initialize PATHES.
   for (i = 0, ptr = W_PATHES; i < W_PATHES; i++) {
      path[i].save_path =
      path[i].now =
      path[i].root = w->data[i] ? &w->data[ptr] : 0;
      ptr += w->data[i];
// Initialize some variables.
      path[i].wait = 0;
   }

}

void *Weapon::operator new(size_t s)
{
   return (void *)wmem.alloc(s);
}

void Weapon::operator delete(void *p)
{
   wmem.free((Weapon *)p);
}



// ------------------------------------------------------------------
//  WeaponPool members.
// ------------------------------------------------------------------
void WeaponPool::load(char *file)
{
   ulong   ptr;
   int    i;

// Set some pointers to easy access data.
   anchor = (ulong *) loadfile(file, NULL);
   ptr = (ulong)anchor;

// Set pointers to index.
   weapon = (RawWeapon **)&anchor[WP_STARTOFINDEX];

// Get the number of pointers to adjust.
   n = anchor[WP_NWEAPONS];

// Adjust index pointers.
   for (i = 0; i < n; i++) {
      anchor[WP_STARTOFINDEX+i] += ptr;
   }

}

// ------------------------------------------------------------------
//  Weapons members.
// ------------------------------------------------------------------
void Weapons::load(char *file)
{
   char  text[40];

// We are going to load the WEAPONS now.
   strcpy(text, file); strcat(text, ".csp");
   SpritePool::load(text, true);       // Sprites.
   strcpy(text, file); strcat(text, ".cwa");
   WeaponPool::load(text);       // Weapon Pathes.
   strcpy(text, file); strcat(text, ".cwv");
   WavePool::load(text);         // WAVs.
}

void Weapons::unload(void)
{
   WavePool::unload();
   WeaponPool::unload();
   SpritePool::unload();
}

// ------------------------------------------------------------------
//  Enemy members.
// ------------------------------------------------------------------
Enemy::Enemy(RawEnemy *e)
{
   int   i;
   int   ptr;

// Initialize a new ENEMY.
// We copy some values now.
   id = e->id; subid = e->subid;
   mode = e->mode;
   power = e->power;
   flags = e->flags;
   rock = e->rock;
   glass = e->glass;
   speed = e->speed;
   money = e->money;
   crash_steps = 0;
   z0 = z1 = 0;

#ifdef DEBUG
   if (e->npathes != E_PATHES) error("Incorrect number of pathes in Enemy::Enemy.");
#endif

// Initialize PATHES.
   for (i = 0, ptr = E_PATHES; i < E_PATHES; i++) {
      path[i].now = path[i].root = e->data[i] ? &e->data[ptr] : 0;
      ptr += e->data[i];
// Initialize some variables.
      path[i].flags = EPF_CAP0;   // Set to full capabilities.
      path[i].wait = 0;
      path[i].break_count = 1;    // Default break countdown value.
   }
// Disable execution of DEATHpath(tm).
   path[E_DEATH].now = 0;
}

void *Enemy::operator new(size_t s)
{
   return (void *)emem.alloc(s);
}

void Enemy::operator delete(void *p)
{
   emem.free((Enemy *)p);
}



// ------------------------------------------------------------------
//  EnemyPool members.
// ------------------------------------------------------------------
void EnemyPool::load(char *file)
{
   long   size;
   ulong   ptr;
   int    i;

// Set some pointers to easy access data.
   anchor = (ulong *)loadfile(file, NULL);
   ptr = (ulong)anchor;

// Set pointers to index.
   enemy = (RawEnemy **)&anchor[EP_STARTOFINDEX];

// Get the number of pointers to adjust.
   size = anchor[EP_NENEMIES];

// Adjust index pointers.
   for (i = 0; i < size; i++) {
      anchor[EP_STARTOFINDEX+i] += ptr;
   }
// Adjust money data. This is because I was too lazy to adjust every level.
   for (i = 0; i < size; i++) {
      enemy[i]->money *= 10;
   }
}


// ------------------------------------------------------------------
//  AttackPool members.
// ------------------------------------------------------------------
void AttackPool::load(char *file)
{
// Set some cool pointers.
   anchor = (ulong *)loadfile(file, NULL);

// Copy data.
   nattacks = anchor[AP_NATTACKS];

// Set pointer to attack structure array.
   attack = (Attack *)&anchor[AP_DATA];

}


// ------------------------------------------------------------------
//  Level members.
// ------------------------------------------------------------------
void Level::load(char *file)
{
   char  text[40];

// Loading all the needed stuff.
   strcpy(text, file); strcat(text, ".cpa");
   palette = (void *)loadfile(text, NULL);   // Palette
   mainPalette->set(palette);

// Load Level Description
   strcpy(text, file); strcat(text, ".cds");
   descript = (Descript *)loadfile(text, NULL);    // Description.
   strcpy(text, file); strcat(text, ".cma");
   loadmap(text);               // Map.
   strcpy(text, file); strcat(text, ".csp");
   SpritePool::load(text, true);      // Sprites.
   strcpy(text, file); strcat(text, ".cwa");
   EnemyPool::load(text);       // Enemy Pathes.
   strcpy(text, file); strcat(text, ".cat");
   AttackPool::load(text);      // Attack Table.
   if (descript->flags & DES_WAV) {
      strcpy(text, file); strcat(text, ".cwv");
      WavePool::load(text);     // WAVs.
   }
   if (descript->flags & DES_MOD) {
      strcpy(text, file); strcat(text, ".uni");
      s_loadmod(text);          // Mod file.
   }

}

void Level::unload(void)
{
   unloadfile(palette);
   if (descript->flags & DES_MOD) s_unloadmod();
   if (descript->flags & DES_WAV) WavePool::unload();
   AttackPool::unload();
   EnemyPool::unload();
   SpritePool::unload();
   unloadmap();
   unloadfile(descript);
}


