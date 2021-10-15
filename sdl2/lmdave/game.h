#ifndef _LMDAVE_H
#define _LMDAVE_H

#include <SDL2/SDL.h>

// for dealing with resources in EXE
#include "util/util.h"

#include "sound.h"
#include "player.h"
#include "monster.h"

// global game state
typedef struct {
	uint8_t quit;
	uint16_t tick;
	uint8_t current_level;
	// view and scroll are per tile
	uint8_t view_x, view_y;
	int8_t scroll_x;

	// player state
	player_state_t ps;
	// max 5 monster states
	monster_state_t ms[5];

	// a single monster bullet
	uint16_t mbullet_px, mbullet_py;
	int8_t mbullet_dir;

	level_t levels[NUM_EXE_LEVELS]; // copied from exe util's GetLevel
} game_state_t;

// game assets
typedef struct {
	// tiles as textures converted from util's tile surfaces
	SDL_Texture* tile_tx[NUM_EXE_TILES];
	// sfx
	Mix_Chunk* sfx[NUM_SFX];
	// music
	//Mix_Music* mus;
} game_assets_t;

// level tile size in pixels
#define TILE_SIZE 16
// fixed frame delay
#define FRAME_DELAY 33

// common functions
// update a tile index based on game tick for animation
uint8_t G_UpdateFrame( uint8_t til, uint8_t salt );

#endif

