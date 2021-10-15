
#ifndef AH_OBJECTS
#define AH_OBJECTS

#include "SDL_BASE.HPP"
#include "XLIB.HPP"

// Shadow Flags
#define SH_NONE         0     // No shadows are displayed.
#define SH_SOME         1     // Only object shadows. No Layer shadow.
#define SH_ALL          2     // All shadows are displayed.

// Object FLAGS
#define OBJ_MAXOBJECTS  200   // Max number of objects allowed.
#define OBJ_MAXPRIORITY 10    // Supported priority levels.

#define O_PRIORITY      0x00ff
#define O_FLASH         0x0100      // If this is set, object will flash.
#define O_ONECYCLE      0x0200      // Just cycle once and destroy object afterwards.
#define O_SHADOW        0x0400      // Draw objects shadow.
#define O_CRISPY        0x0800      // Do the crispy cha-cha-cha.
#define O_POOL          0x2000      // Object SDL sprite is in pool.
//---
#define O_NOLAYER       -1          // Identifies Airborne object.
#define O_MAXCRISPY     (CRISPYSTEPS-1)

#pragma pack (4)
class Object {
public:  // Yeah. It's strange. Everything is public here ?? Poor C++ programmer.
   Sprite   *sprite;       // Main sprite.
   SDLSprite *sSprite;
   SDLSprite *sfSprite;
   Sprite   *shadow;       // Object's shadow.
   SDLSprite *sShadow;
   int      priority;      // Object's priority level.
   int      layer;         // Layer to which object is attached.
   int      frame;         // Current frame.
   int      f_low, f_high; // Upper and lower frame limits.
   int      f_direction;   // Direction of animation. (+1 or -1).
   int      f_speedcount;  // Speed counter.
   int      c_now;         // Crispy NOW value.
   int      c_speed;       // Crispy speed.
   int      c_speedcount;  // Crispy speed countdown.
   int      c_direction;   // Crispy direction.
   int      flags;         // Mode of operation.

   int      shadowx;       // Pos of SHADOW relative to object's position.
   int      shadowy;       //
   int      x, y;          // (x, y) position of object on screen.
   Object   *next, *prev;  // Link to next\prev Object in queue.

public:  // functions.
   Object(Sprite *spr, int x, int y, int flags);
   ~Object();
   void     *operator new(size_t size);
   void     operator delete(void *p);
   void     changesprite(Sprite *spr, int dx, int dy);
   void     chgspr_center(Sprite *spr)
   {
      changesprite(spr, (sprite->xs-spr->xs)/2, (sprite->ys-spr->ys)/2);
   }
   void     moveshadow(int dx, int dy)
   {
      shadowx += dx; shadowy += dy;
   }
   void     changeshadow(Sprite *spr);
   void     changeshadow(Sprite *spr, int dx, int dy)
   {
      changeshadow(spr);
      moveshadow(dx, dy);
   }
   void     setlayer(int layer) { Object::layer = layer; }
   void     flash(void) {
		flags |= O_FLASH;
	}
   void     animation(int dir) { f_direction = dir; }
   void     setbounds(int low, int high)
   {
      f_low = low; f_high = high;
      if (frame < low || frame > high) frame = low;
   }
   void     crispy_set(int l)
   {
      c_now = l;
      c_direction = 0;
      flags |= O_CRISPY;
   }
   void     crispy_do(int speed)
   {
      c_speedcount = c_speed = speed;
      if (c_now <= 0) {
         c_now = 0;
         c_direction = 1;
      } else {
         c_now = O_MAXCRISPY;
         c_direction = -1;
      }
      flags |= O_CRISPY;
   }
   void     draw(void);
   void     drawshadow(void);
   int      process(void);
// Some friends are needed here.
   friend   int crash(Object *obj1, Object *obj2);
   friend   int crashcore(Object *obj1, Object *obj2);
};

// Crash analysis. This is a cool one and it should be inlined.
// Crash just checks whether the two OBJECTS do overlap and returns 1 if so.
inline int crash(Object *obj1, Object *obj2)
{
   Sprite *s1 = obj1->sprite;
   Sprite *s2 = obj2->sprite;

// The compare below is done in the hope the OPTIMIZER will
// do a pretty good job.
   return ((obj1->x>=obj2->x-s1->xs) && (obj1->x<=obj2->x+s2->xs) &&
           (obj1->y>=obj2->y-s1->ys) && (obj1->y<=obj2->y+s2->ys));

}

// crashcore is bit more complex.
// It tests the core of the two objects for overlapping.
inline int crashcore(Object *obj1, Object *obj2)
{
   Sprite *s1 = obj1->sprite;
   Sprite *s2 = obj2->sprite;

   return ((obj1->x+s1->x_core >= obj2->x+s2->x_core-s1->xs_core) &&
           (obj1->x+s1->x_core <= obj2->x+s2->x_core+s2->xs_core) &&
           (obj1->y+s1->y_core >= obj2->y+s2->y_core-s1->ys_core) &&
           (obj1->y+s1->y_core <= obj2->y+s2->y_core+s2->ys_core));

}


// Map handling.
void loadmap(char *f);
void unloadmap(void);
void setlayer(int layer, int y);
int getlayer(int layer);
void movelayer(int layer, int dy);
int layerlength(int layer);

// Object handling.
int crash(Object *obj1, Object *obj2);
int crashcore(Object *obj1, Object *obj2);
void killallobjects(void);

// General lall.
void initobjects(void);
void shutobjects(void);
int object_usage(void);
void process_objects(void);
void drawscreen(void);
void postprocess_objects(void);
void shadow_level(int);

#endif


