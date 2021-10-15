/*-------------------------------------------------------*/
/* object library [c] copyright 1995 by AlphaHelix       */
/*                                                       */
/*                                                       */
/*                                                       */
/*-------------------------------------------------------*/

#include <stdlib.h>

#include "ERROR.HPP"
#include "MEMORY.HPP"
#include "DISKIO.HPP"
#include "SDL_BASE.HPP"
#include "OBJECTS.HPP"
#include "TYPES.H"

// The Map class.
// A map is a multi layer background made out of TILES.
#define MAP_MAXLAYERS         2     // Max number of layers supported.
// Position of data in MapDefinition(MD).
// Assuming array is of type "long".
#define MP_BOTTOM             0
#define MP_BOTTOMSIZE         1
#define MP_TOP                2
#define MP_TOPSIZE            3
#define MP_DATA               4
#pragma pack (4)
struct Map {
   long     *anchor;
   void     *layer[MAP_MAXLAYERS];  // Pointer to all the MAPS.
   int      y[MAP_MAXLAYERS];       // Current viewport position.
   int      dy[MAP_MAXLAYERS];      // Next delta movement.
   int      len[MAP_MAXLAYERS];     // Usable length in pixels.
};


static Map        rMap;
SDLMapLayer				sMap[2];
static Object     *obj[OBJ_MAXPRIORITY];
static int        _shadows;         // Shadows ENABLE level.

// Memory management variables.
// Because of speed reasons we overload the heavily used operators
// "new" and "delete". I don't know if it is worth the effort.
static Memory<Object>     mem;             // Memory system.

// Initialize system.
void initobjects(void)
{
   int   i;

// Initialize memory system.
   mem.init(OBJ_MAXOBJECTS);

// Clear object variables.
   for (i = 0; i < OBJ_MAXPRIORITY; i++) {
      obj[i] = NULL;
   }
   rMap.anchor = 0;
   shadow_level(SH_ALL);
}

// Destroy all objects and leave the system in a clean manner.
void shutobjects(void)
{
// Kill objects.
   killallobjects();
// Shut memory system.
   mem.shut();
}

int object_usage(void)
{
   return mem.usage();
}

void shadow_level(int level)
{
   _shadows = level;
}

/*---------------------------------------------------------
 Function:

 Description:
 Move objects according to MAP movement.
 Don't draw anything now.
---------------------------------------------------------*/
void process_objects(void)
{
   int     i;
   Object  *ptr;

// Move Map.
   for (i = 0; i < MAP_MAXLAYERS; i++)
      rMap.y[i] += rMap.dy[i];

// Move objects on map.
   for (i = OBJ_MAXPRIORITY-1; i >= 0; i--) {
      ptr = obj[i];
      while (ptr != NULL) {
// Move object accoding to map movement.
         if (ptr->layer != -1) ptr->y += rMap.dy[ptr->layer];
         ptr = ptr->next;
      }
   }
}

/*---------------------------------------------------------
 Function:

 Description:
 Update the vscreen.
 First the TILE based background layers are drawn.
 Then shadows and finally the objects.
---------------------------------------------------------*/
void drawscreen(void)
{
   int     i;
   Object  *ptr;

// At the bottom, there's map layer 0
   if (rMap.layer[0] != NULL) {
		render.drawMap(&sMap[0], rMap.y[0]);
   }

// Next we draw layer 1.
   if (rMap.layer[1] != NULL) {
		render.drawMap(&sMap[1], rMap.y[1]);
   }

// Going straight to the objects.
// First draw shadow if needed.
   if (_shadows > SH_NONE) {
      for (i = OBJ_MAXPRIORITY-1; i >= 0; i--) {
         ptr = obj[i];
         while (ptr != NULL) {
            ptr->drawshadow();
            ptr = ptr->next;
         }
      }
   }

// The objects will be drawn now.
   for (i = OBJ_MAXPRIORITY-1; i >= 0; i--) {
      ptr = obj[i];
      while (ptr != NULL) {
         ptr->draw();
         ptr = ptr->next;
      }
   }

}

void postprocess_objects(void)
{
   int     i;
   Object  *ptr, *tmp;

// The objects will be drawn now.
   for (i = OBJ_MAXPRIORITY-1; i >= 0; i--) {
      ptr = obj[i];
      while (ptr != NULL) {
         if (ptr->process()) {
            tmp = ptr;
            ptr = ptr->next;
            delete tmp;       // DESTRUCTION.
         } else {
            ptr = ptr->next;
         }
      }
   }

}

//---------------------------------------------------------
// Object class
//---------------------------------------------------------
Object::Object(Sprite *spr, int x, int y, int flags)
{
   sprite = spr;

   sSprite = sPool.get(spr);
   sfSprite = sfPool.get(spr);

   shadow = NULL;
   sShadow = NULL;

// Initialize new Object.
   priority = flags & O_PRIORITY;
   Object::flags = flags;
// Object is glued to given layer.
   Object::layer = O_NOLAYER;
// Set position on screen.
   Object::x = x; Object::y = y;
// Define animation limits.
   frame = 0;
   f_low = 0; f_high = sprite->frames - 1;
   f_direction = 1;
   f_speedcount = sprite->speed;
   c_now = 0;

// Now link it to the existing Object list.
   if ((next = obj[priority]) != NULL) obj[priority]->prev = this;
   obj[priority] = this;
   prev = NULL;

}

// Destructor.
Object::~Object()
{
    if(sSprite != NULL && !sPool.inPool(sSprite))
    {
        sSprite->destroy();
        delete sSprite;
        sSprite = NULL;
    }

    if(sfSprite != NULL && !sfPool.inPool(sfSprite))
    {
        sfSprite->destroy();
        delete sfSprite;
        sfSprite = NULL;
    }

    if(sShadow != NULL && !sPool.inPool(sShadow) && !shPool.inPool(sShadow))
    {
        sShadow->destroy();
        delete sShadow;
        sShadow = NULL;
    }

   if (prev != NULL) prev->next = next; else obj[priority] = next;
   if (next != NULL) next->prev = prev;
}

void *Object::operator new(size_t s)
{
   return (void *)mem.alloc(s);
}

void Object::operator delete(void *p)
{
   mem.free((Object *)p);
}

void Object::changeshadow(Sprite *spr)
{
	if(_shadows == SH_NONE) return;

   if (!shadow) {
      shadow = spr;

      shadowx = shadowy = 0;
   } else {
      shadow = spr;
   }

    if(sShadow != NULL && !sPool.inPool(sShadow) && !shPool.inPool(sShadow))
    {
       sShadow->destroy();
       delete sShadow;
       sShadow = NULL;
    }

	if(shadow)
	{
		if(sSprite != NULL && shadow == sSprite->getBase()) //Special case of when the shadow is the sprite itself.
		{
			if(!shPool.inPool(shadow))
			{
				sShadow = sPool.getCopy(shadow);
				shPool.add(sShadow, shadow);
			}
			else
			{
				sShadow = shPool.get(shadow);
			}

		}
		else
		{
			sShadow = sPool.get(shadow);
		}

		if(sShadow != NULL)
        {
            sShadow->getTexture()->setAlpha(64);

            SDL_SetTextureColorMod(sShadow->getTexture()->getBase(), 1, 1, 1);
        }
	}

	return;
}

// Change objects sprite. Keep it centered at old sprites position.
void Object::changesprite(Sprite *spr, int dx, int dy)
{
   x += dx;
   y += dy;

   sprite = spr;

   if(sSprite != NULL && !sPool.inPool(sSprite))
   {
       sSprite->destroy();
       delete sSprite;
       sSprite = NULL;
   }

   if(sfSprite != NULL && !sfPool.inPool(sfSprite))
   {
       sfSprite->destroy();
       delete sfSprite;
       sfSprite = NULL;
   }

   sSprite = sPool.get(spr);
   sfSprite = sfPool.get(spr);

   f_low = 0; f_high = sprite->frames - 1;
   if (frame > f_high) frame = f_high; else if (frame < f_low) frame = f_low;
}


void Object::drawshadow(void)
{
   if (sShadow != NULL) render.x_drawsprite(sShadow, x+shadowx, y+shadowy, frame);
}

void Object::draw(void)
{
   if (!(flags & O_FLASH) && sSprite != NULL)
	{
		render.x_drawsprite(sSprite, x, y, frame);
	}
	else
	{
		if(sfSprite != NULL)
		{
			render.x_drawsprite(sfSprite, x, y, frame);
		}

      flags &= ~O_FLASH;
	}

}

int Object::process(void)
{
   if (flags & O_CRISPY) {
      if (--c_speedcount <= 0) {
         c_speedcount = c_speed;
         c_now += c_direction;
         if (c_now > O_MAXCRISPY) c_now = O_MAXCRISPY;
         if (c_now < 0) flags &= ~O_CRISPY;
      }
   }

// Go on to the next frame.
   if (--f_speedcount <= 0) {
      f_speedcount = sprite->speed;
      frame += f_direction;   // Increment to next frame.
// Check for bound violation.
      if (frame > f_high) {
         frame = f_low;
         if (flags & O_ONECYCLE) return 1;
      } else if (frame < f_low) {
         frame = f_high;
         if (flags & O_ONECYCLE) return 1;
      }
   }
   return 0;
}

//---------------------------------------------------------
//
//---------------------------------------------------------

void killallobjects(void)
{
   int     i;

   for (i = 0; i < OBJ_MAXPRIORITY; i++) {
      while (obj[i] != NULL) delete obj[i];
   }
}


//---------------------------------------------------------
// MAP & Layer handling.
//---------------------------------------------------------

void loadmap(char *file)
{
   int    size, lSize;
   ulong  ptr;
   long   *anchor;
   int    i;

// Set some pointers.
   anchor = rMap.anchor = (long *)loadfile(file, NULL);
   ptr = (ulong)anchor; //Get the base pointer address. anchor[MP_BOTTOM/MP_TOP] will refer to an offset from here.

// Layer 0 must always be present.
   rMap.layer[0] = (void *)(anchor[MP_BOTTOM] + ptr);
   lSize = size = anchor[MP_BOTTOMSIZE];
   rMap.len[0] = (size/XTILES)*TILEYS - YMAX;

	//printf("Init map of size: %d, length %d\n", size, map.len[0]);
	sMap[0].init(size, rMap.len[0], _shadows == SH_ALL ? true : false);

// Layer 1 might not be present.
   if (anchor[MP_TOP] != 0) {
      size += anchor[MP_TOPSIZE];
      rMap.layer[1] = (void *)(anchor[MP_TOP] + ptr);
      rMap.len[1] = rMap.len[0]*2;
   }

	sMap[1].init(size - lSize, rMap.len[1], _shadows == SH_ALL ? true : false);

// Adjust tile pointers but let NULL pointers stay NULL pointers.
   for (i = 0; i < size; i++) {
      if (anchor[MP_DATA+i] != 0)
		{
			anchor[MP_DATA+i] += ptr; //ANCHOR[MP_DATA + i] now refers to the actual location of the tile data instead of being an offset from ptr
		}

	}

	//Include null tiles
	for (i = 0; i < lSize; i++) {
			sMap[0].createTile(lSize - 1 - i, (long *)anchor[MP_DATA+i]);
	}

	for (i = i; i < size; i++) {
			sMap[1].createTile((size - lSize) - 1 - (i - lSize), (long *)anchor[MP_DATA+i]);
	}

}

void unloadmap(void)
{
	sMap[0].destroy();
	sMap[1].destroy();

   unloadfile(rMap.anchor);
   rMap.anchor = 0;
}

void setlayer(int layer, int y)
{
   rMap.y[layer] = y;
}

int getlayer(int layer)
{
   return rMap.y[layer];
}

void movelayer(int layer, int dy)
{
   rMap.dy[layer] = dy;
}

int layerlength(int layer)
{
   return rMap.len[layer];
}



