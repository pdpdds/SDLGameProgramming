// extract level data from uncompressed dave.exe
// named as levelxx.dat
#include <stdio.h>
#include <string.h>
#include "util.h"

// all levels in the exe
static level_t levels[NUM_EXE_LEVELS];

// copy a single level structure for external use
void Util_GetLevel( unsigned n, level_t* lvl ) {
	if ( n > NUM_EXE_LEVELS - 1 ) n = 0;
	memcpy( lvl, &levels[n], sizeof(level_t) );
}

// export all levels to seperate .dat file
void Util_SaveLevels() {
	for ( int l = 0; l < NUM_EXE_LEVELS; ++l ) {
		char fname[1024];
		snprintf( fname, 1024, "./levels/level%02u.dat", l );
		printf( "Saving level %u to '%s'\n", l, fname );

		SDL_RWops* lvlfile = SDL_RWFromFile( fname, "wb" );

		// write path data
		for ( int p = 0; p < 256; ++p ) {
			SDL_RWwrite( lvlfile, &levels[l].path[p], 1, 1 );
		}
		// write tile data
		for ( int t = 0; t < 1000; ++t ) {
			SDL_RWwrite( lvlfile, &levels[l].tiles[t], 1, 1 );
		}
		// padding
		SDL_RWwrite( lvlfile, levels[l].pad, 24, 1 );

		SDL_RWclose( lvlfile );
	}
}

// fill global level array with dat from exe
void Util_LoadLevels() {
	const uint32_t lvl_dat_addr = 0x26e0a;

	SDL_RWops* ddexe = SDL_RWFromFile( "res/DAVE.EXE", "rb" );
	if ( ddexe == NULL ) { fprintf( stderr, "Error opening DAVE.EXE for levels.\n" ); return; }

	// 10 levels @ 1280 bytes each
	// 16x16 tiles, 320x160px (20x10 tiles) view
	// off edges will wrap

	// seek to start of data
	SDL_RWseek( ddexe, lvl_dat_addr, RW_SEEK_SET );

	memset( levels, 0, sizeof(levels) );

	// read each level into array
	for ( int l = 0; l < NUM_EXE_LEVELS; ++l ) {
		// read path data
		for ( int p = 0; p < 256; ++p ) {
			SDL_RWread( ddexe, &levels[l].path[p], 1, 1 );
		}
		// read tile data
		for ( int t = 0; t < 1000; ++t ) {
			SDL_RWread( ddexe, &levels[l].tiles[t], 1, 1 );
		}
		// padding
		SDL_RWread( ddexe, levels[l].pad, 24, 1 );
	}

	// close exe
	SDL_RWclose( ddexe );
}

// create a large world map image with all levels
void Util_CreateWorldMap() {
	// create big empty surface for containing entire world map
	SDL_Surface* map = SDL_CreateRGBSurface( 0, 1600, 1600, 32, 0, 0, 0, 0 );
	// level, row, column
	for ( int l = 0; l < NUM_EXE_LEVELS; ++l ) {
		for ( int y = 0; y < 10; ++y ) {
			for ( int x = 0; x < 100; ++x ) {
				uint8_t til = levels[l].tiles[y * 100 + x];
				SDL_Rect dst;
				dst.x = x * 16;
				dst.y = l * 160 + y * 16;
				dst.w = 16; dst.h = 16;
				SDL_Surface** tile_sfc = Util_GetTileSurfaces();
				SDL_BlitSurface( tile_sfc[til], NULL, map, &dst );

				// hardcoded player and monster starts
				if ( (l == 0) && ((x == 2 && y == 8) || (x == 20 && y == 0)) ) { SDL_BlitSurface( tile_sfc[53], NULL, map, &dst ); }
				if ( (l == 1) && ((x == 1 && y == 8) || (x == 51 && y == 0)) ) { SDL_BlitSurface( tile_sfc[53], NULL, map, &dst ); }
				if ( l == 2 && x == 2 && y == 5 ) { SDL_BlitSurface( tile_sfc[53], NULL, map, &dst ); }
				// l2 monsters
				if ( l == 2 )
					if ( (x == 44 || x == 59) && y == 4 ) {
						SDL_BlitSurface( tile_sfc[89], NULL, map, &dst );
					}
				if ( l == 3 && x == 1 && y == 5 ) { SDL_BlitSurface( tile_sfc[53], NULL, map, &dst ); }
				if ( l == 4 && x == 2 && y == 8 ) { SDL_BlitSurface( tile_sfc[53], NULL, map, &dst ); }
				if ( (l == 5) && ((x == 2 && y == 8) || (x == 71 && y == 0)) ) { SDL_BlitSurface( tile_sfc[53], NULL, map, &dst ); }
				if ( (l == 6) && ((x == 1 && y == 2) || (x == 80 && y == 0)) ) { SDL_BlitSurface( tile_sfc[53], NULL, map, &dst ); }
				if ( l == 7 && x == 2 && y == 8 ) { SDL_BlitSurface( tile_sfc[53], NULL, map, &dst ); }
				if ( l == 8 && x == 6 && y == 1 ) { SDL_BlitSurface( tile_sfc[53], NULL, map, &dst ); }
				if ( l == 9 && x == 2 && y == 8 ) { SDL_BlitSurface( tile_sfc[53], NULL, map, &dst ); }
			}
		}
	}
	// save map file
	SDL_SaveBMP( map, "./map.bmp" );

	// free world map
	SDL_FreeSurface( map );
}

