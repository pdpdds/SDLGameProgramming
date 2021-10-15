#include "game.h"
#include "world.h"

extern game_state_t* gs;

void M_Move() {
	for ( int i = 0; i < sizeof(gs->ms) / sizeof(gs->ms[0]); ++i ) {
		monster_state_t* m = &gs->ms[i];
		if ( m->type && !m->dead_timer ) {
			if ( !m->npx && !m->npy ) {
				m->npx = gs->levels[gs->current_level].path[m->path_index];
				m->npy = gs->levels[gs->current_level].path[m->path_index + 1];
				m->path_index += 2;
			}
			// end of path
			if ( m->npx == (int8_t)0xea && m->npy == (int8_t)0xea ) {
				m->npx = gs->levels[gs->current_level].path[0];
				m->npy = gs->levels[gs->current_level].path[1];
				m->path_index = 2;
			}
			// move
			if ( m->npx < 0 ) { m->px -= 1; m->npx += 1; }
			if ( m->npx > 0 ) { m->px += 1; m->npx -= 1; }
			if ( m->npy < 0 ) { m->py -= 1; m->npy += 1; }
			if ( m->npy > 0 ) { m->py += 1; m->npy -= 1; }
			// update tile pos
			m->tx = m->px / TILE_SIZE;
			m->ty = m->py / TILE_SIZE;
		}
	}
}

void M_Fire() {
	if ( !gs->mbullet_px && !gs->mbullet_py ) {
		for ( int i = 0; i < sizeof(gs->ms) / sizeof(gs->ms[0]); ++i ) {
			monster_state_t* m = &gs->ms[i];
			// assumed size of 24x20
			if ( m->type && !m->dead_timer && W_IsVisible( m->px ) && W_IsClear( m->px+12, m->py+10, 0 ) && !m->fire_timer ) {
				gs->mbullet_dir = gs->ps.px < m->px ? -1 : 1;
				if ( gs->mbullet_dir == 1 )
					gs->mbullet_px = m->px + 18;
				if ( gs->mbullet_dir == -1 )
					gs->mbullet_px = m->px - 10;
				gs->mbullet_py = m->py + 10;
				m->fire_timer = 15;
			}
		}
	}
}

void M_UpdateBullet() {
	if ( !gs->mbullet_px || !gs->mbullet_py ) return;
	if ( !W_IsClear( gs->mbullet_px, gs->mbullet_py, 0 ) || !W_IsVisible( gs->mbullet_px ) )
		gs->mbullet_px = gs->mbullet_py = 0;
	if ( gs->mbullet_px ) {
		gs->mbullet_px += gs->mbullet_dir * 4;
		uint8_t tx = gs->mbullet_px / TILE_SIZE;
		uint8_t ty = gs->mbullet_py / TILE_SIZE;
		if ( tx == gs->ps.tx && ty == gs->ps.ty && !gs->ps.dead_timer ) {
			gs->mbullet_px = gs->mbullet_py = 0;
			gs->ps.dead_timer = 30;
		}
	}
}

