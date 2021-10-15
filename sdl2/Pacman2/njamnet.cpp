//-----------------------------------------------------------------------------
//	Filename:	njamnet.cpp
//	Created:	05. Jun 2003. by Milan Babuskov
//
//  Purpose:	Manage all networking stuff
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
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_net.h>
#include "njamutils.h"
#include "njamfont.h"
#include "njam.h"
#include "njamsingleton.h"
//-----------------------------------------------------------------------------
void NjamEngine::Disconnect()
{
   	if (m_PeerSocket)
    	SDLNet_TCP_Close(m_PeerSocket);
   	if (m_GameType == gtHostDuel && m_ServerSocket)
    	SDLNet_TCP_Close(m_ServerSocket);
    m_ServerSocket = NULL;
    m_PeerSocket = NULL;
}
//-----------------------------------------------------------------------------
// return false is game is canceled or network error occurs
bool NjamEngine::SetupNetwork()
{
	if(!m_NetworkAvailable)		// init SDL networking if needed
	{
		if (SDLNet_Init() == -1)
		{
		    printf("SDLNet_Init: %s\n", SDLNet_GetError());
		    return false;
		}
		else
			m_NetworkAvailable = true;
	}
	else
	 	Disconnect();			// clean up if there is anything left from previous sessions

	if (m_GameType == gtHostDuel)
	{
		IPaddress ip;
		if(SDLNet_ResolveHost(&ip,NULL,5547) == -1)		// create a listening TCP socket (server)
		{
		    printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		    return false;
		}

		m_ServerSocket = SDLNet_TCP_Open(&ip);
		if(!m_ServerSocket)
		{
		    printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		    return false;
		}
	}

	m_PeerSocket = NULL;
	if (m_GameType == gtJoinDuel)
	{
		bool HasTried = false;
		while (!m_PeerSocket)
		{
			if (!EnterServerIP(HasTried))	// or exit
				return false;

            HasTried = true;

			// connect to localhost using TCP (client)
			IPaddress ip;
			LogFile("CLIENT: Resolving server name...\n");
			if(SDLNet_ResolveHost(&ip, m_GameOptions.ServerIP, 5547) == -1)
			{
			    printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
			    continue;
			}

			LogFile("CLIENT: Connecting to server...\n");
			m_PeerSocket = SDLNet_TCP_Open(&ip);
			if(!m_PeerSocket)
			{
			    printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
				LogFile("CLIENT: Failed.\n");
			    continue;
			}
		}
		LogFile("CLIENT: Connected.\n");
 	}

	while (true)														// show network connect screen
	{
		if (m_GameType == gtHostDuel && !m_PeerSocket)					// incoming connections
		{
			m_PeerSocket = SDLNet_TCP_Accept(m_ServerSocket);			// accept a new connection coming in on Server Socket
			if (m_PeerSocket)
			{
				IPaddress *ip = SDLNet_TCP_GetPeerAddress(m_PeerSocket);
				if(!ip)
				{
				    printf("SDLNet_TCP_GetPeerAddress: %s\n", SDLNet_GetError());
				    printf("This may be a server socket.\n");
				    m_PeerSocket = NULL;
				}
				else
					printf("Client connected!\n");
			}
		}

		RenderNetworkScreen();

		int player;							// declared outside because of switch glitch ;)
		bool Ready = false;
		SDL_Keycode key = NjamGetch(false);
		switch (key)
		{
			case SDLK_r:					// select players (each player has to press any key) to toggle
			case SDLK_d:
			case SDLK_f:
			case SDLK_g:
				player = (m_GameType == gtHostDuel ? 1 : 3);
				m_Player[player].Playing = !m_Player[player].Playing;
				break;

			case SDLK_UP:
			case SDLK_DOWN:
			case SDLK_LEFT:
			case SDLK_RIGHT:
				player = (m_GameType == gtHostDuel ? 0 : 2);
				m_Player[player].Playing = !m_Player[player].Playing;
				break;

		    case SDLK_ESCAPE:
		    	Disconnect();
		    	return false;

			case SDLK_SPACE:		// SPACE to start the game, ESC to exit
				if (m_GameType == gtHostDuel && (m_Player[0].Playing || m_Player[1].Playing)
				||	m_GameType == gtJoinDuel && (m_Player[2].Playing || m_Player[3].Playing))
					Ready = true;
		};

		if (Ready)	// server starts the game?
		{
			if (!m_PeerSocket)
				printf("Cannot start game without client");
			else
				break;	// let's go
		}
	}

	LogFile("Checking if peer is there?\n");
	if (!m_PeerSocket)	// disconnected (just in case)
	{
		Disconnect();
		return false;
	}

	LogFile("Sending CID\n");
	Uint8 data[5];						// send Connection Initialization Data (CID)
	data[0] = 'N';
	data[1] = 'J';
	int player = (m_GameType == gtHostDuel ? 0 : 2);
	data[2] = (Uint8)(m_Player[0+player].Playing ? 1 : 0);
	data[3] = (Uint8)(m_Player[1+player].Playing ? 1 : 0);
	if (4 > SDLNet_TCP_Send(m_PeerSocket, data, 4))
	{
	    printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
		Disconnect();
    	return false;
	}

	RenderNetworkScreen(true);									// show: waiting...

	LogFile("Waiting for peer CID\n");							// receive CID
	int result = SDLNet_TCP_Recv(m_PeerSocket, data, 4);
	if (result < 4)
	{
	    printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		Disconnect();
    	return false;
	}
	else
	{
		LogFile("Received some data, reading\n");
		if (data[0] == 'N')
		{
			LogFile("Data seems to be ok.\n");
			int player = (m_GameType == gtHostDuel ? 2 : 0);
			m_Player[0+player].Playing = data[2];
			m_Player[1+player].Playing = data[3];
		}
		else
		{
			LogFile("Bad data received.\n");
			Disconnect();
	    	return false;
		}
	}

	LogFile("ALL ok. Showing who is playing...\n");
	RenderNetworkScreen(true);

	SDL_Delay(1000);	// just to be visible that other players are playing
	return true;		// all ok, start the game
}
//-----------------------------------------------------------------------------
void NjamEngine::RenderNetworkScreen(bool GameStarting)
{
	SDL_BlitSurface(m_NetLobbyImage, NULL, m_Screen, NULL);
 	m_NetLobbyImage;

	SDL_Rect src, dest;
	NjamSetRect(src, 100, 0, 25, 25);
	if (m_GameType == gtHostDuel && m_ServerSocket)
	{
		NjamSetRect(dest, 103, 143);
		SDL_BlitSurface(m_SpriteImage, &src, m_Screen, &dest);
	}

	if (m_GameType == gtJoinDuel)
	{
		SDL_Rect r;
		NjamSetRect(r, 99, 497, 430, 40);
		NjamSetRect(dest, 99, 137);
		SDL_BlitSurface(m_NetLobbyImage, &r, m_Screen, &dest);
	}

	if (GameStarting)
	{
		NjamSetRect(dest, 103, 464);
		SDL_BlitSurface(m_SpriteImage, &src, m_Screen, &dest);
	}

	if (m_PeerSocket)
	{
		NjamSetRect(dest, 103, 422);
		SDL_BlitSurface(m_SpriteImage, &src, m_Screen, &dest);
	}
	else
	{
		SDL_Rect r;
		NjamSetRect(r, 99, 497, 430, 40);
		NjamSetRect(dest, 99, 457);
		SDL_BlitSurface(m_NetLobbyImage, &r, m_Screen, &dest);
	}

	for (int i=0; i<4; i++)
	{
		if (m_Player[i].Playing)
		{
			NjamSetRect(dest, 103, 215 + i * 30);
			src.x = 75 + i * 25;
			SDL_BlitSurface(m_SpriteImage, &src, m_Screen, &dest);
		}
	}
	Render(m_Screen);
}
//-----------------------------------------------------------------------------
// show entry dialog, and accept server address in m_GameOptions.ServerIP
bool NjamEngine::EnterServerIP(bool HasTried)
{
	if (HasTried)
	{
		SDL_Rect dest;
		NjamSetRect(dest, 249, 199);
		SDL_BlitSurface(m_NetEnterImage, NULL, m_Screen, &dest);
		m_FontYellow->WriteTextXY(m_Screen, 273, 295, "CONNECTION FAILED.");
		Render(m_Screen);
		SDL_Delay(2000);
	}

 	Uint32 white = SDL_MapRGB(m_Screen->format, 255, 255, 255);

	Uint32 t = SDL_GetTicks();
	bool CursorOn = true;

	while (true)
	{
		SDL_Rect dest;
		NjamSetRect(dest, 249, 199);
		SDL_BlitSurface(m_NetEnterImage, NULL, m_Screen, &dest);
		m_FontYellow->WriteTextXY(m_Screen, 273, 295, m_GameOptions.ServerIP);

		// get len
		int l = 0;
		while (m_GameOptions.ServerIP[l])
			l++;

		if (SDL_GetTicks() > t + 400)	// make the cursor blink
		{
			t = SDL_GetTicks();
			CursorOn = !CursorOn;
		}

		if (CursorOn)
		{
			NjamSetRect(dest, 275 + 10 * l, 295, 10, 15);	// draw cursor
	        SDL_FillRect(m_Screen, &dest, white);
	 	}

		Render(m_Screen);

		SDL_Keycode key = NjamGetch(false);
		if (key >= '0' && key <= '9' || key == '.')
		{
			if (l < 15)
			{
				m_GameOptions.ServerIP[l] = key;
				m_GameOptions.ServerIP[l+1] = '\0';
			}
		}

		if (key == SDLK_BACKSPACE && l > 0)			// backspace
			m_GameOptions.ServerIP[l-1] = '\0';

		if ((key == SDLK_KP_ENTER || key == SDLK_RETURN) && m_GameOptions.ServerIP[0])	// enter
		{
			SDL_Rect dest;
			NjamSetRect(dest, 249, 199);
			SDL_BlitSurface(m_NetEnterImage, NULL, m_Screen, &dest);
			m_FontYellow->WriteTextXY(m_Screen, 273, 295, "CONNECTING...");
			Render(m_Screen);
			return true;
		}

		if (key == SDLK_ESCAPE)
			return false;
	}
}
//-----------------------------------------------------------------------------
bool NjamEngine::NetworkMap()
{
 	LogFile("Exchanging map data.\n");

 	if (!m_PeerSocket)
 	{
		Disconnect();
 		return false;
 	}

	// draw previos map stats again for background (if doublebuffer is used)
	SDL_FillRect(m_Screen, NULL, 0);
	DrawGameStats();

	int size = MAPH*MAPW;
	Uint8 *buffer = m_Maps.GetTiles();
 	if (m_GameType == gtHostDuel)		// send map data
 	{
 		LogFile("SERVER: sending map data\n");
		if (size > SDLNet_TCP_Send(m_PeerSocket, buffer, size))
		{
		    printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
			Disconnect();
	    	return false;
		}

		// Render SENDING DATA and wait for ACK (this is because otherwise the server would start READY, SET, GO... earlier)
		SDL_Rect dest;
		NjamSetRect(dest, 278, 216);
		SDL_BlitSurface(m_NetSendImage, NULL, m_Screen, &dest);
		Render(m_Screen);

		Uint8 c;
		if (SDLNet_TCP_Recv(m_PeerSocket, &c, 1) < 1 || c != 'A')
		{
		    printf("SDLNet_TCP_Recv: %s\n", SDLNet_GetError());
			Disconnect();
	    	return false;
		}
 	}
	else								// recv map data
	{
		SDL_Rect dest;
		NjamSetRect(dest, 278, 216);
		SDL_BlitSurface(m_NetSendImage, NULL, m_Screen, &dest);
		Render(m_Screen);

 		LogFile("CLIENT: receiving map data\n");
		int result = SDLNet_TCP_Recv(m_PeerSocket, buffer, size);
		if (result < size)
		{
		    printf("SDLNet_TCP_Recv: %s\n", SDLNet_GetError());
			Disconnect();
	    	return false;
		}
		else
		{
			LogFile("Received map data\n");

			Uint8 c = 'A';
			if (1 > SDLNet_TCP_Send(m_PeerSocket, &c, 1))	// Send ACK
			{
			    printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
				Disconnect();
		    	return false;
			}
		}
	}

	return true;
}
//-----------------------------------------------------------------------------
// Packet:
// pos            len data
//   0              1 char 'N' make sure the packet is ok
//   1              1 Type of packet: 0 = game packet, 1 = exit game packet
//   2              7 Player1 data (rotate, x, y, xo, yo, vx, vy)
//   9              7 Player2 data
//  16   GHOSTMAX * 5 Ghost data (x , y, xo, yo, delay)
bool NjamEngine::ExchangeNetwork()
{
	m_NetworkEscapedGame = false;

 	if (m_GameType != gtHostDuel && m_GameType != gtJoinDuel)
 		return true;

 	int ghostsize = 5;
 	Uint8 buffer[16 + GHOSTMAX * 5];
 	int size = 16 + GHOSTMAX * ghostsize;
	buffer[0] = 'N';
	buffer[1] = 0;

	if (m_GameType == gtHostDuel)
	{
		// stream data to buffer
		for (int i=0; i<2; i++)
		{
			buffer[2+i*7] = m_Sprite[i].rotate;
			buffer[3+i*7] = m_Sprite[i].x;
			buffer[4+i*7] = m_Sprite[i].y;
			buffer[5+i*7] = m_Sprite[i].xo;
			buffer[6+i*7] = m_Sprite[i].yo;
			buffer[7+i*7] = 2 + m_Sprite[i].vx;	// to eliminate negative numbers
			buffer[8+i*7] = 2 + m_Sprite[i].vy;
		}
		for (int i=0; i<GHOSTMAX; i++)
		{
			buffer[i*ghostsize + 16] = m_Sprite[i+4].x;
			buffer[i*ghostsize + 17] = m_Sprite[i+4].y;
			buffer[i*ghostsize + 18] = m_Sprite[i+4].xo;
			buffer[i*ghostsize + 19] = m_Sprite[i+4].yo;
			buffer[i*ghostsize + 20] = m_Sprite[i+4].delay;
		}

		// send
		if (size > SDLNet_TCP_Send(m_PeerSocket, buffer, size))
		{
		    printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
			Disconnect();
	    	return false;
		}

		// wait for reply (must have the same counter!)
		if (SDLNet_TCP_Recv(m_PeerSocket, buffer, 16) < 16)
		{
		    printf("SDLNet_TCP_Recv: %s\n", SDLNet_GetError());
			Disconnect();
	    	return false;
		}

		if (buffer[1] == 1)		// client signaled the end of map
		{
			m_NetworkEscapedGame = true;
        	return true;
  		}

		// process data
		for (int i=0; i<2; i++)
		{
			m_Sprite[i+2].rotate   = buffer[2+i*7];
			m_Sprite[i+2].x        = buffer[3+i*7];
			m_Sprite[i+2].y        = buffer[4+i*7];
			m_Sprite[i+2].xo       = buffer[5+i*7];
			m_Sprite[i+2].yo       = buffer[6+i*7];
			m_Sprite[i+2].vx = (int)(buffer[7+i*7]) - 2;
			m_Sprite[i+2].vy = (int)(buffer[8+i*7]) - 2;
		}
	}
	else
	{
		// wait for data
		if (SDLNet_TCP_Recv(m_PeerSocket, buffer, size) < size)
		{
		    printf("SDLNet_TCP_Recv: %s\n", SDLNet_GetError());
			Disconnect();
	    	return false;
		}

		if (buffer[1] == 1)		// server signaled the end of map
		{
			m_NetworkEscapedGame = true;
        	return true;
  		}

		// process data
		for (int i=0; i<2; i++)
		{
			m_Sprite[i].rotate   = buffer[2+i*7];
			m_Sprite[i].x        = buffer[3+i*7];
			m_Sprite[i].y        = buffer[4+i*7];
			m_Sprite[i].xo       = buffer[5+i*7];
			m_Sprite[i].yo       = buffer[6+i*7];
			m_Sprite[i].vx = (int)(buffer[7+i*7]) - 2;
			m_Sprite[i].vy = (int)(buffer[8+i*7]) - 2;
		}

		for (int i=0; i<GHOSTMAX; i++)
		{
			m_Sprite[i+4].x     = buffer[i*ghostsize + 16];
			m_Sprite[i+4].y     = buffer[i*ghostsize + 17];
			m_Sprite[i+4].xo    = buffer[i*ghostsize + 18];
			m_Sprite[i+4].yo    = buffer[i*ghostsize + 19];
			m_Sprite[i+4].delay = buffer[i*ghostsize + 20];
		}

		// send reply - publish local players
		for (int i=0; i<2; i++)
		{
			buffer[2+i*7] = m_Sprite[i+2].rotate;
			buffer[3+i*7] = m_Sprite[i+2].x;
			buffer[4+i*7] = m_Sprite[i+2].y;
			buffer[5+i*7] = m_Sprite[i+2].xo;
			buffer[6+i*7] = m_Sprite[i+2].yo;
			buffer[7+i*7] = 2 + m_Sprite[i+2].vx;	// to eliminate negative numbers
			buffer[8+i*7] = 2 + m_Sprite[i+2].vy;
		}

		if (16 > SDLNet_TCP_Send(m_PeerSocket, buffer, 16))
		{
		    printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
			Disconnect();
	    	return false;
		}
	}

	return true;
}
//-----------------------------------------------------------------------------

