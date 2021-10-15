#include "game.h"
#include "world.h"
#include <string.h>
//#include "player.h"

extern game_state_t* gs;

// sets new beginning state for current level
void W_StartLevel() {
	P_Spawn();

	// reset monster bullet
	gs->mbullet_px = 0;
	gs->mbullet_py = 0;
	memset( gs->ms, 0, sizeof(gs->ms) );

	// set hardcoded monster type based on level
	switch ( gs->current_level ) {
	case 2: { // level 3, two spiders
		gs->ms[0].type = 89;
		gs->ms[0].px = 44 * TILE_SIZE;
		gs->ms[0].py = 4 * TILE_SIZE;
		gs->ms[1].type = 89;
		gs->ms[1].px = 59 * TILE_SIZE;
		gs->ms[1].py = 4 * TILE_SIZE;
	} break;
	case 3: { // level 4, one monster
		gs->ms[0].type = 93;
		gs->ms[0].px = 32 * TILE_SIZE;
		gs->ms[0].py = 2 * TILE_SIZE;
	} break;
	}

	// reset items
	gs->ps.gun = 0;
	gs->ps.jetpack = 0;
	gs->ps.trophy = 0;
	gs->ps.check_door = 0;
}

// hard reset current level from original data
void W_ResetLevel() {
	Util_GetLevel( gs->current_level, &gs->levels[gs->current_level] );
	gs->ps.lives = 255;
	W_StartLevel();
}

// returns 1 if passed pixel point is not within a solid tile, is_player non zero to execute functionality
uint8_t W_IsClear( int16_t px, int16_t py, uint8_t is_player ) {
	uint8_t tx, ty; // tile pos
	uint8_t til; // tile index

	// off top and bottom
	if ( py < 0 || py >= 10 * TILE_SIZE ) return 1;

	// pixel point to tile pos
	tx = px / TILE_SIZE; ty = py / TILE_SIZE;
	// tile index at level's tx, ty pos
	til = gs->levels[gs->current_level].tiles[ty * 100 + tx];

	// solid tiles
	if ( til == 1 || til == 3 || til == 5 ) return 0;
	if ( til >= 15 && til <= 19 ) return 0;
	if ( til >= 21 && til <= 24 ) return 0;
	if ( til >= 29 && til <= 30 ) return 0;

	// player collision functionality
	if ( is_player && !gs->ps.dead_timer ) {
		// adjusted tile collision
		SDL_Rect colbox;
		colbox.x = (tx - gs->view_x) * TILE_SIZE + 2;
		colbox.y = ty * TILE_SIZE + 2;
		colbox.w = TILE_SIZE-4; colbox.h = TILE_SIZE-4;
		SDL_Point colpt;
		colpt.x = px - gs->view_x * TILE_SIZE;
		colpt.y = py;
		// hazard tiles
		if ( til == 6 || til == 25 || til == 36 ) {
			if ( SDL_PointInRect( &colpt, &colbox ) ) {
				//return 0;
				//P_Spawn();
				gs->ps.dead_timer = 30;
			}
		}

		// pickups
		else if ( til == 10 || til == 4 || til == 20 || (til >= 47 && til <= 52) ) {
			// taller box
			colbox.x -= 0; colbox.y -= 2;
			colbox.w += 0; colbox.h += 4;
			if ( SDL_PointInRect( &colpt, &colbox ) ) {
				gs->ps.check_pickup_x = tx;
				gs->ps.check_pickup_y = ty;
			}
		}

		// door
		else if ( til == 2 ) {
			gs->ps.check_door = 1;
		}
	}

	return 1;
}

uint8_t W_IsVisible( int16_t px ) {
	int tx = px / TILE_SIZE;
	return (tx - gs->view_x < 20 && tx - gs->view_x >= 0);
}

// level-wide state update
void W_Update() {
	// update world tick
	gs->tick++;
	// check if at door and has trophy
	if ( gs->ps.check_door ) {
		if ( gs->ps.trophy ) {
			if ( gs->current_level < 9 ) {
				gs->current_level++;
				W_StartLevel();
			} else {
				// finshed level 10
				gs->quit = 1;
			}
		} else { // no trophy
			gs->ps.check_door = 0;
		}
	}
	// update dead timers
	if ( gs->ps.dead_timer ) {
		gs->ps.dead_timer--;
		if ( !gs->ps.dead_timer ) {
			if ( gs->ps.lives ) {
				gs->ps.lives--;
				P_Spawn();
			} else gs->quit = 1;
		}
	}
	// monsters
	for ( int i = 0; i < sizeof(gs->ms) / sizeof(gs->ms[0]); ++i ) {
		monster_state_t* m = &gs->ms[i];
		if ( m->type ) {
			if ( m->fire_timer ) m->fire_timer--;
			if ( m->dead_timer ) {
				m->dead_timer--;
				if ( !m->dead_timer ) { m->type = 0; }
			} else { // player and monster collision
				if ( m->tx == gs->ps.tx && m->ty == gs->ps.ty ) {
					m->dead_timer = 30;
					gs->ps.dead_timer = 30;
				}
			}
		}
	}
}

// update game view based on set scroll values
void W_ScrollView() {
	// scroll view if dave is about to move off view
	if ( gs->ps.tx - gs->view_x >= 18 )
		gs->scroll_x = 15;
	if ( gs->ps.tx - gs->view_x < 2 )
		gs->scroll_x = -15;

	// do the scroll
	if ( gs->scroll_x > 0 ) {
		if ( gs->view_x == 80 ) gs->scroll_x = 0;
		else { gs->view_x++; gs->scroll_x--; }
	}
	if ( gs->scroll_x < 0 ) {
		if ( gs->view_x == 0 ) gs->scroll_x = 0;
		else { gs->view_x--; gs->scroll_x++; }
	}
}
