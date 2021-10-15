//-----------------------------------------------------------------------------
//	Filename:	njamgame.cpp
//	Created:	31. May 2003. by Milan Babuskov
//
//  Purpose:	Manage game logic & run the game
//				Handle keyboard
//				Render game objects
//
//  This file implements methods of NjamEngine class
//  which are defined in njam.h
//
//  Tabs should be at 4 spaces.
//  Each section is separated with line: //-------...
/*-----------------------------------------------------------------------------
Copyright 2003 Milan Babuskov

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
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>
#include "njamutils.h"
#include "njamfont.h"
#include "njam.h"
#include "njamsingleton.h"
//-----------------------------------------------------------------------------
// game loop, all games are started from here
void NjamEngine::StartGame(GameType Gt)
{
	// "forget" the last hiscore if any
	m_LastHiscore = -1;

	// init random nr. generator
	srand( SDL_GetTicks() );

	for (int i=0; i<4; i++)
		m_Player[i].Playing = false;

	m_Ghosts = GHOSTMAX;
 	m_GameType = Gt;
	m_Levels = 0;
	switch (m_GameType)
	{
		case gtTwoPlayer:
			m_Player[1].Playing = true;
		case gtOnePlayer:
			m_Player[0].Playing = true;
			m_Ghosts = 5;
			m_Levels = SelectMap('P');
			break;
		case gtDuel:
			m_Player[0].Playing = true;
			m_Player[1].Playing = true;
		case gtHostDuel:
			m_Levels = SelectMap('L');
			break;
		case gtJoinDuel:								// don't load, the server will send map data
			m_Levels = 1;
			break;
		default:
			return;	// should never get here
	};

	if (m_GameType == gtHostDuel || m_GameType == gtJoinDuel)
	{
		if (!SetupNetwork())	// game canceled or network error
			return;
	}

	if (m_Levels < 1)	// no playable maps found
	{
		printf("Map loader error: njamgame.cpp: StartGame():  no playable maps found.\n");
		return;
	}

	// stop the music
	if (m_AudioAvailable && m_MainMenuMusic && Mix_PlayingMusic())
		Mix_HaltMusic();

    SetupGame();    	// zero player points, etc...
    int Winner;
    m_CurrentMap = -1;	// it will increase if it's not a duel game

    do
    {
    	if (m_GameType == gtOnePlayer || m_GameType == gtTwoPlayer)		// play maps in order
    		m_CurrentMap++;
    	else                                                            // select random map for duel
	        m_CurrentMap = NjamRandom(m_Levels);

		if (m_GameType != gtJoinDuel)
			m_Maps.SetCurrentMap(m_CurrentMap);

		// music 1 should play more often
		Mix_Music *music = (NjamRandom(4) > 0 ? m_GameMusic1 : m_GameMusic2);	// play in-game music
		if (m_AudioAvailable && music && m_GameOptions.PlayMusic && Mix_PlayMusic(music, -1)==-1)
	        printf("ERROR: njamgame.cpp: Mix_PlayMusic: %s\n", Mix_GetError());

        LogFile("Starting map...\n");
		Winner = PlayMap();		// 0-3: ply, -1 draw game, -2 non_duel over, -3 non_duel won, -4 non_duel lost.

		if (m_AudioAvailable && Mix_PlayingMusic())	// stop music
			Mix_HaltMusic();

		if (Winner == etNetworkError)
			return;

        if (Winner == etLevelWon && m_CurrentMap == m_Levels - 1)	// all levels passed
        	Winner = etGameWon;

		if (Winner > -1 && (m_GameType != gtOnePlayer && m_GameType != gtTwoPlayer))
            m_Player[Winner].GamePoints++;	// we have a winner in a duel game

		if (m_GameOptions.PlaySound)
		{
			int sfx;
			if (Winner == etGameWon || Winner == etLevelWon)       // play applause
				sfx = 0;
			else if (Winner == etGameOver)	// play no-applause (ahm, ahm)
				sfx = 1;
			else					// play anything
				sfx = NjamRandom(2);

			Mix_PlayChannel(-1, m_Sounds[7+sfx], 0);
		}

        RenderGameScreen();
        DrawGameStats();
       

		Render(m_Screen);

		Uint32 t = SDL_GetTicks();

        while (NjamGetch(false) != SDLK_UNKNOWN);	// empty event buffer
        SDL_Keycode key;					 			// space to continue, enter to exit to main menu
        do
        {
            key = NjamGetch(true);				// true = wait for keypress
            if (key == SDLK_ESCAPE)
            {									// wait 2sec in multiplayer to avoid both sides pressing ESC
            	if ((m_GameType == gtHostDuel || m_GameType == gtJoinDuel) && SDL_GetTicks() < t + 2000)
            		continue;
            	else
					return;
			}
        }
        while (key != SDLK_SPACE);

        if (Winner == etGameDraw || Winner == etLevelWon)
        {
        	Winner = 0;		// he have to set Winner here, so that the while loop doesn't break
        	continue;		// since it would be checking invalid memory ( negative array index )
        }

        if (Winner == etGameWon || Winner == etGameOver)		// non_duel game over (won or lost)
        	break;
    }																		// set winner to negative number in
    while (m_GameType == gtOnePlayer || m_Player[Winner].GamePoints < 4);	// first to get to 4 victories is a winner

	if (Winner == etGameOver)
	{
        SDL_FillRect(m_Screen, NULL, 0);
		SDL_Rect dest;
		NjamSetRect(dest, 175, 130);
		SDL_BlitSurface(m_GameOverImage, NULL, m_Screen, &dest);
		Render(m_Screen);

		if (m_AudioAvailable && m_GameOptions.PlaySound)
			Mix_PlayChannel(-1, m_Sounds[0], 0);

		SDL_Delay(700);
		while (NjamGetch(false) != SDLK_UNKNOWN);	// empty keyboard buffer
		if (!CheckHiscore())					// if !hiscore
			SDL_Delay(1300);					// wait a little longer
	}
	else if (Winner == etGameWon)		// non_duel won
    {
        SDL_FillRect(m_Screen, NULL, 0);
		SDL_Rect dest;
		NjamSetRect(dest, 80, 60);

		// load .bmp for network-send screen
		LogFile("Loading game-won image...");
		SDL_Surface *temp = IMG_Load("data/winall.jpg");
		if (!temp)
		{
			LogFile("FAILED.\n");
			LogFile((const char *)SDL_GetError());
		}
		else
		{
			LogFile("OK.\n");
			SDL_BlitSurface(temp, NULL, m_Screen, &dest);
			SDL_FreeSurface(temp);
		}
		m_FontYellow->WriteTextCentered(m_Screen, 550, "CONGRATULATIONS !!!   YOU COMPLETED ALL LEVELS IN THIS LEVELSET.");
		m_FontBlue->WriteTextCentered(m_Screen, 570, "PRESS ANY KEY TO CONTINUE");
		Render(m_Screen);

		if (m_AudioAvailable && m_GameOptions.PlaySound)
			Mix_PlayChannel(-1, m_Sounds[15], 0);

		SDL_Delay(1300);
		while (NjamGetch(false) != SDLK_UNKNOWN);	// empty keyboard buffer
		NjamGetch(true);						// wait for a key
		CheckHiscore();							// if !hiscore
    }
    else					// duel over... we have a winner(0-3) in Winner variable
    {
    	char filename[] = "data/win?.jpg";
    	filename[8] = (char)('1'+Winner);
	 	SDL_Surface *temp = IMG_Load(filename);
	 	if (!temp)
	 	{
			printf("Loading file: %s", filename);
			printf(SDL_GetError());
			return;
	 	}

        SDL_FillRect(m_Screen, NULL, 0);
		SDL_Rect dest;
		NjamSetRect(dest, 175, 130);
		SDL_BlitSurface(temp, NULL, m_Screen, &dest);
		Render(m_Screen);

		if (m_AudioAvailable && m_GameOptions.PlaySound)
			Mix_PlayChannel(-1, m_Sounds[15], 0);

		SDL_Delay(4000);
		while (NjamGetch(false) != SDLK_UNKNOWN);	// empty keyboard buffer
		SDL_FreeSurface(temp);
    }
}
//---------------------------------------------------------------------------
void NjamEngine::SetupGame()
{
 	m_LivesLeft = 4;
    for (int i=0; i<4; i++)
    {
        m_Player[i].GamePoints = 0;
        for (int j=0; j<6; j++)
            m_Player[i].stats.data[j] = 0;
    }
}
//---------------------------------------------------------------------------
// returns winner (0-3), -1 if the game is a draw, and EndingType values...
int NjamEngine::PlayMap(bool Allow_powerups)
{
	if (Allow_powerups && m_GameType != gtOnePlayer && m_GameType != gtTwoPlayer && m_GameType != gtJoinDuel)
		m_Maps.AddPowerups(6);

	if (m_GameType == gtHostDuel || m_GameType == gtJoinDuel)
	{
		if (!NetworkMap())
			return etNetworkError;
	}

	if (m_GameType == gtTwoPlayer)
		m_Bonus = 1000;
	else if (m_GameType == gtOnePlayer)
		m_Bonus = 1850;
	else
		m_Bonus = 0;

    m_TripleDinged = false;

    m_CookiesLeft = 0;
    for (int i=0; i<MAPW; i++)
        for (int j=0; j<MAPH; j++)
            if (m_Maps.GetTile(i, j) == ttCookie)
                m_CookiesLeft++;

	if (m_Bonus && m_CookiesLeft > 260)		// map packed with cookies, add extra bonus time
		m_Bonus += (m_GameType == gtOnePlayer ? 7 : 4) * (m_CookiesLeft - 250);

    SetupMap();    		// zero points, place ghosts, players, juice, etc...

	if (m_GameOptions.UsedSkin == 0)		// random skin
	{
		if (m_GameType == gtOnePlayer || m_GameType == gtTwoPlayer)
			m_Maps.SetMapImages(m_Skins[m_NumberOfSkins * m_CurrentMap / m_Levels], 25, 25);
		else
			m_Maps.SetMapImages(m_Skins[NjamRandom(m_NumberOfSkins)], 25, 25);
	}
	else
		m_Maps.SetMapImages(m_Skins[m_GameOptions.UsedSkin-1], 25, 25);	// selected skin

	ReadySetGo();
	Uint32 total=0, counter=0;		// counter can be used for fps and network sync

    bool go=true;

    while (m_CookiesLeft && go)			// MAIN game loop
    {
        Uint32 StartTime = SDL_GetTicks();
        Animate();						// updates animations

        MoveGhosts();

		if (!ExchangeNetwork())			// server send, client recv, this is blocking function
		    return etNetworkError;
		if (m_NetworkEscapedGame)
			break;

        MovePlayers();
        if (m_CookiesLeft == 0)
        	break;

		bool FrameCounts = true;
        if (Collide(FrameCounts) == etGameOver)	// check collisions & if players lost a life
			return etGameOver;

        UpdateWorld();					// update world - juices, freeze, etc.
        RenderGameScreen();

		Render(m_Screen);

		// define it during compile time if you want to get screenshots in sc directory
		#ifdef MAKE_SCREENSHOTS
        char screenshot[20];
        sprintf(screenshot, "sc/%03d.bmp\0", counter/20);
        if (counter % 20 == 0)
        	SDL_SaveBMP(m_Screen, screenshot);
        #endif

		if (FrameCounts)	// calculate avg fps
		{
	        total += (SDL_GetTicks() - StartTime);
    	    counter++;
	        m_fps = (1000.00 * counter) / total;
	 	}

        do
        {
            if (!HandleKeyboard())		// if ESC pressed during game:
            {
            	if (m_GameType == gtOnePlayer || m_GameType == gtTwoPlayer)
            	{
					m_LivesLeft--;
					if (m_LivesLeft < 0)
						return etGameOver;

					if (!RenderLifeLostScreen())	// another escape to exit
						return etGameOver;

					continue;
            	}

				go = false;
            	if (m_GameType == gtHostDuel || m_GameType == gtJoinDuel)	// inform peer
            	{
            		int size = 16 + GHOSTMAX * 5;
            		Uint8 buffer[16 + GHOSTMAX * 5];

					// client has to pickup server's last frame data - otherwise that would be delivered later as a trash
					if (m_GameType == gtJoinDuel)
					{
						if (SDLNet_TCP_Recv(m_PeerSocket, buffer, size) < size)
						{
						    printf("SDLNet_TCP_Recv: %s\n", SDLNet_GetError());
							Disconnect();
					    	return etNetworkError;
						}
            			size = 16;	// set outgoing packet size
					}

            		buffer[0] = 'N';
            		buffer[1] = 1;
					if (size > SDLNet_TCP_Send(m_PeerSocket, buffer, size))
					{
					    printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
						Disconnect();
				    	return etNetworkError;
					}
            	}
            }
        }
        while (StartTime + 34 > SDL_GetTicks() && go);				// target about 30 fps
    }

	if (m_GameType == gtOnePlayer || m_GameType == gtTwoPlayer)		// non-duel game
	{
		if (m_Bonus > 0)
		{
			m_LivesLeft++;
			RenderLifeLostScreen(true);
		}
		return etLevelWon;
	}
	else															// duel
	{
	    int max=-1;
	    int Winner = 0;
	    for (int i=0; i<4; i++)
	    {
	        if (m_Player[i].MapPoints > max)
	        {
	            max = m_Player[i].MapPoints;
	            Winner = i;
	        }
	    }

	    // check if 2 players have the same score
	    int total=0;
	    for (int i=0; i<4; i++)
	        if (m_Player[i].MapPoints == max)
	            total++;

	    if (total > 1)
	        return -1;

	    return Winner;
	}
}
//---------------------------------------------------------------------------
// zero points, place ghosts, players, juice, etc...
void NjamEngine::SetupMap()
{
 	m_Freeze = 0;

    // init sprites
    for (int i=0; i<GHOSTMAX+4; i++)
    {
        m_Sprite[i].xo=m_Sprite[i].yo=m_Sprite[i].vx=m_Sprite[i].vy=0;
		m_Sprite[i].frame=m_Sprite[i].framemax=m_Sprite[i].delay=m_Sprite[i].special=m_Sprite[i].rotate=0;

        if (i > 3)	// ghosts
        {
            m_Sprite[i].delay = i * (MAXDELAY/GHOSTMAX);
            m_Sprite[i].SurfaceOffset = (i%3) * 25;
            MapPoint p = m_Maps.GhostHouseXY();
            m_Sprite[i].x = p.x;
            m_Sprite[i].y = p.y;
        }
        else		// players
        {
            m_Sprite[i].SurfaceOffset = 75 + i * 25;
            m_Player[i].Juice=m_Player[i].Invisible=m_Player[i].Freeze=0;
            m_Player[i].MapPoints = 0;
            m_Sprite[i].framemax = 5;	            // animation

            if (i == 1 || i == 2)					// players on right of screen - turn towards left
            	m_Sprite[i].rotate = 2;
        }
    }

    // place players - local players / server players
    m_Sprite[0].x = 1;
    m_Sprite[0].y = 1;
    m_Sprite[1].x = MAPW - 2;
    m_Sprite[1].y = MAPH - 2;

    m_Sprite[2].x = MAPW - 2;	// remote players (on server) / client players
    m_Sprite[2].y = 1;
    m_Sprite[3].x = 1;
    m_Sprite[3].y = MAPH - 2;
}
//---------------------------------------------------------------------------
void NjamEngine::RenderGameScreen()
{
    // render map
    m_Maps.RenderMap(m_Screen);

    // render player powerups (glow, invisibility) - goes first since it's below sprite
    // during this loop - find who has the most points
    int maxp = 0;
    for (int i=0; i<4; i++)
    {
        NjamPlayer *p = &m_Player[i];
        NjamSprite *s = &m_Sprite[i];

        if (p->Playing && p->MapPoints > maxp)
            maxp = p->MapPoints;

        if (!s->delay && (p->Juice || p->Invisible))
        {
            SDL_Rect dest, src;
            NjamSetRect(dest, s->x*25 + s->xo*5, s->y*25 + s->yo*5, 25, 25);

            if (p->Juice)
                NjamSetRect(src, 0, 50, 25, 25);
            else
                NjamSetRect(src, 0, 75, 25, 25);

			SDL_BlitSurface(m_SpriteImage, &src, m_Screen, &dest);
        }
    }

    RenderSprites();

    // render points & juice
    SDL_Rect src, dest;						// render background (use part of main menu image)
    NjamSetRect(dest, 700, 0, 100, 600);
	SDL_FillRect(m_Screen, &dest, 0);

    char buff[33];
   	bool Duel_game = (m_GameType != gtOnePlayer && m_GameType != gtTwoPlayer);
	//                               Yellow    L.Green     Red        D.Green
	const Uint8 ply_color[12] = { 255,255,25, 25,255,25, 255,25,25, 25,155,155 };

	int Dinger = 0;
	int PlayersPlaying = 0;
	for (int i=0; i<4; i++)
        if (m_Player[i].Playing)
			PlayersPlaying++;

    for (int i=0; i<4; i++)					// render points & (lives | victories)
    {
        NjamPlayer *p = &m_Player[i];

        if (p->Playing)
        {
        	// show current score in duel mode / or lives left in non-duel mode
	    	if (Duel_game)
	    	{
                sprintf(buff, "%d\0", p->GamePoints);
	            m_FontYellow->WriteTextXY(m_Screen, 705 + 25 * i, 0, buff);
	     	}
	     	else	// draw nice yellow pacman-s
	     	{
	     		SDL_Rect src, dest;
	     		NjamSetRect(src, 75, 75, 25, 25);
	     		dest.y = 0;

	     		int d;
				if (m_LivesLeft)
					d = 100 / m_LivesLeft;
				if (d > 25)
					d = 25;
				for (int i=0; i<m_LivesLeft; i++)
				{
					dest.x = 700 + d * i;
                    SDL_BlitSurface(m_SpriteImage, &src, m_Screen, &dest);
				}
	     	}

			// draw vertical bar (how much points player has)
            Uint32 FillColor = SDL_MapRGB(m_Screen->format, ply_color[i*3], ply_color[i*3+1], ply_color[i*3+2]);
            SDL_Rect re;
            NjamSetRect(re, 702 + i*25, 25, 20, p->MapPoints);
            SDL_FillRect(m_Screen, &re, FillColor);

			// draw thin vertical gray bar (how much points would player have if he takes all that's left)
            if (Duel_game && maxp != p->MapPoints)
            {
	            FillColor = SDL_MapRGB(m_Screen->format, 150, 180, 200);

                if (p->MapPoints + m_CookiesLeft < maxp)
                {
		            FillColor = SDL_MapRGB(m_Screen->format, 255, 255, 255);
					Dinger++;
					if (!m_TripleDinged)
					{
						if (Dinger + 1 == PlayersPlaying)
						{
							m_TripleDinged = true;
							if (m_GameOptions.PlaySound)
								Mix_PlayChannel(-1, m_Sounds[14], 0);
						}
					}
		        }

                NjamSetRect(re, 710+i*25, 26+p->MapPoints, 4, m_CookiesLeft);
	            SDL_FillRect(m_Screen, &re, FillColor);
            }
        }

        if (p->Juice && p->Juice < 60)			// superpower is going to run out in few moments
        {										// so we draw a countdown: 3, 2, 1, ...
            sprintf(buff, "%d\0", p->Juice / 20);
            m_FontYellow->WriteTextXY(m_Screen,
	            					25 * m_Sprite[i].x + 5 * m_Sprite[i].xo - 10,
									25 * m_Sprite[i].y + 5 * m_Sprite[i].yo - 10, buff);
        }
    }

	// Dinger = nr. of players with white bar
	if (m_TripleDinged && PlayersPlaying - Dinger > 1)	// someone has a chance again
	{
		m_TripleDinged = false;
		if (m_GameOptions.PlaySound)
			Mix_PlayChannel(-1, m_Sounds[8], 0);
	}

    if (m_GameType == gtOnePlayer || m_GameType == gtTwoPlayer)
    {
        sprintf(buff, "LEVEL: %02d\0", m_CurrentMap + 1);
		m_FontYellow->WriteTextXY(m_Screen, 705, 562, buff);
	}

	if (m_GameType == gtOnePlayer)
	{
        sprintf(buff, "PTS: %4d\0", m_Player[0].GamePoints);
		m_FontYellow->WriteTextXY(m_Screen, 705, 543, buff);
	}

    sprintf(buff, "%2.0f FPS\0", m_fps);
	m_FontYellow->WriteTextXY(m_Screen, 705, 581, buff);

	if (m_Bonus)
	{
		int minus = 0;
		if (m_GameType == gtOnePlayer)
			minus = 19;

		SDL_Rect src, dest;

        NjamSetRect(dest, 774, 559 - m_Bonus/10 - minus, 22, m_Bonus/10 + 1);
        SDL_FillRect(m_Screen, &dest, 0);

		NjamSetRect(src, 0, 125 + 209 - m_Bonus/10, 20, m_Bonus/10);
		NjamSetRect(dest, 775, 559 - m_Bonus/10 - minus);
        SDL_BlitSurface(m_SpriteImage, &src, m_Screen, &dest);
	}
}
//---------------------------------------------------------------------------
void NjamEngine::RenderSprites()
{
    MapPoint gh = m_Maps.GhostHouseXY();
    for (int i=0; i<GHOSTMAX+4; i++)
    {
        NjamSprite *s = &m_Sprite[i];
        if (s->delay > MAXDELAY/2 && s->delay%2 && i > 3 && (gh.x != s->x || gh.y != s->y))		// dead ghost?
        {
	        SDL_Rect dest, src;		// show him on the screen for a while - blinking fast
	        NjamSetRect(dest, s->x * 25 + s->xo * 5, s->y * 25 + s->yo * 5);
	        NjamSetRect(src, 0, 25, 25, 25);
	        SDL_BlitSurface(m_SpriteImage, &src, m_Screen, &dest);
	        continue;
        }

        if (s->delay > 0 || (i < 4 && !m_Player[i].Playing))	// not visible | not playing
            continue;

        // vertical offset because of rotation: 0 Right, 1 Down, 2 Left, 3 Up
        int offset = s->rotate * 150;
        SDL_Rect dest, src;
        NjamSetRect(dest, s->x * 25 + s->xo * 5, s->y * 25 + s->yo * 5);
        NjamSetRect(src, s->SurfaceOffset, offset + s->frame * 25, 25, 25);
        SDL_BlitSurface(m_SpriteImage, &src, m_Screen, &dest);

		// Alpha?
		if (i>3 && m_Freeze)	// ghost & freezer active
		{
			int top = (m_Freeze > MAXDELAY ? MAXDELAY : m_Freeze);
			SDL_SetSurfaceAlphaMod(m_SpriteImage, 150 + 105 * top/MAXDELAY);
			NjamSetRect(dest, s->x * 25 + s->xo*5 - 1, s->y*25 + s->yo*5 - 2);
			NjamSetRect(src, 35, 50, 28, 31);
			SDL_BlitSurface(m_SpriteImage, &src, m_Screen, &dest);
			SDL_SetSurfaceAlphaMod(m_SpriteImage, 255);
		}

		// frozen player
		if (i<4 && m_Player[i].Freeze)
		{
			int top = (m_Player[i].Freeze > MAXDELAY/2 ? MAXDELAY/2 : m_Player[i].Freeze);
			SDL_SetSurfaceAlphaMod(m_SpriteImage, 150 + 105 * top / MAXDELAY);			
			NjamSetRect(dest, s->x * 25 + s->xo*5 - 1, s->y*25 + s->yo*5 - 2);
			NjamSetRect(src, 35, 50, 28, 31);
			SDL_BlitSurface(m_SpriteImage, &src, m_Screen, &dest);
			SDL_SetSurfaceAlphaMod(m_SpriteImage, 255);
		}
    }
}
//---------------------------------------------------------------------------
void NjamEngine::Animate()
{
    for (int i=0; i<4+GHOSTMAX; i++)
    {
        NjamSprite *s = &m_Sprite[i];

        s->frame++;
        if (s->frame > s->framemax)
            s->frame = 0;
    }
}
//---------------------------------------------------------------------------
void NjamEngine::MoveGhosts()	// AI stuff ;)
{
    MapPoint Gh = m_Maps.GhostHouseXY();
    m_Maps.SetTile(Gh.x, Gh.y, ttGHouse);	// assume noone is there
    for (int i=4; i<m_Ghosts+4; i++)		// only move active ghosts
    {
        NjamSprite *s = &m_Sprite[i];
        if (s->delay > 0)
        {
	        if (s->delay == 1)      		// will become 0 so set him up
	        {
                MapPoint gh = m_Maps.GhostHouseXY();
                s->x = gh.x;
                s->y = gh.y;
                s->xo = 0;
                s->yo = 0;
	            s->vx = 0;
	            s->vy = 1;    // go down...
 				if( m_Maps.GetTile( s->x+s->vx, s->y+s->vy ) == ttWall )				// or up
 				{
 					s->vy = -1;
 					if( m_Maps.GetTile( s->x+s->vx, s->y+s->vy ) == ttWall )			// or left
 					{
 						s->vx = -1;
 						s->vy = 0;
 						if( m_Maps.GetTile( s->x+s->vx, s->y+s->vy ) == ttWall )		// or right
 						{
 							s->vx = 1;
 							if( m_Maps.GetTile( s->x+s->vx, s->y+s->vy ) == ttWall )	// walls everywhere
 								s->vx = 0;
 						}
 					}
 				}
	        }
	        else
	            m_Maps.SetTile(Gh.x, Gh.y, ttGHouseActive);

			if (m_Freeze > 0)
			{
				if (s->delay%2)
					s->delay--;
				else
					s->delay++;
				continue;
			}

            s->delay--;
        }

        if (s->delay || m_Freeze>0 || m_GameType == gtJoinDuel)
            continue;

        s->xo += s->vx;
        s->yo += s->vy;

        if (s->xo > 4)		// move to next field
        {
            s->xo = 0;
            s->x++;
        }
        else if (s->xo < 0)
        {
            s->xo = 4;
            s->x--;
        }

        if (s->yo > 4)
        {
            s->yo=0;
            s->y++;
        }
        else if (s->yo < 0)
        {
            s->yo = 4;
            s->y--;
        }

        // changing directions at crossroads (red ghosts just go straight)
        //                              blue ghost ||  orange      &&  !hunter
        if (s->xo==0 && s->yo==0 && (i%3==gtSTUPID || i%3==gtSMART && s->special==0))
        {
        	int Paths = m_Maps.CrossRoads(s->x,s->y);
            if (Paths > 1 && i%3==gtSMART || Paths > 2)
	            m_Maps.TurnOnCrossRoads(s->x, s->y, s->vx, s->vy, NjamRandom(Paths-1));
        }

        // check - does the ghost see the player...
        // conditions:		1. either x or y coords are equal
		//                  2. there are no obstacles in between
        if (s->xo==0 && s->yo==0)
        {
            int nearest  = -1;
            int distance = 1000;		// dummy value (more than map_w * 5)
            bool by_x = false;			// whether the direction is x or y axis

            // FIND NEAREST PLAYER
            for (int k=0;k<4;k++)   // 4 players
            {
                NjamPlayer *p = &m_Player[k];
                NjamSprite *sp = &m_Sprite[k];
                if (sp->delay || p->Playing==0 || p->Invisible>0)	// works only for active ply
                    continue;

                if (s->x == sp->x)		// if x coords are equal
                {
                    int from = (s->y < sp->y ? s->y : sp->y);	// higher y coord
                    int to = s->y + sp->y - from;				// lower  y coord

                    bool go_there = true;
                    for (int j=from ; j<to ; j++)     			// check obstacles
                    {
                        if (m_Maps.GetTile(s->x, j) == ttWall)
                        {
                            go_there = false;
                            break;
                        }
                    }

                    if (go_there && distance > to - from)	// no obstacle
                    {
                        distance = to - from;
                        nearest = k;
                        by_x = true;
                    }
                    continue;		// cannot be both x and y with same ply
                }

                if (s->y == sp->y)		// if y coords are equal
                {
                    int from = (s->x < sp->x ? s->x : sp->x);	// higher x coord
                    int to = s->x + sp->x - from;				// lower  x coord

                    bool go_there=true;
                    for (int j=from ; j<to ; j++)     			// check obstacles
                    {
                        if (m_Maps.GetTile( j, s->y) == ttWall)
                        {
                            go_there=false;
                            break;
                        }
                    }
                    if (go_there && distance > to - from)        // no obstacles
                    {
                        distance = to - from;
                        nearest = k;
                        by_x = false;
                    }
                }   // end if (y coords)...
            }   // end (for k = ) end check for nearest ply

            if (nearest > -1)      // sees a ply
            {
                NjamPlayer *p = &m_Player[nearest];
                NjamSprite *sp = &m_Sprite[nearest];

                if (by_x)
                {
                    if (p->Juice == 0) 				// ply has no super-power (juice)
                    {
                        if (i%3 != gtSTUPID)		// red & orange ghost
                        {
                            // moves towards ply
                            s->vy = (s->y < sp->y ? 1 : -1);
                            s->vx = 0;
                            if (i%3 == gtSMART)		// orange becomes hunter
                                s->special = 1;
                        }
                    }
                    else			// ply has juice
                    {
                        int j = s->y + (s->y < sp->y ? -1 : 1);
                        if (m_Maps.GetTile(s->x, j) != ttWall)	// run away
                        {
                            s->vy = (s->y < sp->y ? -1 : 1);
                            s->vx = 0;
                        }
                    }
                }
                else        // by y coord
                {
                    if (p->Juice == 0) // ply has no super-power (juice)
                    {
                        if (i%3 != gtSTUPID)		// red & orange ghost
                        {
                            // moves towards ply
                            s->vx = (s->x < sp->x ? 1 : -1);
                            s->vy = 0;
                            if (i%3 == gtSMART) 	// orange becomes hunter
                                s->special = 1;
                        }
                    }
                    else			// ply has juice
                    {
                        int j = s->x + (s->x < sp->x ? -1 : 1);
                        if (m_Maps.GetTile( j, s->y) != ttWall)		// run away
                        {
                            s->vx = (s->x < sp->x ? -1 : 1);
                            s->vy = 0;
                        }
                    }
                }
            }   // end if nearest
        }   // end if xo=0&yo=0

        // check whether the ghost would hit a wall, and change direction if needed
        while (s->xo==0 && s->yo==0 && m_Maps.GetTile(s->x + s->vx, s->y + s->vy)==ttWall)
        {
			// if ghost is surrounded with walls, stop
			if (m_Maps.GetTile(s->x - 1, s->y) == ttWall &&	m_Maps.GetTile(s->x + 1, s->y) == ttWall &&
				m_Maps.GetTile(s->x, s->y - 1) == ttWall &&	m_Maps.GetTile(s->x, s->y + 1) == ttWall)
			{
				s->vx = 0;
				s->vy = 0;
				break;
			}

            if (i%3 == gtSMART)
                s->special=0;
            s->vx=0;
            s->vy=0;
            if (NjamRandom(2)==0)			// up or down
                s->vx=-1+2*NjamRandom(2);
            else							// left or right
                s->vy=-1+2*NjamRandom(2);
        }
    }
}
//-----------------------------------------------------------------------------
bool NjamEngine::HandleKeyboard()
{
    SDL_Keycode key;

    // process all keypresses
    while ((key = NjamGetch(false)) != SDLK_UNKNOWN)
    {
        if (key == SDLK_ESCAPE)
            return false;

        // pause
        if (key == SDLK_p && m_GameType != gtJoinDuel)		// only server can pause the game
        {
            while (NjamGetch(false) != SDLK_UNKNOWN);	// wait to release the key
            RenderGameScreen();
            SDL_Rect r;
			NjamSetRect(r, 0, 0, 700, 600);
			SurfaceEffect(m_Screen, r, fxBlackWhite);
			NjamSetRect(r, 359, 284, 82, 27);
			Uint32 yello = SDL_MapRGB(m_Screen->format, 255, 255, 200);
			SDL_FillRect(m_Screen, &r, yello);
            NjamSetRect (r, 360, 285, 80, 25);
		 	Uint32 black = SDL_MapRGB(m_Screen->format, 0, 0, 0);
	        SDL_FillRect(m_Screen, &r, black);
	        m_FontYellow->WriteTextXY(m_Screen, 371, 290, "PAUSED");
			Render(m_Screen);
            while (SDLK_p != NjamGetch(false));    	// wait to press the key again
        }

		int ply0 = 0, ply1 = 1;		// default, except for joined net-game
		if (m_GameType == gtJoinDuel)
		{
			ply0 = 2;
			ply1 = 3;
		}

		if (m_Player[ply0].Playing)
		{
	  		if (key == SDLK_RIGHT)
	        {
	            m_Sprite[ply0].rotate = 0;
	            if (m_Sprite[ply0].vx == -1)
	                m_Sprite[ply0].vx = 1;
	        }
	        else if (key == SDLK_DOWN)
	        {
	            m_Sprite[ply0].rotate = 1;
	            if (m_Sprite[ply0].vy == -1)
	                m_Sprite[ply0].vy = 1;
	        }
	        else if (key == SDLK_LEFT)
	        {
	            m_Sprite[ply0].rotate = 2;
	            if (m_Sprite[ply0].vx == 1)
	                m_Sprite[ply0].vx = -1;
	        }
	        else if (key == SDLK_UP)
	        {
	            m_Sprite[ply0].rotate = 3;
	            if (m_Sprite[ply0].vy == 1)
	                m_Sprite[ply0].vy = -1;
	        }
	 	}

        if (m_Player[ply1].Playing)
        {
            if (key == SDLK_g)
            {
                m_Sprite[ply1].rotate = 0;
                if (m_Sprite[ply1].vx == -1)
                    m_Sprite[ply1].vx = 1;
            }
            else if (key == SDLK_f)
            {
                m_Sprite[ply1].rotate = 1;
                if (m_Sprite[ply1].vy == -1)
                    m_Sprite[ply1].vy = 1;
            }
            else if (key == SDLK_d)
            {
                m_Sprite[ply1].rotate = 2;
                if (m_Sprite[ply1].vx == 1)
                    m_Sprite[ply1].vx = -1;
            }
            else if (key == SDLK_r)
            {
                m_Sprite[ply1].rotate = 3;
                if (m_Sprite[ply1].vy == 1)
                    m_Sprite[ply1].vy = -1;
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NjamEngine::MovePlayers()
{
    // to take some map values
    MapPoint pt;
	TileType t;

    for (int i=0; i<4; i++)		// 4 ply
    {
        NjamSprite *s = &m_Sprite[i];
        if (s->delay > 0 || !m_Player[i].Playing)
            continue;

		if (m_Player[i].Freeze)
		{
			m_Player[i].Freeze--;
			continue;
		}

		bool was_moving = (s->vx || s->vy);

        // on center of tile
        if (s->xo == 0 && s->yo == 0)
        {
            t = m_Maps.GetTile(s->x, s->y);

			if (ttWall == m_Maps.GetTile(s->x + s->vx, s->y + s->vy))
				s->vx = s->vy = 0;

            switch (t)
            {
				case ttPoints:
                    m_Maps.SetTile(s->x, s->y, ttEmpty);
					m_Player[i].GamePoints += 50;
                    if (m_GameOptions.PlaySound)
						if (-1 == Mix_PlayChannel(-1, m_Sounds[16], 0))
							printf("Mix_PlayChannel: %s\n",Mix_GetError());
					break;
                case ttWall:		// dies (trap kills him)
                    s->vx = 0;
                    s->vy = 0;
                    s->xo = 0;
                    s->yo = 0;
                    s->delay = MAXDELAY * 2;
                    pt = m_Maps.FindSpecificTile(ttDoor);
                    s->x = pt.x;
                    s->y = pt.y;

                    if (m_GameOptions.PlaySound)
						Mix_PlayChannel(-1, m_Sounds[6], 0);
					break;
                case ttCookie:
                    m_CookiesLeft--;
                    m_Player[i].MapPoints++;
					if (m_GameType == gtOnePlayer)
						m_Player[0].GamePoints++;
                    m_Maps.SetTile(s->x, s->y, ttEmpty);
                    m_Player[i].stats.indata.cookies++;
                    break;
                case ttFreezer:
                    m_Freeze += MAXDELAY;
                    m_Maps.SetTile(s->x, s->y, ttEmpty);

                    if (m_GameOptions.PlaySound)
						if (-1 == Mix_PlayChannel(-1, m_Sounds[13], 0))
							printf("Mix_PlayChannel: %s\n",Mix_GetError());
                    break;
                case ttTrap:
                    m_Maps.SetTile(s->x, s->y, ttWall);

                    if (m_GameOptions.PlaySound)
						if (-1 == Mix_PlayChannel(-1, m_Sounds[12], 0))
							printf("Mix_PlayChannel: %s\n",Mix_GetError());
                    break;
                case ttInvisible:
                    m_Player[i].Invisible = MAXDELAY * 3;
                    m_Maps.SetTile(s->x, s->y, ttEmpty);
                    m_Player[i].stats.indata.invisibles++;

                    if (m_GameOptions.PlaySound)
						if (-1 == Mix_PlayChannel(-1, m_Sounds[11], 0))
							printf("Mix_PlayChannel: %s\n",Mix_GetError());
                    break;
                case ttJuice:
                    if (m_Player[i].Juice)	// cannot have another one
					{
						// move the juice to opposite corner of the map.
						// this is made so that one player that first gets the juice cannot go
						// to the next juice and "protect" it so others can't take it.
						if (m_GameType == gtDuel || m_GameType == gtHostDuel || m_GameType == gtJoinDuel)
						{
							// get the juice quadrant and move it to the opposite corner
							int xc = 1, yc = 1;
							if (s->x < MAPW/2)
								xc = MAPW-2;
							if (s->y < MAPH/2)
								yc = MAPH-2;
							m_Maps.SetTile(s->x, s->y, ttEmpty);
							m_Maps.SetTile(xc, yc, ttJuice);
							if (m_GameOptions.PlaySound)		// teleport sound
								if (-1 == Mix_PlayChannel(-1, m_Sounds[10], 0))
									printf("Mix_PlayChannel: %s\n",Mix_GetError());
						}

                        break;
					}
                    m_Player[i].Juice = MAXDELAY * 2;
                    m_Maps.SetTile(s->x, s->y, ttEmpty);
                    m_Player[i].stats.indata.juices++;

                    if (m_GameOptions.PlaySound)
						if (-1 == Mix_PlayChannel(-1, m_Sounds[9], 0))
							printf("Mix_PlayChannel: %s\n",Mix_GetError());
                break;
            }
        }

        s->xo += s->vx;   // add vx & vx
        s->yo += s->vy;   // to x & y coord
        if (s->xo > 4)      // check if he's moving into next...
        {
            s->xo = 0;
            s->x++;
        }
        else if (s->xo < 0)   // ... or previous field
        {
            s->xo = 4;
            s->x--;
        }

        if (s->yo > 4)          // same for y coord
        {
            s->yo = 0;
            s->y++;
        }
        else if (s->yo < 0)
        {
            s->yo = 4;
            s->y--;
        }

       /* checking possibility to change direction...
            Explanation:
            -player (pacman) can turn himself even before he changes
             direction of moving, but he can also stand still for a
             while, and then turn
            -the following code checks whether the direction the player
			 is facing matches the direction he is moving in, and if they
			 differ - whether they can be aligned.
			 ex: player can be moving right, but facing down (if there is a
			 wall beneath him. Once that wall is gone (ex. he moved right
			 more and there isn't any wall beneath anymore) he starts moving
			 down.
            -case conditions represent moving directions: right, down, left, up
       */

        if (s->xo == 0 && s->yo == 0)
        {
            switch (s->rotate)
            {							// right
                case 0:	t = m_Maps.GetTile(s->x + 1, s->y);
                        if (t != ttWall)
                        {
                            s->vx=1;
                            s->vy=0;
                        }
                        break;
                case 1:	t = m_Maps.GetTile(s->x, s->y + 1);
                        if (t != ttWall)
                        {
                            s->vx=0;
                            s->vy=1;
                        }
                            break;
                case 2:	t = m_Maps.GetTile(s->x - 1, s->y);
                        if (t != ttWall)
                        {
                            s->vy=0;
                            s->vx=-1;
                        }
                            break;
                case 3:	t = m_Maps.GetTile(s->x, s->y - 1);
                        if (t != ttWall)
                        {
                            s->vy=-1;
                            s->vx=0;
                        }
            }
        }

		// if he's gonna hit the wall - stop
		if (s->xo == 0 && s->yo == 0)
		{
			t = m_Maps.GetTile(s->x, s->y);
			if (was_moving && t == ttTeleport)
			{
				pt = m_Maps.FindOtherTeleport(s->x, s->y);
				s->x = pt.x;
				s->y = pt.y;

				if (m_GameOptions.PlaySound)		// teleport sound
					if (-1 == Mix_PlayChannel(-1, m_Sounds[10], 0))
						printf("Mix_PlayChannel: %s\n",Mix_GetError());
			}

			if (ttWall == m_Maps.GetTile(s->x + s->vx, s->y + s->vy))
			{
				s->vx=0;
				s->vy=0;
			}
		}
    }
}
//---------------------------------------------------------------------------
void NjamEngine::UpdateWorld()
{
    if (m_Freeze > 0)
        m_Freeze--;

    if (m_Bonus > 0)
    {
    	m_Bonus--;
    	if (m_Bonus == 0)
		{
			if (m_AudioAvailable && m_GameOptions.PlaySound)	// play tripple-ding
				Mix_PlayChannel(-1, m_Sounds[14], 0);
		}

		// chance to get some random powerup
		// ~10 times per level
		// 20% chance each time
		if (m_GameType == gtOnePlayer && m_Bonus%180 == 120 && NjamRandom(5) == 1)
		{
			while (true)	// find empty tile
			{
				int x = 1+NjamRandom(MAPW - 2);
				int y = 1+NjamRandom(MAPH - 2);
				if (m_Maps.GetTile(x, y) == ttEmpty)
				{
					m_Maps.SetTile(x, y, ttPoints);
					break;
				}
			}
		}
	}

    // reduce juice & invisible to all players
    for (int i=0; i<4; i++)
    {
    	if (!m_Player[i].Playing)
    		continue;

        if (m_Player[i].Juice > 0)
            m_Player[i].Juice--;
        if (m_Player[i].Invisible > 0)
            m_Player[i].Invisible--;
        //if (m_Player[i].Freeze > 0)
        //    m_Player[i].Freeze--;
		if (m_Sprite[i].delay)
            m_Sprite[i].delay--;
    }
}
//---------------------------------------------------------------------------
// This function can warn if player lost all lives in non-duel game
EndingType NjamEngine::Collide(bool& FrameCounts)
{
    NjamSprite *s, *p;

    // ply vs ply (only in duel games)
    if (m_GameType != gtOnePlayer && m_GameType != gtTwoPlayer)		// duel game
    {
	    for (int i=0; i<3; i++)
	    {
	        s = &m_Sprite[i];
			if (s->delay || !m_Player[i].Playing)  // ply1 is inactive
	            continue;

	        int xi = s->x*5 + s->xo;       // x & y coords - ply1
	        int yi = s->y*5 + s->yo;

	        for (int j=i+1; j<4; j++)
	        {
	            p = &m_Sprite[j];
	            if (p->delay || !m_Player[j].Playing)  // ply2 is inactive
	                continue;

	            // are ply "i" and ply "j" in collision?
	            int xj = p->x*5 + p->xo;	// coord ply2
	            int yj = p->y*5 + p->yo;
	            if (abs(xi-xj)+abs(yi-yj) < 4)	// contact ply vs ply
	            {
	                if (m_Player[i].Juice && !m_Player[j].Juice)	// super power "i"
	                {
	                    p->xo = 0;
	                    p->yo = 0;
	                    p->vx = 0;
	                    p->vy = 0;
	                    p->delay = MAXDELAY;
	                    MapPoint pt = m_Maps.FindSpecificTile(ttDoor);
	                    p->x = pt.x;
	                    p->y = pt.y;
	                    m_Player[i].stats.indata.player_kills++;
        	            if (m_GameOptions.PlaySound)
							if (-1 == Mix_PlayChannel(-1, m_Sounds[6], 0))
								printf("Mix_PlayChannel: %s\n",Mix_GetError());
	                }
	                else if (!m_Player[i].Juice && m_Player[j].Juice)	// super power "j"
	                {
	                    s->xo = 0;
	                    s->yo = 0;
	                    s->vx = 0;
	                    s->vy = 0;
	                    s->delay = MAXDELAY;
	                    MapPoint pt = m_Maps.FindSpecificTile(ttDoor);
	                    s->x = pt.x;
	                    s->y = pt.y;
	                    m_Player[j].stats.indata.player_kills++;
	                    if (m_GameOptions.PlaySound)
							if (-1 == Mix_PlayChannel(-1, m_Sounds[6], 0))
								printf("Mix_PlayChannel: %s\n",Mix_GetError());
	                }
	            }
	        }
	    }
	}

    // ghost vs ply
	for (int i=4;i<GHOSTMAX+4;i++)
	{
        s = &m_Sprite[i];
		if (s->delay)  // inactive ghost
            continue;

        int x = s->x*5 + s->xo;       // x & y coords - ghost
        int y = s->y*5 + s->yo;
        for (int k=0;k<4;k++)
        {
            p = &m_Sprite[k];
            if (p->delay || !m_Player[k].Playing)		// (in)active ply
                continue;

            int x0 = p->x*5 + p->xo;	// coord ply
            int y0 = p->y*5 + p->yo;
            if (abs(x-x0)+abs(y-y0) < 4)	// contact ghost vs ply "k"
            {
                if (m_Player[k].Juice)	// super power
                {
                    s->delay = MAXDELAY;	// ghost dies
                    m_Player[k].MapPoints += 5;
					if (m_GameType == gtOnePlayer)
						m_Player[0].GamePoints += 5;
                    m_Player[k].stats.indata.kills++;
                    if (m_GameOptions.PlaySound)
						if (-1 == Mix_PlayChannel(-1, m_Sounds[3 + NjamRandom(3)], 0))
							printf("Mix_PlayChannel: %s\n",Mix_GetError());
                }
                else                // no super power
                {
                    p->delay = MAXDELAY;
                    m_Player[k].stats.indata.deaths++;

                    if (m_GameOptions.PlaySound)
						if (-1 == Mix_PlayChannel(-1, m_Sounds[NjamRandom(3)], 0))
							printf("Mix_PlayChannel: %s\n",Mix_GetError());

					if (m_GameType == gtOnePlayer || m_GameType == gtTwoPlayer)		// ply lost a life, restart
					{
						FrameCounts = false;	// don't account this frame in fps counter because there will be
												// a huge delay
						m_LivesLeft--;
						if (m_LivesLeft < 0)
							return etGameOver;

						p->delay = 0;	// show player on screen
						if (!RenderLifeLostScreen())	// another ESC
						{
							//m_CurrentMap--;			// prevent skipping levels
							return etGameOver;
						}
					}
					else
					{
	                    p->xo = p->yo = p->vx = p->vy = 0;
	                    MapPoint pt = m_Maps.FindSpecificTile(ttDoor);
	                    p->x = pt.x;
	                    p->y = pt.y;

	                    if (m_Player[k].Invisible < 1.5*MAXDELAY)
							m_Player[k].Invisible = (int)(1.5*MAXDELAY);
				    }
                }
            }
        }
    }
	return etNone;
}
//---------------------------------------------------------------------------
bool NjamEngine::RenderLifeLostScreen(bool LifeAwarded)
{
 	if (m_AudioAvailable && m_GameOptions.PlayMusic && Mix_PlayingMusic() && !LifeAwarded)
		Mix_PauseMusic();

    RenderGameScreen();

	// show lives left screen (box with pacmans...)
	SDL_Rect r, src;
	NjamSetRect(r, 300, 260, 250, 81);
 	Uint32 black = SDL_MapRGB(m_Screen->format, 0, 0, 0);
    SDL_FillRect(m_Screen, &r, black);
    NjamSetRect(r, 310, 270, 230, 60);
 	Uint32 green = SDL_MapRGB(m_Screen->format, 25, 155, 25);
    SDL_FillRect(m_Screen, &r, green);

	if (LifeAwarded)
		m_FontYellow->WriteTextXY(m_Screen, 320, 280, "BONUS LIFE");
	else
		m_FontYellow->WriteTextXY(m_Screen, 320, 280, "LIVES LEFT");

	NjamSetRect(src, 75, 75, 25, 25);
	for (int i=0; i<m_LivesLeft; i++)
	{
		if (m_LivesLeft > 8)
			NjamSetRect(r, 320 + i * 10, 300);
		else
			NjamSetRect(r, 320 + i * 25, 300);

        SDL_BlitSurface(m_SpriteImage, &src, m_Screen, &r);
    }

	if (!LifeAwarded)
	    m_FontBlue->WriteTextXY(m_Screen, 317, 331, "PRESS SPACE TO CONTINUE, ESC TO QUIT");

	Render(m_Screen);

	if (LifeAwarded)
	{
	 	if (m_AudioAvailable)
		{
			if (m_GameOptions.PlaySound)
				Mix_PlayChannel(-1, m_Sounds[15], 0);
		 	if (m_GameOptions.PlayMusic && Mix_PlayingMusic())
				Mix_HaltMusic();
		}
		SDL_Delay(2000);
		return true;
	}

	while (NjamGetch(false) != SDLK_UNKNOWN);		// empty keyboard buffer

	SDL_Keycode key;
	do
	{
		key = NjamGetch(true);
		if (SDLK_ESCAPE == key)
			return false;
	}
	while (SDLK_SPACE != key);	// wait for a SPACE key

	// player keeps playing, setup the delays
    for (int i=0; i<GHOSTMAX+4; i++)
    {
        m_Sprite[i].xo=m_Sprite[i].yo=m_Sprite[i].vx=m_Sprite[i].vy=0;
    	if (i<4)
    	{
    		MapPoint p = m_Maps.FindSpecificTile(ttDoor);
    		m_Sprite[i].delay = 0;
    		m_Sprite[i].x = p.x;
            m_Sprite[i].y = p.y;
            m_Sprite[i].rotate = 0;	// turn right since the Door is always at left edge of screen
    	}
    	else
    	{
            m_Sprite[i].delay = i * (MAXDELAY/GHOSTMAX);
            MapPoint p = m_Maps.GhostHouseXY();
            m_Sprite[i].x = p.x;
            m_Sprite[i].y = p.y;
    	}
    }

	ReadySetGo();

 	if (m_AudioAvailable && m_GameOptions.PlayMusic && Mix_PausedMusic())
		Mix_ResumeMusic();

	return true;
}
//---------------------------------------------------------------------------
// renders ready, set, go messages
void NjamEngine::ReadySetGo()
{
    char message[3][6] = { "READY", " SET ", " GO  " };
    for (int i=0; i<3; i++)
    {
        RenderGameScreen();			// draw starting screen

        SDL_Rect r;

        NjamSetRect(r, 359, 284, 77, 27);
		Uint32 yello = SDL_MapRGB(m_Screen->format, 255, 255, 200);
        SDL_FillRect(m_Screen, &r, yello);

        NjamSetRect(r, 360, 285, 75, 25);
		Uint32 black = SDL_MapRGB(m_Screen->format, 0, 0, 0);
        SDL_FillRect(m_Screen, &r, black);

        m_FontYellow->WriteTextXY(m_Screen, 373 + (i == 2 ? 5 : 0), 289, message[i]);
		Render(m_Screen);
        SDL_Delay(1000);
    }
}
//---------------------------------------------------------------------------
void NjamEngine::DrawGameStats()
{
 	SDL_Rect src, dest;
 	NjamSetRect(dest, 42, 60);
	SDL_BlitSurface(m_StatsImage, NULL, m_Screen, &dest);

    char points[33];

    // render player sprites and stats
    for (int i=0; i<4; i++)
    {
        if (!m_Player[i].Playing)
            continue;

		NjamSetRect(src, m_Sprite[i].SurfaceOffset, 25, 25, 25);
		NjamSetRect(dest, 90, 220 + 30*i);
		SDL_BlitSurface(m_SpriteImage, &src, m_Screen, &dest);

		if (m_GameType == gtOnePlayer || m_GameType == gtTwoPlayer)
	        sprintf(points, "  (%3d)\0", m_Player[i].MapPoints);
		else
	        sprintf(points, "%01d (%3d)\0", m_Player[i].GamePoints, m_Player[i].MapPoints);
        m_FontYellow->WriteTextXY(m_Screen, 130, 225 + 30*i, points);

        for (int j=0; j<6; j++)
        {
            sprintf(points, "%4d\0", m_Player[i].stats.data[j]);
            m_FontYellow->WriteTextXY(m_Screen, 220 + 75*j, 225 + 30*i, points);
        }
    }

	if (m_GameType == gtOnePlayer)
	{
		sprintf(points, "POINTS: %d\0", m_Player[0].GamePoints);
		m_FontYellow->WriteTextXY(m_Screen, 90, 260, points);
	}
}
//-----------------------------------------------------------------------------
// returns true if player made it to the top10
bool NjamEngine::CheckHiscore()
{
	if (m_GameType != gtOnePlayer)
		return false;

	int score = m_Player[0].GamePoints;
	int i=9;
	while (i > -1 && score > TopTenScores[i].points)
	{
		if (i < 9)
		{
			TopTenScores[i+1].points = TopTenScores[i].points;
			TopTenScores[i+1].level = TopTenScores[i].level;
			char *name = TopTenScores[i+1].name;
			char *name2 = TopTenScores[i].name;
			while (*name2)
				*name++ = *name2++;
			*name = '\0';
		}
		i--;
	}

	if (i<9)
	{
		TopTenScores[i+1].points = score;
		TopTenScores[i+1].level = m_CurrentMap+1;
		TopTenScores[i+1].name[0] = '\0';

		// ENTER YOUR NAME
		Uint32 white = SDL_MapRGB(m_Screen->format, 255, 255, 255);
		Uint32 t = SDL_GetTicks();
		bool CursorOn = true;

		while (true)
		{
			SDL_Rect dest;
			NjamSetRect(dest, 249, 199);
			SDL_BlitSurface(m_HiScoresImage, NULL, m_Screen, &dest);
			m_FontYellow->WriteTextXY(m_Screen, 277, 296, TopTenScores[i+1].name);

			// get len
			int l = 0;
			while (TopTenScores[i+1].name[l])
				l++;

			if (SDL_GetTicks() > t + 400)	// make the cursor blink
			{
				t = SDL_GetTicks();
				CursorOn = !CursorOn;
			}

			if (CursorOn)
			{
				NjamSetRect(dest, 279 + 10 * l, 296, 10, 15);	// draw cursor
				SDL_FillRect(m_Screen, &dest, white);
			}

			Render(m_Screen);

			SDL_Keycode key = NjamGetch(false);
			char allowed[] = "0123456789abcdefghijklmnopqrstuvwxyz. :)(!*";
			bool ok = false;
			for (int k=0; allowed[k]; k++)
				if (allowed[k] == key)
					ok = true;

			if (ok && l < 9)
			{
				char c = key;
				if (c >= 'a' && c <= 'z')
					c -= ('a' - 'A');
				TopTenScores[i+1].name[l] = c;
				TopTenScores[i+1].name[l+1] = '\0';
			}

			if (key == SDLK_BACKSPACE && l > 0)			// backspace
				TopTenScores[i+1].name[l-1] = '\0';

			if ((key == SDLK_KP_ENTER || key == SDLK_RETURN) && TopTenScores[i+1].name[0])	// entered
				break;
		}

		m_LastHiscore = i+1;
		return true;
	}

	return false;
}
//-----------------------------------------------------------------------------
