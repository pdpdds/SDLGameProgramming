#include "sound.h"
#include <stdio.h>

// custom chunk loader for emscripten support
Mix_Chunk* S_LoadChunk( const char* f ) {
	SDL_RWops* rw = SDL_RWFromFile( f, "rb" );
	int sz = SDL_RWsize( rw );
	uint8_t* rwdat = malloc( sz );
	SDL_RWread( rw, rwdat, 1, sz );
	SDL_RWclose( rw );
	rw = SDL_RWFromConstMem( rwdat, sz );
	Mix_Chunk* chunk = Mix_LoadWAV_RW( rw, 1 );
	SDL_RWclose( rw );
	free( rwdat );
	if ( chunk == NULL ) { printf( "Failed to load chunk %s!\n", f ); return NULL; }
	return chunk;
}

