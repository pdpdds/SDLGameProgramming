#include "game.h"
#include "world.h"
#include "draw.h"
#include "sound.h"
#include <string.h>
#include <stdlib.h>
// global game state
game_state_t* gs = NULL;
// global game assets
game_assets_t* g_assets = NULL;

// initialize a new game state
static void G_Init() {
	gs = malloc( sizeof(game_state_t) );
	memset( gs, 0, sizeof(game_state_t) );

	// clean player state
	memset( &gs->ps, 0, sizeof(player_state_t) );
	// 3 lives per game
	gs->ps.lives = 255;
	// clean monster states
	memset( gs->ms, 0, sizeof(gs->ms) );
}

// initialize game assets
static void G_InitAssets( SDL_Renderer* r ) {
	// load resources from DAVE.EXE
	Util_LoadTiles();
	Util_LoadLevels();

	g_assets = malloc( sizeof(game_assets_t) );
	memset( g_assets, 0, sizeof(game_assets_t) );

	// copy all levels loaded from exe by util
	for ( int i = 0; i < NUM_EXE_LEVELS; ++i )
		Util_GetLevel( i, &gs->levels[i] );

	// get loaded tile surface array from util lib
	SDL_Surface** tileSfcs = Util_GetTileSurfaces();

	// convert each surface from util's array to g_asset texture array
	for ( int i = 0; i < NUM_EXE_TILES; ++i ) {
		// mask player, monster, and bullet sprites
		if ( (i >= 53 && i <= 59) || (i >= 67 && i <= 68)
			 || (i >= 71 && i <= 73) || (i >= 77 && i <= 82) || (i >= 89 && i <= 132) )
			SDL_SetColorKey( tileSfcs[i], 1, SDL_MapRGB( tileSfcs[i]->format, 0, 0, 0 ) );

		g_assets->tile_tx[i] = SDL_CreateTextureFromSurface( r, tileSfcs[i] );
	}

	// tile surfaces should be converted as textures inside g_assets now
	Util_FreeTileSurfaces();

	// sfx
	//g_assets->sfx[SFX_JUMP] = S_LoadChunk( "res/jumpd8.wav" ); //Mix_LoadWAV_RW( rw, 1 );
	//g_assets->sfx[SFX_PICKUP] = S_LoadChunk( "res/pickupd8.wav" ); //Mix_LoadWAV_RW( rw, 1 );
}

// poll input
static void G_CheckInput() {
	SDL_Event ev;

	while ( SDL_PollEvent( &ev ) ) {

		// events
		switch ( ev.type ) {

		case SDL_QUIT: gs->quit = 1; break;

		case SDL_KEYDOWN: {
			if ( ev.key.repeat ) break;
			SDL_Keycode key = ev.key.keysym.sym;
			// android back
			if ( key == SDLK_AC_BACK ) { gs->quit = 1; }
			// jump event
			if ( key == SDLK_UP || key == SDLK_z ) { gs->ps.try_jump = 1; }
			// fire
			if ( key == SDLK_LCTRL || key == SDLK_x ) { gs->ps.try_fire = 1; }
			// jetpack
			if ( key == SDLK_LALT || key == SDLK_c ) { gs->ps.try_jetpack = 1; }
			// level select
			if ( key >= SDLK_0 && key <= SDLK_9 ) {
				gs->current_level = key - SDLK_0;
				W_ResetLevel();
			}
		} break;

		default: break;
		} // switch
	}

	// real-time keystate
	const uint8_t* keys = SDL_GetKeyboardState( NULL );

	// attempt dave movement by setting try_ flags
	if ( keys[SDL_SCANCODE_RIGHT] ) gs->ps.try_right = 1;
	if ( keys[SDL_SCANCODE_LEFT] ) gs->ps.try_left = 1;
	if ( keys[SDL_SCANCODE_DOWN] ) gs->ps.try_down = 1;
	if ( keys[SDL_SCANCODE_UP] ) gs->ps.try_up = 1;
}

// clear all try input flags at end of frame
static void G_ClearInput() {
	gs->ps.try_left = 0;
	gs->ps.try_right = 0;
	gs->ps.try_jump = 0;
	gs->ps.try_fire = 0;
	gs->ps.try_jetpack = 0;
	gs->ps.try_up = 0;
	gs->ps.try_down = 0;
}

// main update routine
static void G_Update() {
	// update collision point flags
	P_UpdateCollision();
	// pickups
	P_PickupItem();
	// player bullet
	P_UpdateBullet();
	// monster bullet
	M_UpdateBullet();
	// verify input try flags
	P_VerifyInput();
	// apply player movement
	P_Move();
	// monster movement
	M_Move();
	// monser shooting
	M_Fire();
	// game view scrolling
	W_ScrollView();
	// player gravity
	P_ApplyGravity();
	// update level-wide state
	W_Update();
	// reset input flags
	G_ClearInput();
}

// update tile animation
uint8_t G_UpdateFrame( uint8_t til, uint8_t salt ) {
	uint8_t mod;
	switch ( til ) {
	case 6: case 25: case 129: mod = 4; break;
	case 10: case 36: mod = 5; break;
	default: mod = 1; break;
	}
	return til + (gs->tick/5+salt) % mod;
}

// main drawing routine
static void G_Draw( SDL_Renderer* r ) {
	// clear backbuffer
	SDL_SetRenderDrawColor( r, 0, 40, 80, 0xff );
	SDL_RenderClear( r );

	Draw_World( r );
	Draw_Player( r );
	Draw_Monsters( r );
	Draw_Bullet( r );
	Draw_MonsterBullet( r );

	// flip buffers
	SDL_RenderPresent( r );
}

// main loop function for emscripten support
#ifdef __EMSCRIPTEN__
#include <emscripten.h>

static void emloop( void* p ) {
	SDL_Renderer* r = (SDL_Renderer*)p;
	G_CheckInput();
	G_Update();
	G_Draw( r );
}
#endif

int main( int argc, char** argv ) {
	// initialize SDL
	if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) )
		SDL_Log( "SDL Init error: %s\n", SDL_GetError() );
	// mixer
	//int res = Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 1024 );
	//if ( res < 0 ) fprintf( stderr, "Mix_OpenAudio error: %s\n", Mix_GetError() );
	//Mix_AllocateChannels( 8 );

	// create window and renderer
	int winw = 1280, winh = 720;
	// emscripten canvas size
#ifdef __EMSCRIPTEN__
	winw = 1024; winh = 576;
#endif
	SDL_Window* window = SDL_CreateWindow( "lmdave", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		winw, winh, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL );
	if ( window == NULL ) {
		SDL_Log( "SDL_CreateWindow error: %s\n", SDL_GetError() );
		SDL_Quit(); return EXIT_FAILURE;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED
		| SDL_RENDERER_PRESENTVSYNC );
	if ( renderer == NULL ) {
		SDL_DestroyWindow( window );
		SDL_Log( "SDL_CreateRenderer error: %s\n", SDL_GetError() );
		SDL_Quit(); return EXIT_FAILURE;
	}

	// set internal rendering size
	SDL_RenderSetLogicalSize( renderer, 320, 200 );
	// scaling hint
	//SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "linear" );

	// initialize game state and assets
	G_Init();
	G_InitAssets( renderer );

	// clear initial frame
	SDL_SetRenderDrawColor( renderer, 0, 80, 80, 0xff );
	SDL_RenderClear( renderer );

	// set state for first level
	W_StartLevel();
	// start bg music
	//Mix_PlayMusic( g_assets->mus, -1 );

	// main loop 
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg( emloop, renderer, 1000 / FRAME_DELAY, 1 );
	//emscripten_set_main_loop_timing( EM_TIMING_RAF, 2 ); // vsync/2
#else
	while ( gs->quit == 0 ) {
		// fixed timestep
		uint32_t st = SDL_GetTicks();

		G_CheckInput();
		G_Update();
		G_Draw( renderer );

		uint32_t delay = FRAME_DELAY - (SDL_GetTicks() - st);
		delay = delay > FRAME_DELAY ? 0 : delay;
		SDL_Delay( delay );
	}
#endif
	// destroy each tile texture
	for ( int i = 0; i < NUM_EXE_TILES; ++i )
		SDL_DestroyTexture( g_assets->tile_tx[i] );
	// free audio
	//for ( int i = 0; i < NUM_SFX; ++i )
		//Mix_FreeChunk( g_assets->sfx[i] );
	//Mix_FreeMusic( g_assets->mus );
	free( g_assets );
	free( gs );
	//Mix_CloseAudio();

	// cleanup SDL
	SDL_DestroyRenderer( renderer );
	SDL_DestroyWindow( window );
	SDL_Quit();

	// an explicit exit for android
#if defined( __ANDROID__ ) || defined( ANDROID )
	exit( EXIT_SUCCESS );
#endif

	return EXIT_SUCCESS;
}

