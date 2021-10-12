/*   
 * Copyright (C) 2008 - 2011 The OpenTitus team
 *
 * Authors:
 * Eirik Stople
 *
 * "Titus the Fox: To Marrakech and Back" (1992) and
 * "Lagaf': Les Aventures de Moktar - Vol 1: La Zoubida" (1991)
 * was developed by, and is probably copyrighted by Titus Software,
 * which, according to Wikipedia, stopped buisness in 2005.
 *
 * OpenTitus is not affiliated with Titus Software.
 *
 * OpenTitus is  free software; you can redistribute  it and/or modify
 * it under the  terms of the GNU General  Public License as published
 * by the Free  Software Foundation; either version 3  of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the GNU
 * General Public License for more details.
 */

/* player.h
 * Handles player movement and keyboard handling
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include "level.h"

int move_player(TITUS_level *level);
TITUS_object *FORCE_POSE(TITUS_level *level);


void DEC_ENERGY(TITUS_level *level);
CASE_DEAD_IM(TITUS_level *level);
t_pause(TITUS_level *level);
BRK_COLLISION(TITUS_level *level);
DEC_LIFE(TITUS_level *level);
DECELERATION(TITUS_player *player);
INC_ENERGY(TITUS_level *level);


#endif

