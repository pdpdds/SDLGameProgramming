#include "game.h"
#include "world.h"

// sound channels
#define S_CHAN_JUMP 0
#define S_CHAN_PICKUP 1

extern game_state_t* gs;
extern game_assets_t* g_assets;

// sets player position to current level's player start
void P_Spawn() {
	// reset view
	gs->view_x = 0;
	gs->view_y = 0;
	gs->scroll_x = 0;

	// reset player jump
	gs->ps.jump_timer = 0;
	gs->ps.on_ground = 0;
	gs->ps.do_jump = 0;
	// reset direction and bullet
	gs->ps.last_dir = 0;
	gs->ps.bullet_px = 0;
	gs->ps.bullet_py = 0;
	gs->ps.do_fire = 0;
	gs->ps.do_jetpack = 0;
	gs->ps.do_up = 0; gs->ps.do_down = 0;
	gs->ps.dead_timer = 0;

	// hardcoded player starts
	switch ( gs->current_level ) {
	case 0: case 4: case 5: case 7: case 9: gs->ps.tx = 2; gs->ps.ty = 8; break;
	case 1: gs->ps.tx = 1; gs->ps.ty = 8; break;
	case 2: gs->ps.tx = 2; gs->ps.ty = 5; break;
	case 3: gs->ps.tx = 1; gs->ps.ty = 5; break;
	case 6: gs->ps.tx = 1; gs->ps.ty = 2; break;
	case 8: gs->ps.tx = 6; gs->ps.ty = 1; break;
	default: break;
	}

	gs->ps.px = gs->ps.tx * TILE_SIZE;
	gs->ps.py = gs->ps.ty * TILE_SIZE;
}

// pickup functionality and remove from world
void P_PickupItem() {
	uint8_t tx = gs->ps.check_pickup_x;
	uint8_t ty = gs->ps.check_pickup_y;

	if ( !tx && !ty ) return;

	uint8_t til = gs->levels[gs->current_level].tiles[ty * 100 + tx];

	// pickup functionality here
	if ( til == 4 ) gs->ps.jetpack = 0xff;
	if ( til == 10 ) {
		gs->ps.score += 1000;
		gs->ps.trophy = 1;
	}
	if ( til == 20 ) gs->ps.gun = 1;

	// remove
	gs->levels[gs->current_level].tiles[ty * 100 + tx] = 0;
	gs->ps.check_pickup_x = 0;
	gs->ps.check_pickup_y = 0;
	// sfx
	//Mix_PlayChannel( S_CHAN_PICKUP, g_assets->sfx[SFX_PICKUP], 0 );
}

// update collision point clear flags
void P_UpdateCollision() {
	if ( gs->ps.dead_timer ) return;
	// 8 points of collision; relative to top left of tile 56 neutral frame (20x16)
	// 0, 1 = top left, top right
	gs->ps.col_point[0] = W_IsClear( gs->ps.px + 4, gs->ps.py - 0, 1 );
	gs->ps.col_point[1] = W_IsClear( gs->ps.px + 10, gs->ps.py - 0, 1 );
	// 2, 3 = right edge
	gs->ps.col_point[2] = W_IsClear( gs->ps.px + 12, gs->ps.py + 2, 1 );
	gs->ps.col_point[3] = W_IsClear( gs->ps.px + 12, gs->ps.py + 14, 1 );
	// 4, 5 = bottom edge
	gs->ps.col_point[4] = W_IsClear( gs->ps.px + 10, gs->ps.py + 16, 1 );
	gs->ps.col_point[5] = W_IsClear( gs->ps.px + 4, gs->ps.py + 16, 1 );
	// 6, 7 = left edge
	gs->ps.col_point[6] = W_IsClear( gs->ps.px + 2, gs->ps.py + 14, 1 );
	gs->ps.col_point[7] = W_IsClear( gs->ps.px + 2, gs->ps.py + 2, 1 );
	// update on_ground flag if a bottom point (4,5) is not clear
	gs->ps.on_ground = (!gs->ps.col_point[4] || !gs->ps.col_point[5]);
}

// update bullet state
void P_UpdateBullet() {
	// skip if no bullet in world
	if ( !gs->ps.bullet_px || !gs->ps.bullet_py ) return;

	// collision
	if ( !W_IsClear( gs->ps.bullet_px, gs->ps.bullet_py, 0 ) )
		gs->ps.bullet_px = gs->ps.bullet_py = 0;

	// off-screen
	uint8_t tx = gs->ps.bullet_px / TILE_SIZE;
	uint8_t ty = gs->ps.bullet_py / TILE_SIZE;
	if ( tx - gs->view_x < 0 || tx - gs->view_x > 20 )
		gs->ps.bullet_px = gs->ps.bullet_py = 0;

	if ( gs->ps.bullet_px ) {
		gs->ps.bullet_px += gs->ps.bullet_dir * 4;

		for ( int i = 0; i < sizeof(gs->ms) / sizeof(gs->ms[0]); ++i ) {
			monster_state_t* m = &gs->ms[i];
			if ( m->type && !m->dead_timer ) {
				if ( (ty == m->ty || ty == m->ty + 1) && (tx == m->tx || tx == m->tx + 1) ) {
					gs->ps.bullet_px = gs->ps.bullet_py = 0;
					m->dead_timer = 30;
				}
			}
		}
	}
}

// validate input whose try flags were set
void P_VerifyInput() {
	// no input when dead
	if ( gs->ps.dead_timer ) return;
	// right; col points 2, 3
	if ( gs->ps.try_right && gs->ps.col_point[2] && gs->ps.col_point[3] ) {
		gs->ps.do_right = 1;
	}
	// left; col points 6, 7
	if ( gs->ps.try_left && gs->ps.col_point[6] && gs->ps.col_point[7] ) {
		gs->ps.do_left = 1;
	}
	// jump; on_ground and col points 0, 1
	if ( gs->ps.try_jump && gs->ps.on_ground && !gs->ps.do_jump && !gs->ps.do_jetpack
		 && (gs->ps.col_point[0] && gs->ps.col_point[1]) ) {
		gs->ps.do_jump = 1;
	}
	// reset jump timer if contact a ground while still "jumping"
	if ( gs->ps.try_jump && gs->ps.on_ground && gs->ps.jump_timer )
		gs->ps.jump_timer = 0;
	// fire if have gun and no bullet in world
	if ( gs->ps.try_fire && gs->ps.gun && !gs->ps.bullet_px && !gs->ps.bullet_py ) {
		gs->ps.do_fire = 1;
	}
	// jetpack toggle
	if ( gs->ps.try_jetpack && gs->ps.jetpack ) {
		gs->ps.do_jetpack = !gs->ps.do_jetpack;
		// stop jump
		if ( gs->ps.do_jetpack ) {
			gs->ps.do_jump = 0; gs->ps.jump_timer = 0;
		}
	}
	// down if bottom is clear and jetpack
	if ( gs->ps.try_down && gs->ps.do_jetpack && gs->ps.col_point[4] && gs->ps.col_point[5] ) {
		gs->ps.do_down = 1;
	}
	// up if top is clear and jetpack
	if ( gs->ps.try_up && gs->ps.do_jetpack && gs->ps.col_point[0] && gs->ps.col_point[1] ) {
		gs->ps.do_up = 1;
	}
}

// apply validated player movement
void P_Move() {
	if ( gs->ps.dead_timer ) return;
	// update player's tile pos
	// sample x towards the center
	gs->ps.tx = (gs->ps.px + TILE_SIZE / 2) / TILE_SIZE;
	gs->ps.ty = gs->ps.py / TILE_SIZE;
	// wrap to top if off level bottom
	if ( gs->ps.py >= 10 * TILE_SIZE ) { gs->ps.ty = 0; gs->ps.py = -TILE_SIZE * 2; }

	if ( gs->ps.do_right ) {
		gs->ps.px += 2;
		gs->ps.last_dir = 1;
		gs->ps.do_right = 0;
		gs->ps.tick++;
	}
	if ( gs->ps.do_left ) {
		gs->ps.px -= 2;
		gs->ps.last_dir = -1;
		gs->ps.do_left = 0;
		gs->ps.tick++;
	}
	// up and down
	if ( gs->ps.do_up ) {
		gs->ps.py -= 2;
		gs->ps.do_up = 0;
		gs->ps.tick++;
	} else if ( gs->ps.do_down ) {
		gs->ps.py += 2;
		gs->ps.do_down = 0;
		gs->ps.tick++;
	// jump
	} else if ( gs->ps.do_jump ) {
		if ( !gs->ps.jump_timer ) {
			gs->ps.jump_timer = 25;
			//gs->ps.last_dir = 0;
			//Mix_PlayChannel( S_CHAN_JUMP, g_assets->sfx[SFX_JUMP], 0 );
		}

		if ( gs->ps.col_point[0] && gs->ps.col_point[1] ) {
			if ( gs->ps.jump_timer > 12 )
				gs->ps.py -= 2;
			if ( gs->ps.jump_timer >= 7 && gs->ps.jump_timer <= 12 )
				gs->ps.py -= 1;
			gs->ps.jump_timer--;
		} else gs->ps.jump_timer = 0;

		//gs->ps.jump_timer--;

		if ( !gs->ps.jump_timer )
			gs->ps.do_jump = 0;
	}
	// fire
	if ( gs->ps.do_fire ) {
		gs->ps.bullet_dir = gs->ps.last_dir;
		// default right
		if ( !gs->ps.bullet_dir ) gs->ps.bullet_dir = 1;
		// start bullet in "front" of player
		if ( gs->ps.bullet_dir == 1 )
			gs->ps.bullet_px = gs->ps.px + 18;
		if ( gs->ps.bullet_dir == -1 )
			gs->ps.bullet_px = gs->ps.px - 8;
		// halfway down player
		gs->ps.bullet_py = gs->ps.py + 8;
		gs->ps.do_fire = 0;
	}
}

// apply gravity to player
void P_ApplyGravity() {
	if ( gs->ps.dead_timer ) return;
	if ( !gs->ps.do_jump && !gs->ps.on_ground && !gs->ps.do_jetpack ) {
		// check below sprite
		if ( W_IsClear( gs->ps.px + 4, gs->ps.py + 17, 0 ) && W_IsClear( gs->ps.px + 10, gs->ps.py + 17, 0 ) )
			gs->ps.py += 2;
		else { // align to tile
			uint8_t not_align = gs->ps.py % TILE_SIZE;
			if ( not_align ) {
				gs->ps.py = not_align < (TILE_SIZE / 2) ?
					gs->ps.py - not_align : gs->ps.py + TILE_SIZE - not_align;
			}
		}
	}
}

