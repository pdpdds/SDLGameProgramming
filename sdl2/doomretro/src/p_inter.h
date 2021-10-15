/*
========================================================================

                           D O O M  R e t r o
         The classic, refined DOOM source port. For Windows PC.

========================================================================

  Copyright © 1993-2012 id Software LLC, a ZeniMax Media company.
  Copyright © 2013-2018 Brad Harding.

  DOOM Retro is a fork of Chocolate DOOM. For a list of credits, see
  <https://github.com/bradharding/doomretro/wiki/CREDITS>.

  This file is part of DOOM Retro.

  DOOM Retro is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation, either version 3 of the License, or (at your
  option) any later version.

  DOOM Retro is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with DOOM Retro. If not, see <https://www.gnu.org/licenses/>.

  DOOM is a registered trademark of id Software LLC, a ZeniMax Media
  company, in the US and/or other countries and is used without
  permission. All other trademarks are the property of their respective
  holders. DOOM Retro is in no way affiliated with nor endorsed by
  id Software.

========================================================================
*/

#if !defined(__P_INTER_H__)
#define __P_INTER_H__

dboolean P_GiveBody(int num, dboolean stat);
void P_GiveMegaHealth(dboolean stat);
dboolean P_GiveArmor(armortype_t armortype, dboolean stat);
dboolean P_GiveAllCards(void);
dboolean P_GivePower(int power);
dboolean P_GiveAllWeapons(void);
dboolean P_GiveBackpack(dboolean giveammo, dboolean stat);
dboolean P_GiveFullAmmo(dboolean stat);
void P_AddBonus(void);
void G_RemoveChoppers(void);
void P_UpdateKillStat(mobjtype_t type, int value);
void P_KillMobj(mobj_t *target, mobj_t *inflicter, mobj_t *source);

extern dboolean oldweaponsowned[NUMWEAPONS];

#endif
