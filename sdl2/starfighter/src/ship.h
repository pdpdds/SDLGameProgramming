/*
Copyright (C) 2015, 2016 Julie Marchant <onpon4@riseup.net>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SHIP_H
#define SHIP_H

#include "defs.h"
#include "structs.h"

int ship_collision(Object *ship, Object *otherShip);
void ship_fireBullet(Object *ship, int weaponType);
void ship_fireRay(Object *ship);

#endif
