#include "room.h"

int Room::c_room = 0;
int Room::w = 32;
int Room::h = 24;

#include "room_list.h"

void Room::next() {
	go(c_room + 1);
}

void Room::prev() {
	go(c_room - 1);
}

void Room::go(int n) {
	Gamebase::clearObjects();
	Gamebase::clearInput();
	Object::resetDepth();
	c_room = n;
	if (c_room == -1) {
		menu();
	} else {
		rm[c_room]();
	}
	Gamebase::roomStart();
}

void Room::restart() {
	go(c_room);
}

void Room::menu() {
	Gamebase::addObject<Menu>(0, 0);
}
