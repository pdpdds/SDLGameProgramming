/*
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011 Guus Sliepen
Copyright (C) 2015-2017 Julie Marchant <onpon4@riseup.net>

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

#include "defs.h"
#include "structs.h"

#include "alien.h"
#include "collectable.h"
#include "gfx.h"
#include "radio.h"

Object cargo[MAX_CARGO];

void cargo_init()
{
	for (int i = 0 ; i < MAX_CARGO ; i++)
	{
		cargo[i].active = 0;
		cargo[i].owner = NULL;
	}
}

/*
* I think you all know what this does by now! ;)
*/
static int cargo_get()
{
	for (int i = 0 ; i < MAX_CARGO ; i++)
	{
		if (!cargo[i].active)
			return i;
	}

	return -1;
}

Object *cargo_add(Object *owner, int cargoType)
{
	int index = cargo_get();

	if (index == -1)
		return NULL;

	cargo[index].active = 1;
	cargo[index].owner = owner;
	cargo[index].x = owner->x;
	cargo[index].y = owner->y;
	cargo[index].dx = 0;
	cargo[index].dy = 0;
	cargo[index].collectType = cargoType;
	cargo[index].image[0] = gfx_sprites[SP_CARGO];
	if (cargoType == P_PHOEBE)
		cargo[index].image[0] = gfx_shipSprites[SS_FRIEND];

	return &cargo[index];
}

void cargo_becomeCollectable(int i)
{
	if (cargo[i].collectType != P_PHOEBE)
	{
		collectable_add(cargo[i].x, cargo[i].y, cargo[i].collectType, 1, 600);
	}
	else
	{
		aliens[ALIEN_PHOEBE].active = 1;
		aliens[ALIEN_PHOEBE].x = cargo[i].x;
		aliens[ALIEN_PHOEBE].y = cargo[i].y;
		radio_setMessage(FS_PHOEBE, "Thanks! Watch out, WEAPCO! Phoebe's loose and she's ANGRY!", 1);
	}

	cargo[i].active = 0;
}
