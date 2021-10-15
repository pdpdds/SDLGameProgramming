#ifndef __ROOM_H__
#define __ROOM_H__

#include "rm_not.h"
#include "gamebase.h"
#include "objects.h"
#include <future>

struct Room {
	static int c_room, w, h;
	typedef void (*Rooms)();

	#include "room_def.h"

	static Rooms rm[];

	static void next();
	static void prev();
	static void go(int n);
	static void restart();

	static void menu();
};


#endif