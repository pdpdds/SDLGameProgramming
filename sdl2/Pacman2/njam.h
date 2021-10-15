/*-----------------------------------------------------------------------------
Copyright 2003, 2004 Milan Babuskov

This file is part of Njam (http://njam.sourceforge.net).

Njam is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Njam is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Njam in file COPYING; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
-----------------------------------------------------------------------------*/
#ifndef NJAM_H
#define NJAM_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_mixer.h>
#include "njammap.h"
#include <stdio.h>
//-----------------------------------------------------------------------------
// Maybe this can even be hardcoded, but just in case...
#define MAXDELAY 120
#define GHOSTMAX 8
//-----------------------------------------------------------------------------
// Data used in game: players, sprites, etc...
typedef enum { gtSMART = 0, gtSTUPID, gtCHASER } GhostType;
//-----------------------------------------------------------------------------
struct hiscore_data
{
	char name[10];
	int points;
	int level;
};
//-----------------------------------------------------------------------------
struct indata_struct
{
	int cookies;
	int juices;
	int kills;
	int player_kills;
	int deaths;
	int invisibles;
};
typedef struct NjamPlayer_struct
{
    int MapPoints;
    int GamePoints;		// In a duel game
    bool Playing;
    int Juice;
    int Invisible;
	int Freeze;			// player can get frozen, this is not used currently but it works

    union stats_union
    {
        int data[6];       				// dugmadi, juices, kills, ply-kills, deaths, invisib
        struct indata_struct indata;
    } stats;
} NjamPlayer;
//-----------------------------------------------------------------------------
typedef struct NjamSprite_struct
{
    int x,y;    // coord    1-26, 1-22
    int xo,yo;  // offset   0-4
    int vx,vy;  // speed    -1 0 1

    // animation
    int frame;      	// current frame
    int framemax;  		// total nr. of frames
    int rotate;        	// current rotation (0-3) = (up, down, left or right)
    int delay;			// if ply/ghost is dead

    // for ghost - hunter y/n
    int special;

    // graphics (x offset)
    int SurfaceOffset;
} NjamSprite;
//-----------------------------------------------------------------------------
// the same order as in Main Menu, so it can be easily assigned (cast from int)
typedef enum { gtOnePlayer=0, gtTwoPlayer, gtDuel, gtHostDuel, gtJoinDuel } GameType;
typedef enum { mtMainMenu, mtOptionsMenu } MenuType;
typedef enum { etNetworkError = -5, etGameOver = -4, etGameWon = -3, etLevelWon = -2, etGameDraw = -1, etNone = 0 } EndingType;
//-----------------------------------------------------------------------------
typedef struct
{
	bool PlayMusic;
	bool PlaySound;
	int UsedSkin;
	char ServerIP[16];		// in format 255.255.255.255
} tGameOptions;
//-----------------------------------------------------------------------------
struct list_item
{
	char item_text[50];
	char item_path[512];
	struct list_item *next;
	struct list_item *prev;
};
//-----------------------------------------------------------------------------
class NjamEngine
{
private:
	struct hiscore_data TopTenScores[10];
	bool CheckHiscore();
	int m_LastHiscore;

 	bool m_SDL;						// whether SDL is initialized or not
	bool m_AudioAvailable;			// so we know wheter to try to play music/sfx
	char linux_sdl_driver[10];
	SDL_Surface *m_Screen;

	tGameOptions m_GameOptions;

	int m_NumberOfSkins;			// Graphics resources
	SDL_Surface **m_Skins;
	SDL_Surface *m_Icon;
	SDL_Surface *m_MainMenuImage;
	SDL_Surface *m_OptionsMenuImage;
	SDL_Surface *m_GameOverImage;
	SDL_Surface *m_StatsImage;
	SDL_Surface *m_SpriteImage;
	SDL_Surface *m_HiScoresImage;
	SDL_Surface *m_LevelsetImage;
	SDL_Surface *m_NetSendImage;
	SDL_Surface *m_NetEnterImage;
	SDL_Surface *m_NetLobbyImage;
	NjamFont *m_FontBlue;
	NjamFont *m_FontYellow;
	Mix_Music *m_MainMenuMusic;
	Mix_Music *m_GameMusic1;
	Mix_Music *m_GameMusic2;
	Mix_Chunk *m_Sounds[17];

	int m_ScrollOffset;				// main menu animations' & stuff
	int m_ScriptDelay;
	long m_ScriptFilePos;
	long m_ScriptLastPos;
	void DoScript();
	void ScrollText();
	bool MenuItemSelected(int& SelectedMenuItem);
	int m_NumberOfMenuItems;
	MenuType m_ActiveMenu;
	FILE *script_file;

	// -------------- GAME STUFF ------------------------------------
	double m_fps;					// average frames per second
    int m_Freeze;					// how much longer the ghosts are frozen when player collects the freezer
    int m_Bonus;
    int m_LivesLeft;
    bool m_TripleDinged;
    int m_CurrentMap;				// current level in a non-duel game
	int m_Levels;					// total nr. of levels in a non-duel game
	GameType m_GameType;
    NjamPlayer m_Player[4];
    int m_CookiesLeft;				// on the screen
    NjamMap m_Maps;
    NjamSprite m_Sprite[4+GHOSTMAX];
    int m_Ghosts;					// active (8 in duel, 5 in non-duel)

    void StartGame(GameType);		// game powering functions, implemented in njamgame.cpp
    void SetupGame();
    int PlayMap(bool Allow_powerups=true);
    void SetupMap();
    void RenderGameScreen();
	bool RenderLifeLostScreen(bool LifeAwarded=false);
	void ReadySetGo();
    bool HandleKeyboard();
    void RenderSprites();
    void MovePlayers();
    void MoveGhosts();
    void UpdateWorld();
    EndingType Collide(bool&);
    void DrawGameStats();
    void Animate();

	bool SetupNetwork();			// networking (njamnet.cpp)
	void Disconnect();
	bool NetworkMap();
	bool ExchangeNetwork();
	bool EnterServerIP(bool HasTried);
	void RenderNetworkScreen(bool GameStarting = false);
	bool m_NetworkAvailable;
	TCPsocket m_ServerSocket;
	TCPsocket m_PeerSocket;
	bool m_NetworkEscapedGame;

	void LevelEditor();				// level editor (njamedit.cpp)
	void RenderEditor(bool, int, int, int);
	bool EnterFileName(char *file_name);
	int SelectMap(char type, char *filename = NULL, int filename_size = 0);
	struct list_item *SelectFromList(struct list_item *first);
	void Message(char *text);
	bool Query(char *text);
	bool CheckForSave();
	int m_SwapLevel;
public:
	NjamEngine();
	~NjamEngine();
	bool Init(bool,bool,bool);				// initialize everything except for networking
	void Start();					// start the music and main menu loop
};
//-----------------------------------------------------------------------------
#endif
