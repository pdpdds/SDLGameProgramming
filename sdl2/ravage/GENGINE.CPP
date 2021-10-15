
/*-------------------------------------------------------*/
/*                                                       */
/* Game Engine [c] copyright 1996 by AlphaHelix          */
/*                                                       */
/*                                                       */
/*                                                       */
/*-------------------------------------------------------*/

/*
 Every enemy is defined like this:

 Enemy -> Object -> Sprite

 To clear some confusion I explain things.

 Virtual:
 If an enemy is VIRTUAL it has NO OBJECT (and no sprite).
 However it is still a full blown enemy which will act
 as it would have an objects. The only thing not being
 done with a virtual enemy is CRASH test. This means you
 can't shot a virtual enemy and you can't be hit by a
 virtual enemy.

 Wreck:
 If an enemy is WRECK it has been killed by your shots or
 through a dedicated KILL command. A wrecked enemy is
 still on screen and performs all actions according
 to its instruction traces. A wrecked enemy (like a virtual
 one) won't go through CRASH detection anymore.
*/

#include <stdlib.h>

#include "ERROR.HPP"
#include "MEMORY.HPP"
#include "SOUND.HPP"
#include "SDL_BASE.HPP"
#include "SOUND.HPP"
#include "SOUND_SDL.HPP"
#include "RAVAGE.HPP"


//---------------------------------------------------------
// General purpose functions & macros.
//---------------------------------------------------------

/*---------------------------------------------------------
 Important note:
 All of the following trigonometric functions operate
 on fix point values in the form "0.00".
---------------------------------------------------------*/
#define SINTABLESIZE       12
#define SINRADSTEPS        (2*314 / SINTABLESIZE)
static const int sin_table[SINTABLESIZE] = {
   0, 50, 86, 100, 86, 50, 0, -50, -86, -100, -86, -50
};

inline static int mysin(int rad)
{
   #ifdef DEBUG
      if (rad >= 2*314) error("mysin: Argument out of range.");
      if (rad < 0) error("mysin: Argument out of range.");
   #endif
   return sin_table[(rad + SINRADSTEPS/2) / SINRADSTEPS];
}

inline static int mycos(int rad)
{
   rad = (rad + SINRADSTEPS/2) / SINRADSTEPS;
   if ((rad+=SINTABLESIZE/4) >= SINTABLESIZE) rad -= SINTABLESIZE;
   return sin_table[rad];
}

inline static int myatan2(int y, int x)
{
   int   beta;
// Calculate atan using a chebyshev approximation.
   if (abs(y) > abs(x)) {
      beta = 314/2 - (83*x/y);
   } else {
      beta = (83*y/x);
   }
   if (beta < 0) beta += 314;
   if ((y<0) || ((y==0) && (x<0))) beta += 314;

   return beta;
}

// Some rather COOOOOL macros.
#define  max(a, b)   (a > b ? a : b)
#define  min(a, b)   (a < b ? a : b)

inline static int sgn(int x)
{
   if (x == 0) return 0;
   return (x > 0 ? 1 : -1);
}

//---------------------------------------------------------
// Weapon specifics
//---------------------------------------------------------

// remove weapon from queue.
static void remove(Weapon *w)
{
// remove Weapon's OBJECT first.
   if (w->object) delete w->object;
// remove WEAPON itself.
   if (w->prev) w->prev->next = w->next; else gstate.weapon[w->player] = w->next;
   if (w->next) w->next->prev = w->prev;
   delete w;
}

void weapon_removeall(int player)
{
   while (gstate.weapon[player]) remove(gstate.weapon[player]);
}

/*---------------------------------------------------------
 Function:

 Description:
 Release a WEAPON.
 A WEAPON is just a friendly object. It may be a SHIP,
 a SIDEKICK or a SHOT.
---------------------------------------------------------*/
Weapon *release_weapon(int player, int n, int x, int y)
{
   Weapon      *w;
   RawWeapon   *raw = weapons.weapon[n];
   Object      *obj;

// Define Weapon data structure.
   w = new Weapon(raw);
   w->player = player;           // This one belongs to player soUNDso.
   if (raw->sprite == -1) {
// Weapon has no object.
      #ifdef DEBUG
         if (w->type != WT_SIDEKICK) error("Virtual Weapon MUST be Sidekick.");
      #endif
      w->object = 0;
      w->x = x; w->y = y;
   } else {
// Add object & shadow.
      obj = w->object = new Object(weapons.sprite[raw->sprite], x, y, raw->priority | O_POOL);
      if (w->flags & WF_SHADOW) {
         obj->changeshadow(weapons.sprite[raw->shadow], raw->shadowx, raw->shadowy);
      }
// Set proper frame if this is a HOMING missile.
      if (w->mode == M_HOME) {
         w->angle = w->frames / 4;
         w->alpha = 2*314 / w->frames;
         obj->chgspr_center(weapons.sprite[w->sprite+w->angle]);
      }
   }
// Link it to existing list.
   if ((w->next = gstate.weapon[player]) != NULL) gstate.weapon[player]->prev = w;
   gstate.weapon[player] = w;
   w->prev = NULL;

   return w;
}

/*---------------------------------------------------------
 Function:

 Description:
 This function moves a weapon or a virtual weapon.
 A virtual weapon is a weapon with NO associated object.
 Please note: Virtual weapons can't be tested for
 off-screen movement.
 Return:
 0 : everything is alright.
 1 : Weapon has been removed.
---------------------------------------------------------*/
static int step_path(Weapon *w, int x, int y)
{
   W_Path   *path = &w->path[W_1];
   short    *cmd = path->now;
   Object   *obj = w->object;
   Sprite   *spr;
   int      again = 2;

// First we select the proper sprite sequence.
   if ((w->flags & WF_MULTIX) && (w->lastx != sgn(x))) {
      #ifdef DEBUG
         if (!obj) error("obj==0 in step_path at pos 0.");
      #endif
      if (--w->lastx_count == 0) {
         if (sgn(x)) w->lastx += sgn(x); else w->lastx = sgn(x);
         switch (w->lastx) {
         case 0:  // Stay
            obj->changesprite(weapons.sprite[w->sprite], 0, 0);
            obj->changeshadow(weapons.sprite[w->shadow], 0, 0);
            break;
         case -1: // Left
            obj->changesprite(weapons.sprite[w->sprite+1], 0, 0);
            obj->changeshadow(weapons.sprite[w->shadow+1], 0, 0);
            break;
         case 1:  // Right
            obj->changesprite(weapons.sprite[w->sprite+2], 0, 0);
            obj->changeshadow(weapons.sprite[w->shadow+2], 0, 0);
            break;
         }
         w->lastx_count = w->stay_count;
      }
   } else {
      w->lastx_count = w->stay_count;
   }

   if (w->mode == M_PATH) { x = 0; y = 0; }

// Move object.
   if (obj) {
      x += obj->x; y += obj->y;
// Border check. Remove object if moved off screen.
      if (w->flags & WF_WINDOWCHECK) {
         spr = obj->sprite;
         if ((x+spr->xs<0) || (y+spr->ys<0) || (x>XMAX) || (y>YMAX)) {
            remove(w);
            return 1;
         }
      }
   } else {
      x += w->x; y += w->y;
   }

// Process PATH.
   if (cmd) {
      do {
         if (*cmd < C_BASE) {
            if (again == 2) {
               x += cmd[0]; y += cmd[1];
               cmd += 2;
            }
            again--;
         } else {
            switch (cmd[0]) {
            case C_END:          // Remove Weapon from screen & memory.
                                 // (dummy)
               remove(w);        // Weapon is no longer needed.
               return 1;

            case C_MARK:         // Mark path position for LOOPing.
                                 // ()
               path->save_path = ++cmd;
               break;

            case C_LOOP:         // Loop TRACE.
                                 // (dummy)
               cmd = path->save_path;      // Loop path.
               break;

            case C_RELEASEOBJ:   // Release Object
                                 // (sprite, delta_x, delta_y, flags)
               new Object(weapons.sprite[cmd[1]], x+cmd[2], y+cmd[3], cmd[4] | O_POOL);
               cmd += 5;
               break;

            default:
               error("Unknown command in W_1.");
               break;
            }
         }
      } while (again);
      path->now = cmd;
   }

// Finaly move the object (or at last the virtual object.)
   w->x = x; w->y = y;
   if (obj) { obj->x = x; obj->y = y; }

   return 0;
}

// Execute FIRE instruction trace.
// return:
// 0 : o.k.
// 1 : weapon has been removed.
// IMPORTANT WARNING: function "crash" assumes that this function
// always returns 0. So rewrite "crash" if this is no longer true.
static int step_fire(Weapon *w, int fire)
{
   W_Path   *path = &w->path[W_FIRE];
   short    *cmd;
   int      again = 1;

// Reload guns if neccessary.
   if (w->freq_count > 0) w->freq_count--;
// Check whether to start a new FIREpath or to continue the old one.
   if (fire && !w->freq_count) {
      cmd = path->root;
      w->freq_count = w->freq;
   } else {
      cmd = path->now;
   }

// Feuer frei.
   if (!cmd) return 0;

// Test if on WAIT.
   if (path->wait) {
      path->wait--;
      return 0;
   }
// Execute Instruction Stream.
   do {
      switch (*cmd) {
      case C_END:       // End of fire path reached. Stop execution.
                        // (dummy)
         cmd = 0;
         again = 0;
         break;

      case C_WAIT:      // Insert Wait states.
                        // (wait_count)
         path->wait = cmd[1];
         again = 0;
         cmd += 2;
         break;

      case C_RELEASE:   // Release new Weapon (usually this will be a SHOT).
                        // (weapon, delta_x, delta_y, difficulty)
         release_weapon(w->player, cmd[1], w->x+cmd[2], w->y+cmd[3]);
         cmd += 5;
         break;

      case C_SOUND:     // Play sample.
                        // (sample#)
         weapons.play(cmd[1]);
         cmd += 3;
         break;

      default:
         error("Invalid Instruction in FIREpath.");
         break;
      }
   } while (again);
   path->now = cmd;

   return 0;
}

// This is the last thing a WEAPON can do: The DEATH stream.
// return:
// 0 : everything o.k.
// 1 : Weapon has been removed.
static int step_death(Weapon *w)
{
   W_Path   *path = &w->path[W_DEATH];
   short    *cmd = path->now;
   Object   *obj = w->object;
   int      again = 1;

// If no instructions available: just remove WEAPON from this world.
   if (!cmd) {
      remove(w);
      return 1;
   }

// Execute stream.
   do {
      switch (*cmd) {

      case C_END:             // remove weapon from memory and screen.
                              // (dummy)
         remove(w);
         return 1;

      case C_RELEASEOBJ:      // release object.
                              // (sprite, delta_x, delta_y, flags)
         #ifdef DEBUG
         if (!obj) error("Tried to execute weapon DEATH with object==0.");
         #endif
         new Object(weapons.sprite[cmd[1]], obj->x+cmd[2], obj->y+cmd[3], cmd[4] | O_POOL);
         cmd += 5;
         break;

      case C_SOUND:     // Play sample.
                        // (sample#)
         weapons.play(cmd[1]);
         cmd += 3;
         break;

      default:
         error("Command not supported in Weapon DEATHpath(tm).");
         break;
      }
   } while (again);

   return 0;
}

/*---------------------------------------------------------
 Function:

 Description:
 Do a step for the HOMING missile.
 This is:
 - Select a target.
 - Aim at target.
 Return:
 0 : It's ok.
 1 : Homing missile has been destroyed.
---------------------------------------------------------*/
static int step_home(Weapon *w)
{
   Enemy    *e;
   Object   *obj = w->object;
   Sprite   *spr = NULL;
   int      x0, y0;              // "Now" coordinates of missile.
   int      x, y;                // Target coordinates.
   int      angle;               // Requested angle of missile.
   int      beta;                // Fixed point value.

// Load current coordinates.
   x0 = obj->x;
   y0 = -obj->y;

// Choose target and calculate exact coordinates.
   {
      int   bad;
// Start with enemy longest on screen.
      e = gstate.enemy_tail;
      while (e) {
         bad = (e->flags&(EF_WRECK|EF_VIRTUAL)) || e->glass;
         if (!bad) {
            #ifdef DEBUG
               if (!e->object) error("(step_home): object == NULL.");
            #endif
            spr = e->object->sprite;
            if ((e->x+spr->xs>0) && (e->y+spr->ys>0)
               && (e->x<XMAX) && (e->y<YMAX)) break;
         }
         e = e->prev;
      }
   }
// Target is enemy "e".
   if (e) {
      x = e->x + (spr->xs-obj->sprite->xs) / 2;
      y = -(e->y + (spr->ys-obj->sprite->ys) / 2);

      x -= x0; y -= y0;
// Calculate next step.
      if ((--w->mass_count<0) && ((x!=0) || (y!=0))) {
         w->mass_count = w->mass;
         beta = myatan2(y, x);
         angle = ((beta+w->alpha/2) / w->alpha);
// Limit turn speed.
         if (angle != w->angle) {
            if (angle-w->angle > 0) {
               if (angle-w->angle > w->frames/2) w->angle--; else w->angle++;
            } else {
               if (angle-w->angle < -(w->frames/2)) w->angle++; else w->angle--;
            }
            if (w->angle >= w->frames) w->angle = 0;
            if (w->angle < 0) w->angle = w->frames-1;
         }
         obj->chgspr_center(weapons.sprite[w->sprite+w->angle]);
      }
   }
// Move missile.
   x0 += w->speed*mycos(w->angle*w->alpha) / 100;
   y0 = -(y0 + w->speed*mysin(w->angle*w->alpha) / 100);

   obj->x = x0;
   obj->y = y0;

// Remove missile if moved off-screen.
   if (w->flags & WF_WINDOWCHECK) {
      spr = obj->sprite;
      if ((x0+spr->xs<0) || (y0+spr->ys<0) || (x0>XMAX) || (y0>YMAX)) {
         remove(w);
         return 1;
      }
   }

// Remove Missile if exhausted.
   if (--w->range == 0) return step_death(w);

   return 0;
}


// Push all Weapons.
void weaponstep(int p, int x, int y, int fire)
{
   Weapon   *w, *wn;
   Sprite   *spr;
   int      x0, y0;
   int      ret;

// Test for off screen moving.
   w = player[p].main;
   if (w) {
      spr = w->object->sprite;
      x0 = w->object->x; y0 = w->object->y;
      if (x0+x < 0) x = -x0; else if (x0+x+spr->xs > XMAX) x = XMAX-(x0+spr->xs);
      if (y0+y < 0) y = -y0; else if (y0+y+spr->ys > YMAX) y = YMAX-(y0+spr->ys);
   }

   w = gstate.weapon[p];
   while (w) {
      wn = w->next;
      switch (w->mode) {
      case M_PATH:
      case M_WEAPON:
         ret = step_path(w, x, y);
         break;
      case M_HOME:
         ret = step_home(w);
         break;
      }
      if (!ret) step_fire(w, fire);
      w = wn;
   }
}


//=========================================================//
//---------------------------------------------------------//
// Enemy specifics                                         //
//---------------------------------------------------------//
//=========================================================//

// Remove enemy from queue.
static void remove(Enemy *e)
{
// First we kill the object.
   if (e->object) delete e->object;
// Now for the enemy itself.
   if (e->prev) e->prev->next = e->next; else gstate.enemy_head = e->next;
   if (e->next) e->next->prev = e->prev; else gstate.enemy_tail = e->prev;
   delete e;
   gstate.enemy_count--;
}

void enemy_removeall(void)
{
   while (gstate.enemy_head) remove(gstate.enemy_head);
}

// Release one enemy.
Enemy *release_enemy(int n, int x, int y, int layer)
{
   Enemy    *e;
   RawEnemy *raw = level.enemy[n];

// Define Enemy.
   e = new Enemy(raw);
   e->x = x; e->y = y;
// Glue it to given layer.
   if (e->flags & EF_GROUND) e->layer = layer; else e->layer = O_NOLAYER;
// To life: Add shape & shadow to enemy.
// Don't add object if enemy is VIRTUAL.
   if (e->flags & EF_VIRTUAL) {
      e->object = 0;
   } else {
      e->object = new Object(level.sprite[raw->sprite], x, y, raw->priority);
      e->object->setlayer(layer);
// Define SHADOW.
      if (e->flags & EF_SHADOW) {
         e->object->changeshadow(level.sprite[raw->shadow], raw->shadowx, raw->shadowy);
      }
// Stop animation if this is a GROUND unit.
      if (e->flags & EF_GROUND) e->object->animation(0);
   }
// Link it to existing list.
   if ((e->next = gstate.enemy_head) != NULL)
      gstate.enemy_head->prev = e;
   else
      gstate.enemy_tail = e;
   gstate.enemy_head = e;
   e->prev = NULL;
   gstate.enemy_count++;
   return e;
}


// Search the ENEMY QUEUE for an enemy with identification number ID.
Enemy *search_enemy(int id, int subid)
{
   Enemy  *e;

// Search enemy with same base ID.
   e = gstate.enemy_head;
   while (e) {
      if ((e->id==id) && (e->subid==subid)) return e;
      e = e->next;
   }
// Can't find. Now try to find enemy with any ID.
   e = gstate.enemy_head;
   while (e) {
      if (e->subid == subid) return e;
      e = e->next;
   }
   return NULL;
}

// This one calculates the next step of a DIRECTED SHOT.
// Return:
// 0 : ok.
// 1 : enemy has been removed.
static int step_line(Enemy *e)
{
   static   int target;          // Aim at which player's ship.
   Weapon   *w;
   int      x1, y1, x2, y2;
   int      dx, dy, dz;
   Sprite   *spr;
   int      i;

// Get launch coordiantes.
   x1 = e->object->x; y1 = e->object->y;
   dx = e->dx; dy = e->dy; dz = e->dz;


// See if we already have aimed to HAUPTSCHIFF
   if (e->z0 == 0) {

// Choose target.
      i = gstate.nplayers;
      do {
         if (++target >= gstate.nplayers) target = 0;
         i--;
      } while (!player[target].active && i >= 0);
// Don't go any further if no target is present.
      if (i < 0) {
         remove(e);
         return 1;
      }

      w = player[target].main;

      if (w) {
// Load target coordinates.
         spr = w->object->sprite;
         x2 = w->object->x + spr->x_core;
         y2 = w->object->y + spr->y_core;
      } else {
         x2 = XMAX / 2;
         y2 = YMAX;
      }
      e->dx = dx = abs(x1 - x2);
      e->dy = dy = abs(y1 - y2);
      if (x1 < x2) e->z0 = 1; else e->z0 = -1;
      if (y1 < y2) e->z1 = 1; else e->z1 = -1;
// Adjust speed.
// Approximate i = sqrt(dx^2 + dy^2)
      i = max(dx, dy);
      i = max(i, 7*i/8 + min(dx, dy)/2);
      if (dx > dy) {
         dz = dx/2;
         e->speed = (e->speed * dx) / i;
      } else {
         dz = dy/2;
         e->speed = (e->speed * dy) / i;
      }
//      if (e->speed == 0) e->speed = 1;
   }

// Move a bit.
   for (i = 0; i < e->speed; i++) {
      if (dz < dx) { dz += dy; x1 += e->z0; }
      if (dz >= dx) { dz -= dx; y1 += e->z1; }
   }

// Check for off screen and Move object to new position.
   spr = e->object->sprite;
   if ((x1+spr->xs<0) || (y1+spr->ys<0) || (x1>XMAX) || (y1>YMAX)) {
      remove(e);
      return 1;
   } else {
      e->object->x = x1;
      e->object->y = y1;
      e->dz = dz;
   }
   return 0;
}


/*---------------------------------------------------------
 Function:

 Description:
 Execute next instructions of Enemy's path p.
 p is one of the following:
 E_1     : Base Path.
 E_2     : Extended Path.
 E_HIT   : Hit Path.
 E_DEATH : DEATHpath(tm)

 Return value:
 0 : okay
 1 : enemy has been removed.
---------------------------------------------------------*/
static int step_path(Enemy *e, int p)
{
   E_Path   *path = &e->path[p];
   short    *cmd  = path->now;
   Object   *obj  = e->object;
   Enemy    *t_e;                // Temporary enemy pointer.
   Object   *t_o;                // Temporary object pointer.
   int      again = 1;

// Check for off screen and remove object if necessary.
   if ((e->flags&EF_GROUND) && obj) {
      if (obj->y > YMAX) {
         remove(e);
         return 1;
      }
   }

   if (!cmd) return 0;

// Test if on crash course.
// A crash instruction is only allowed in DEATHpath(tm)
   if ((p == E_DEATH) && (e->crash_steps)) {
// Test if still on wait.
      if (--e->crash_waitcount < 0) {
// Folgend sehen wir ein riesen PUFF.
         Sprite *sprite;
         int    dx, dy;
// Change sprite to next crash sprite.
         sprite = level.sprite[e->crash_sprite++];
         dx = (obj->sprite->xs - sprite->xs) / 2;
         dy = (obj->sprite->ys - sprite->ys) / 2;
         obj->changesprite(sprite, dx, dy);
// Move shadow.
         obj->shadowx -= e->crash_dx;
         obj->shadowy -= e->crash_dy;
// Get wait information and initialize next wait.
         e->crash_waitcount = e->crash_wait;
// At last a bit of housekeeping.
         e->crash_steps--;
// If last crash step, enemy becomes a ground unit.
         if (!e->crash_steps) {
            obj->setlayer(e->layer = 0);       // Default to base layer.
         }
      }
//      return 0;
   }

// Test if on wait.
   if (path->wait) {
      path->wait--;
      return 0;
   }

// Refresh objects position.
   if (obj) {
      e->x = obj->x; e->y = obj->y;
   }
// Execute Instruction Stream.
   do {
// Check if this is a movement command.
      if (*cmd < C_BASE) {
         e->x += cmd[0]; e->y += cmd[1];
         if (obj) {
            obj->x = e->x; obj->y = e->y;
         }
         cmd += 2;
         again = 0;
      } else {
// All other commands are executed next.
         switch (*cmd) {
         case C_END:       // Quietly remove enemy from queue.
                           // (enemy_subid)
            if (!cmd[1]) { remove(e); return 1; }
            if ((t_e = search_enemy(e->id, cmd[1])) != 0) {
               remove(t_e);
            }
            cmd += 2;      // Hit the road again.
            break;

         case C_KILL:      // Start DEATHpath(tm).
                           // (enemy_subid)
            if ((t_e = search_enemy(e->id, cmd[1])) != 0) {
               t_e->path[E_DEATH].now = t_e->path[E_DEATH].root;
// Stop some CAPABILITIES of base and ext PATH.
               t_e->path[E_1].flags &= ~EPF_CAP0;
               t_e->path[E_2].flags &= ~EPF_CAP0;
               t_e->flags |= EF_WRECK;      // Avoid further crash detection.
            }
            cmd += 2;
            break;

         case C_REMOVEOBJ: // Remove object from screen but continue PATH
                           // ()
// Remove object from screen, from memory, from mind.
            delete obj; obj = e->object = 0;
            e->flags |= EF_VIRTUAL;    // It's virtual now.
            e->path[E_1].now = 0;      // Stop executioon of PATH.
            e->path[E_2].now = 0;
            cmd++;
            break;

         case C_STOPPATH:  // Stop execution of given path.
                           // (enemy_subid, path)
// First look for the enmey we have to stop.
            if (cmd[1]) t_e = search_enemy(e->id, cmd[1]); else t_e = e;
            if (t_e) {
               t_e->path[cmd[2]].now = 0;
               cmd += 3;
               if (!e->path[p].now) { cmd = 0; again = 0; }
            } else {
               cmd += 3;
            }
            break;

         case C_MARK:      // Mark current position for a LOOP.
                           // ()
// Save current position of PATH 0. Note always path 0 is used to MARK & LOOP.
            path->save_x = e->x;
            path->save_y = e->y;
            path->save_path = ++cmd;         // Increment IP and store it.
            break;

         case C_LOOP:      // Loop Back to MARKED position.
                           // (loop_mode)
// Don't loop if BREAK bit is set
            if (path->break_count > 0) {
               if (cmd[1] == LOOP_FULL) {
                  obj->x = e->x = path->save_x; // restore previous saved (x,y).
                  obj->y = e->y = path->save_y;
               }
               cmd = path->save_path;           // Restore IP.
               again = 0;
            } else {
               cmd += 2;         // Just jump to next instruction.
            }
            break;

         case C_WAIT:      // Wait a little bit before continue execution.
                           // (wait_count)
            path->wait = cmd[1];       // Load wait counter.
// Stop animating object if this is a GROUND unit.
            //if (e->flags & EF_GROUND) obj->stopanimation();
            cmd += 2;
            again = 0;
            break;

         case C_RELEASE:   // Release another enemy.
                           // (enemy, delta_x, delta_y, difficulty)
            if ((path->flags&EPF_CAP0) && (gstate.difficulty>=cmd[4])) {
               release_enemy(cmd[1], e->x + cmd[2], e->y + cmd[3], e->layer);
            }
            cmd += 5;
            break;

         case C_RELEASEOBJ:   // Release a object.
                              // (sprite, delta_x, delta_y, flags)
            t_o = new Object(level.sprite[cmd[1]],
                      e->x+cmd[2], e->y+cmd[3], cmd[4]);
            t_o->setlayer(e->layer);
            cmd += 5;
            break;

         case C_INCGLASS:  // Increment GLASS flag by 1.
                           // (enemy_subid)
            if (!cmd[1]) e->glass++;
            else if ((t_e = search_enemy(e->id, cmd[1])) != NULL) {
               t_e->glass++;
            }
            cmd += 2;      // Hit the road again.
            break;

         case C_DECGLASS:  // Decrement GLASS flag by 1.
                           // (enemy_subid)
            if (!cmd[1]) e->glass--;
            else if ((t_e = search_enemy(e->id, cmd[1])) != NULL) {
               if (t_e->glass > 0) t_e->glass--;
            }
            cmd += 2;      // On to the next command.
            break;

         case C_SOUND:     // Play sample.
                           // (sample#)
            if ((path->flags&EPF_CAP0) && (gstate.difficulty>=cmd[2])) {
               level.play(cmd[1]);
            }
            cmd += 3;
            break;

         case C_CHGSPRITE: // Change Sprite.
                           // (sprite, delta_x, delta_y)
            if (path->flags & EPF_CAP0) {
// Adjust MARKED position.
               e->x += cmd[2]; e->y += cmd[3];
               e->path[0].save_x += cmd[2]; e->path[0].save_y += cmd[3];
// Change sprite.
               obj->changesprite(level.sprite[cmd[1]], cmd[2], cmd[3]);
            }
            cmd += 4;
            break;

         case C_CHGSANDS:  // Change Sprite AND Shadow.
                           // (sprite, shadow, delta_x, delta_y)
            if (path->flags & EPF_CAP0) {
// Adjust MARKED position.
               e->x += cmd[3]; e->y += cmd[4];
               e->path[0].save_x += cmd[2]; e->path[0].save_y += cmd[3];
               obj->changesprite(level.sprite[cmd[1]], cmd[3], cmd[4]);
               obj->changeshadow(level.sprite[cmd[2]], 0, 0);
            }
            cmd += 5;
            break;

         case C_CRASH:     // Prepare crash.
                           // (1st_sprite, n_steps, wait)
   // Get all needed information and prepare a beautiful crash.
            e->crash_sprite = cmd[1];
   // Calculate shadow movement speed.
            e->crash_steps = cmd[2];
            e->crash_dx = obj->shadowx / e->crash_steps;
            e->crash_dy = obj->shadowy / e->crash_steps;
   // Get wait information and start first wait sequence.
            e->crash_waitcount = e->crash_wait = cmd[3] + 1;
   // At last a bit of housekeeping.
            cmd += 4;
            again = 0;
            break;

         case C_SHADOW:    // Don't display object's shadow anymore.
                           // (subcommand, arg1, arg2)
            switch (cmd[1]) {
            case SHADOW_OFF:
               obj->changeshadow(0);
               break;
            case SHADOW_ON:
               obj->changeshadow(level.sprite[cmd[2]]);
               break;
            case SHADOW_MOVE:
               obj->moveshadow(cmd[2], cmd[3]);
               break;
            default:
               error("Shadow command not implemented.");
            }
            cmd += 4;
            break;

         case C_ANIMATE:   // Object animation control.
                           // (command)
            obj->animation(cmd[1]);
            cmd += 2;
            break;

         case C_CRISPY:    // Do CRISPY effect on object.
                           // (effect, (effect dependent data))
            switch (cmd[1]) {
            case CRISPY_SET:
               obj->crispy_set(cmd[2]);
               break;
            case CRISPY_DO:
               obj->crispy_do(cmd[2]);
               break;
            default:
               error("Crispy effect not implemented.");
            }
            cmd += 3;
            break;

         case C_SETFLAG:   // Set a ENEMY FLAG.
                           // (flag, value)
            switch (cmd[1]) {
            case SETFLAG_WRECK:
               if (cmd[2]) e->flags |= EF_WRECK; else e->flags &= ~EF_WRECK;
               break;
            }
            cmd += 3;
            break;

         case C_BREAK:     // Break a LOOP and continue execution of stream
                           // right after the next LOOP command.
                           // (id, path)
            if (!cmd[2]) t_e = e; else t_e = search_enemy(e->id, cmd[2]);
            if (t_e) {
               switch (cmd[1]) {
               case BREAK_DO:
                  t_e->path[cmd[3]].break_count--;
                  break;
               case BREAK_SETCOUNT:
                  t_e->path[cmd[3]].break_count = cmd[4];
                  break;
               }
            }
            cmd += 5;      // On to the next command.
            break;

         default:
            error("Path command not implemented");
         }
      }
   } while (again);

   path->now = cmd;   // Update instruction pointer.

   return 0;
}


// Execute instructions from HIT stream.
// on return:
// 0 : cool.
// 1 : enemy removed from queue.
static int step_hit(Enemy *b, Enemy *e, int power, int p)
{
   short  *cmd = e->path[E_HIT].now;
   Enemy  *t_e;
   int    again = 1;

   e->object->flash();    // Flash me, baby.
// If enemy is already dead: Just FLASH will do the job.
   if (e->power <= 0) return 0;
// Enemy is alive: But can it stand the immense POWER of our shot ?
   if ((e->power -= power) <= 0) {
// Initiate enemy's DEATH.
      e->path[E_DEATH].now = e->path[E_DEATH].root;
// Stop some CAPABILITIES of base and ext PATH.
      e->path[E_1].flags &= ~EPF_CAP0;
      e->path[E_2].flags &= ~EPF_CAP0;
      e->flags |= EF_WRECK;      // Avoid further crash detection.
      if (e->flags & EF_BOSS) gstate.bosses--;  // Another one bites the dust.
// Add kill credits to players account.
      if (!(cheatlevel&CHEAT_NOMONEY)) player[p].money += e->money;
   }

   if (!cmd) return 0;

// Going to execute the HIT command stream now.
   do {
      switch (*cmd) {
      case C_END:
         again = 0;
         break;

      case C_HIT:
         t_e = search_enemy(e->id, cmd[1]);
         if (t_e != b) step_hit(b, t_e, power, p);
         cmd += 2;      // On to the next command.
         break;

      default:
         error("HIT PATH command not implemented.");
         break;
      }
   } while (again);

   return 0;
}


// Push all enemies.
void enemystep(void)
{
   Enemy *e = gstate.enemy_head;
   Enemy *en;
   int   ret;

   while (e) {
      en = e->next;
      switch (e->mode) {
         case M_PATH:
            ret = step_path(e, E_1);
            if (!ret) ret = step_path(e, E_2);
            break;
         case M_SHOT:
            ret = step_line(e);
            break;
      }
      if (!ret) step_path(e, E_DEATH);
      e = en;
   }
}

/*---------------------------------------------------------
 Function:

 Description:
 Attack Table interpreter.
 This one takes a closer look at the ATTACK TABLE.
 The Attack table says when and where enemy waves
 are going to attack.
---------------------------------------------------------*/
void newwave(void)
{
   Enemy    *e;
   Attack   *a = gstate.attack;

   while (a->count == gstate.frame) {
      switch (a->command) {
      case C_RELEASE:      // release new enemy.
         if (a->level <= gstate.difficulty) {
            e = release_enemy(a->enemy, a->x, a->y, a->layer);
            e->id = a->id;       // Set unique ID number.
// Set initial WAIT to delay ground units.
            e->path[E_1].wait = a->wait;
         }
         break;

      case C_MARK:         // Mark current map position.
         gstate.mark.frame = gstate.frame;
         gstate.mark.attack = gstate.attack;
         gstate.mark.layerpos[0] = getlayer(0);
         gstate.mark.layerpos[1] = getlayer(1);
         break;

      case C_LOOP:         // Loop map. Used for demo game.
         gstate.frame = gstate.mark.frame;
         gstate.attack = gstate.mark.attack;
// Set map position.
         setlayer(0, gstate.mark.layerpos[0]);
         setlayer(1, gstate.mark.layerpos[1]);

// Loop playback if in PLAYBACK mode.
         playback_loop();
         break;

      case C_CYCLEPAL:
         break;

      case C_MAPSPEED:     // Set background map speed.
                           // (layer #, speed_count)
         gstate.layerspeed[a->x] = a->y;
         break;

      case C_SHADOW:       // Don't display even the smallest shadow.
         shadow_level(0);  // No shadow.
         break;

      default:
         error("WAVE command not implemented.");
         break;
      }
      a = ++gstate.attack;
   }
}


/*---------------------------------------------------------
 Function: crash

 Description:
 The "crash" function tests for all the possible crashes.
 It will return 0 if everything is ok or 1 if the HAUPT-
 SCHIFF has been destroyed.
 0 : Nothing.
 1 : SHIP is gone.
 2 : BOSS bites the dust.
---------------------------------------------------------*/
int crash(int p)
{
   Enemy  *e;
   Weapon *w, *wn;
   int    power;

   w = gstate.weapon[p];
// Do for all weapons...
   while (w) {
// No crash if weapon is a SIDEKICK or the HAUPTSCHIFF.
      if ((w->type != WT_SIDEKICK) && (w->type != WT_SHIP)) {
      e = gstate.enemy_head;
// ...and for all enemies.
      while (e) {
// No crash if enemy is already wreck or glass.
         if (!(e->flags&(EF_WRECK|EF_VIRTUAL)) && !e->glass) {
         if (!(e->flags & EF_GROUND) || (w->type == WT_SHOT)) {
            if (crash(e->object, w->object)) {
// Can the nasty enemy stand the immense SCHLAGKRAFT of our WEAPON ?
               power = e->power;
               step_hit(e, e, w->power, p);
// What about the WEAPON. Still alive ?
               if (w->type == WT_SHOT) {
                  if ((w->power -= power) <= 0) {
                     wn = w->next;
                     step_death(w);    // Weapon has been killed.
                     w = wn;
                     goto dirty;
                  }
               }
            }
         }
         }
         e = e->next;
      }
      }
      w = w->next;
dirty: ;
   }

// Check if level has been cleared.
   if (gstate.bosses == 0) return 2;

// Do a better crash test against the MOTHERSHIP now.
   if (cheatlevel & CHEAT_INVUL) return 0;

   if (player[p].glass) return 0;
   w = player[p].main;
   e = gstate.enemy_head;
   while (e) {
      if (!(e->flags & (EF_WRECK|EF_VIRTUAL|EF_GROUND))) {
// Do a CORE CRASH TEST.
         if (crashcore(e->object, w->object)) {
// Save last coordinates. So new ship will start at exact the same position
// as this ship has been lost.
            player[p].x = w->object->x;
            player[p].y = w->object->y;
            player[p].main = 0;
            step_death(w);
            return 1;
         }
      }
      e = e->next;
   }
   return 0;
}


