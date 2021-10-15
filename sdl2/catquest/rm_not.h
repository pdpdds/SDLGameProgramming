#ifndef __RM_NOT_H__
#define __RM_NOT_H__

#include "const.h"

#define C(T,x,y) Gamebase::addObject< T >(( x )*TILE_SIZE,( y )*TILE_SIZE)
#define R(w,h) resize(( w )*TILE_SIZE,( h )*TILE_SIZE)

#endif
