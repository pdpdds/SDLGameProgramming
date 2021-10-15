#include "draw.h"

extern game_state_t* gs;
extern game_assets_t* g_assets;

// draw level at current view
void Draw_World( SDL_Renderer* r ) {
	SDL_Rect dst = {0,0,320,200};
	// solid BG fill
	SDL_RenderCopy( r, g_assets->tile_tx[0], NULL, &dst );
	// draw level view 20x10 tiles at 16x16px offset one tile in y
	for ( int j = 0; j < 10; ++j ) {
		dst.y = TILE_SIZE + j * TILE_SIZE;
		dst.w = TILE_SIZE; dst.h = TILE_SIZE;
		for ( int i = 0; i < 20; ++i ) {
			dst.x = i * TILE_SIZE;
			uint8_t til = gs->levels[gs->current_level].tiles[j * 100 + gs->view_x + i];
			if ( til == 0 ) continue;
			til = G_UpdateFrame( til, i );
			SDL_RenderCopy( r, g_assets->tile_tx[til], NULL, &dst );
		}
	}
}

// draw player
void Draw_Player( SDL_Renderer* r ) {
	SDL_Rect dst;
	// relative to view
	dst.x = gs->ps.px - gs->view_x * TILE_SIZE;
	dst.y = TILE_SIZE + gs->ps.py;
	// tile 56 neutral, 53 right, 57 left 20x16px
	uint8_t til = 56;
	dst.w = 20; dst.h = 16;

	// jetpack tile
	if ( gs->ps.do_jetpack )
		til = gs->ps.last_dir >= 0 ? 77 : 80;
	// grounded walk tile
	else if ( gs->ps.on_ground ) {
		til = gs->ps.last_dir >= 0 ? 53 : 57;
		til += (gs->ps.tick/4) % 3;
	}
	// jump tile
	else if ( gs->ps.do_jump || !gs->ps.on_ground )
		til = gs->ps.last_dir >= 0 ? 67 : 68;
	// dead
	if ( gs->ps.dead_timer ) til = 129 + (gs->tick/4) % 4;

	// render
	// grounded debug
	if ( gs->ps.on_ground ) {
		SDL_SetRenderDrawColor( r, 255, 0, 255, 255 );
		SDL_RenderDrawLine( r, dst.x, dst.y+dst.h, dst.x+dst.w, dst.y+dst.h );
	}
	SDL_RenderCopy( r, g_assets->tile_tx[til], NULL, &dst );
	/*dst.w = TILE_SIZE; dst.h = TILE_SIZE;
	dst.x = (gs->ps.tx - gs->view_x) * TILE_SIZE;
	dst.y = gs->ps.ty * TILE_SIZE + TILE_SIZE;
	SDL_RenderDrawRect( r, &dst );*/
}

// draw player bullet
void Draw_Bullet( SDL_Renderer* r ) {
	SDL_Rect dst;
	if ( gs->ps.bullet_px && gs->ps.bullet_py ) {
		// relative to view
		dst.x = gs->ps.bullet_px - gs->view_x * TILE_SIZE;
		dst.y = TILE_SIZE + gs->ps.bullet_py;
		// tile 127 right, 128 left
		uint8_t til = gs->ps.bullet_dir > 0 ? 127 : 128;
		dst.w = 12; dst.h = 3;

		// render
		SDL_RenderCopy( r, g_assets->tile_tx[til], NULL, &dst );
		/*SDL_SetRenderDrawColor( r, 255, 0, 255, 255 );
		dst.w = TILE_SIZE; dst.h = TILE_SIZE;
		dst.x = ((gs->ps.bullet_px / TILE_SIZE) - gs->view_x) * TILE_SIZE;
		dst.y = (gs->ps.bullet_py / TILE_SIZE) * TILE_SIZE + TILE_SIZE;
		SDL_RenderDrawRect( r, &dst );*/
	}
}

// draw monsters
void Draw_Monsters( SDL_Renderer* r ) {
	SDL_Rect dst;
	uint8_t til = 0;

	for ( int i = 0; i < sizeof(gs->ms) / sizeof(gs->ms[0]); ++i ) {
		monster_state_t* m = &gs->ms[i];
		if ( m->type ) {
			dst.x = m->px - gs->view_x * TILE_SIZE;// - 12;
			dst.y = TILE_SIZE + m->py;// - 10;
			dst.w = 24; dst.h = 20;
			til = m->dead_timer ? 129 : m->type;
			til += (gs->tick / 3) % 4;
			SDL_RenderCopy( r, g_assets->tile_tx[til], NULL, &dst );
		}
	}
}
// monster bullets
void Draw_MonsterBullet( SDL_Renderer* r ) {
	SDL_Rect dst;
	if ( gs->mbullet_px && gs->mbullet_py ) {
		// relative to view
		dst.x = gs->mbullet_px - gs->view_x * TILE_SIZE;
		dst.y = TILE_SIZE + gs->mbullet_py;
		// tile 127 right, 128 left
		uint8_t til = gs->mbullet_dir > 0 ? 121 : 124;
		til += (gs->tick / 3) % 3;
		dst.w = 20; dst.h = 3;

		// render
		SDL_RenderCopy( r, g_assets->tile_tx[til], NULL, &dst );
		/*SDL_SetRenderDrawColor( r, 255, 0, 255, 255 );
		dst.w = TILE_SIZE; dst.h = TILE_SIZE;
		dst.x = ((gs->mbullet_px / TILE_SIZE) - gs->view_x) * TILE_SIZE;
		dst.y = (gs->mbullet_py / TILE_SIZE) * TILE_SIZE + TILE_SIZE;
		SDL_RenderDrawRect( r, &dst );*/
	}
}

