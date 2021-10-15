//-----------------------------------------------------------------------------
//  Filename:   njam.cpp
//  Created:    30. May 2003. by Milan Babuskov
//
//  Purpose:    Initialization of the game
//              Managing resources
//              Handling Main Menu
//
//  The order of functions in this file is alphabetical
//  Except for main() and NjamEngine's ctor and destructor.
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
// needed for chdir
#ifdef __linux__
/* this must be done before the first include of unistd.h for setresgid */
#define _GNU_SOURCE
#include <unistd.h>
#include <defaults.h>
#endif

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_image.h>
#include "njamutils.h"
#include "njamfont.h"
#include "njammap.h"
#include "njam.h"
#include "njamsingleton.h"

static FILE* highscore_fp;

//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    bool Fullscreen = false;
    bool SWSurface = false;
    bool UseDGA = false;
#ifdef __linux__
    gid_t realgid;

    highscore_fp = fopen("/var/lib/games/njam.hs", "r+");
    realgid = getgid();
    if (setresgid(-1, realgid, realgid) != 0)
    {
        perror("Could not drop setgid privileges.  Aborting.");
        return 1;
    }
#endif

    if (argc > 1)
    {
        for (int i=1; i<argc; i++)
        {
            bool ok = true;
            if (argv[i][0] == '-')
            {
                if (argv[i][1] == 'w')
                    Fullscreen = false; // use njam.exe -w windowed mode
                else if (argv[i][1] == 'h')
                    SWSurface = false;  // use -h for hardware surfaces
                else if (argv[i][1] == 'd')
                    UseDGA = true;
                else
                    ok = false;
            }
            else
                ok = false;

            if (!ok)
            {
                printf("Usage:  njam -[w|d|h]\n\n");
                printf("-w  start in Windowed mode (as opposed to fullscreen).\n");;
                printf("-h  use Hardware surfaces (faster, doesn't work well with all graphic cards).\n");
                printf("-d  use DGA driver on Linux, (much faster, but must run as root).\n");
                return 1;
            }
        }
    }

    // cd to game directory
#ifdef __linux__
/*  int i=0, l=0;
    while (argv[0][i])
    {
        if (argv[0][i] == '/')
            l = i;
        i++;
    }

    if (l)
    {
        argv[0][l] = '\0';
        chdir(argv[0]);
        argv[0][l] = '/';
    }*/
    char path[300];    // should be enough
    sprintf(path, "%s/%s", DEFAULT_LIBDIR, PACKAGE);
    printf("%s",path);
    chdir(path);
#endif

    NjamEngine Engine;
    if (!Engine.Init(Fullscreen, SWSurface, UseDGA))
    {
        LogFile("Failed.\n");
        return 1;
    }

    try
    {
        Engine.Start();
    }
    catch (...)
    {
        printf("Caught exception.\n");
    }
    return 0;
}
//-----------------------------------------------------------------------------
NjamEngine::NjamEngine()
{
    m_LastHiscore = -1;

    m_Skins = NULL;
    m_ScriptDelay = -350;   // when it's less than 100, the InfoWindow is empty
    script_file = NULL;
    m_ScriptFilePos = 0;
    m_ScriptLastPos = 0;
    m_ScrollOffset = 810;
    m_Screen = NULL;
    m_FontBlue = NULL;
    m_FontYellow = NULL;
    m_AudioAvailable = false;
    m_NetworkAvailable = false;
    m_ServerSocket = NULL;
    m_SDL = false;

    m_Icon = NULL;
    m_MainMenuImage = NULL;
    m_OptionsMenuImage = NULL;
    m_StatsImage = NULL;
    m_SpriteImage = NULL;
    m_HiScoresImage = NULL;
    m_LevelsetImage = NULL;
    m_NetSendImage = NULL;
    m_NetEnterImage = NULL;
    m_NetLobbyImage = NULL;
    m_GameOverImage = NULL;

    m_MainMenuMusic = NULL;
    m_GameMusic1 = NULL;
    m_GameMusic2 = NULL;

    // default values for game options (Init() overrides if njam.conf file exists)
    m_GameOptions.PlayMusic = true;
    m_GameOptions.PlaySound = true;
    m_GameOptions.UsedSkin = 0;
    m_GameOptions.ServerIP[0] = 0;
}
//-----------------------------------------------------------------------------
NjamEngine::~NjamEngine()
{
#ifdef __linux__
    char bufsdl[30];
    sprintf(bufsdl, "SDL_VIDEODRIVER=%s", linux_sdl_driver);
    putenv(bufsdl);
#endif

    if (script_file)
        fclose(script_file);

    if (m_Skins)
    {
        for (int i=0; i<m_NumberOfSkins; i++)
            SDL_FreeSurface(m_Skins[i]);
        delete[] m_Skins;
    }

    if (m_Icon)
        SDL_FreeSurface(m_Icon);
    if (m_MainMenuImage)
        SDL_FreeSurface(m_MainMenuImage);
    if (m_OptionsMenuImage)
        SDL_FreeSurface(m_OptionsMenuImage);
    if (m_GameOverImage)
        SDL_FreeSurface(m_GameOverImage);
    if (m_StatsImage)
        SDL_FreeSurface(m_StatsImage);
    if (m_SpriteImage)
        SDL_FreeSurface(m_SpriteImage);
    if (m_HiScoresImage)
        SDL_FreeSurface(m_HiScoresImage);
    if (m_LevelsetImage)
        SDL_FreeSurface(m_LevelsetImage);
    if (m_NetSendImage)
        SDL_FreeSurface(m_NetSendImage);
    if (m_NetEnterImage)
        SDL_FreeSurface(m_NetEnterImage);
    if (m_NetLobbyImage)
        SDL_FreeSurface(m_NetLobbyImage);

    if (m_FontBlue)
        delete m_FontBlue;
    if (m_FontYellow)
        delete m_FontYellow;

    for (int i=0; i<17; i++)
        if (m_Sounds[i])
            Mix_FreeChunk(m_Sounds[i]);

    if (m_MainMenuMusic)
        Mix_FreeMusic(m_MainMenuMusic);
    if (m_GameMusic1)
        Mix_FreeMusic(m_GameMusic1);
    if (m_GameMusic2)
        Mix_FreeMusic(m_GameMusic2);

    if (m_AudioAvailable)
        Mix_CloseAudio();

    if (m_NetworkAvailable)
        SDLNet_Quit();

    // write configuration options
    LogFile("Opening njam.conf file.\n");
    char *home = getenv("HOME");
    std::string njamconf("njam.conf");
    if (home)
        njamconf = std::string(home) + "/.njamconf";
    FILE *fp = fopen(njamconf.c_str(), "w+");
    if (fp)
    {
        fprintf(fp, "M=%d\n", (m_GameOptions.PlayMusic ? 1 : 0));
        fprintf(fp, "S=%d\n", (m_GameOptions.PlaySound ? 1 : 0));
        fprintf(fp, "U=%d\n", m_GameOptions.UsedSkin);
        fprintf(fp, "I=%s\n", m_GameOptions.ServerIP);
        fclose(fp);
    }

        // format: NAME#POINTS#LEVEL#
    if (highscore_fp)
    {
        rewind(highscore_fp);
        for (int i=0; i<10; i++)
            fprintf(highscore_fp, "%s#%d#%d#\n", TopTenScores[i].name, TopTenScores[i].points, TopTenScores[i].level);
        fclose(highscore_fp);
    }

    if (m_SDL)
        SDL_Quit();
}
//---------------------------------------------------------------------------
void NjamEngine::DoScript(void)     // loads script.txt file, and show data on main-menu screen
{
    if (m_ScriptDelay < -100)   // initial delay, so screen isn't too full when the game is loaded
    {
        m_ScriptDelay++;
        return;
    }

    m_ScriptDelay--;
    if (m_ScriptDelay <= 0)     // change the script
    {
        m_ScriptDelay = 400;    // delay change for 400 cycles
        m_ScriptFilePos = m_ScriptLastPos;
    }

    if (script_file == NULL)
        script_file = fopen("data/script.txt", "rb");
    if (script_file == NULL)
        return;

    fseek(script_file, m_ScriptFilePos, SEEK_SET);

    char buffer[80];
    for (int i=0; i<80; i++)    // empty buffer
        buffer[i] = 0;

    // first line of text
    int Row = 175 / m_FontBlue->GetCharHeight();
    int Column = 440 / m_FontBlue->GetCharWidth();

    while (true)
    {
        fgets(buffer, 79, script_file);
        if (feof(script_file))
        {
            printf("njam.cpp: ERROR: Script.txt not terminated with -- line.");
            m_ScriptLastPos = 0;
            break;
        }

        if (buffer[0] == '-' && buffer[1] == '-')   // -- end of script
        {
            m_ScriptLastPos = 0;
            break;
        }

        if (buffer[0] == '-')                   // end of part
        {
            m_ScriptLastPos = ftell(script_file);
            break;
        }

        if (buffer[0] == '<')                   // image
        {
            int i = (int)(buffer[2] - '0');     // image index

            if (buffer[1] == 'G')               // image of a ghost
            {
                SDL_Rect src, dest;
                NjamSetRect(dest, 440, 120);
                NjamSetRect(src, i*25,   0, 25, 25);
                SDL_BlitSurface(m_SpriteImage, &src, m_Screen, &dest);
            }
            else                                // image of a map-data
            {
                i+=2;

                for (int j=0; j<m_NumberOfSkins; j++)
                {
                    SDL_Rect src, dest;
                    NjamSetRect(dest, 440 + j*30, 120);
                    NjamSetRect(src,        i*25,   0, 25, 25);
                    SDL_BlitSurface(m_Skins[j], &src, m_Screen, &dest);
                }
            }
        }
        else    // text
        {
            m_FontBlue->WriteTextColRow(m_Screen, Column, Row, buffer);
            Row++;
        }
    }
}

#define WINDOW_TITLE "Njam     http://njam.sourceforge.net"

//-----------------------------------------------------------------------------
bool NjamEngine::Init(bool Fullscreen, bool SoftwareSurface, bool UseDGA)
{
#ifdef __linux__
    // Check if $HOME/.njam-levels exists and if it doesn't create it
    char buf[512];
    char *home = NjamGetHomeDir();
    snprintf(buf, sizeof(buf), "%s/.njam-levels", home? home:"");
    NjamCheckAndCreateDir(buf);

    sprintf(linux_sdl_driver, "x11\0");
    char *driver_name = getenv("SDL_VIDEODRIVER");
    if (driver_name)
        sprintf(linux_sdl_driver, "%s\0", driver_name);

    if (UseDGA)
    {
        printf("You must be root to run DGA\nType ./njam -? for help\n");
        putenv("SDL_VIDEODRIVER=dga");
    }
#else
    if (UseDGA)
        printf("Parameter -d is ignored for non-Linux platforms.\n");
#endif

    LogFile("Initializing SDL: VIDEO & AUDIO...", true);
    if ( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER ) < 0 )
    {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        return false;
    }
    else
        m_SDL = true;

    LogFile("OK.\nSetting window title...");
    //SDL_WM_SetCaption("Njam     http://njam.sourceforge.net", NULL);

    // load .bmp for icon
    LogFile("done.\nLoading icon...");
    m_Icon = SDL_LoadBMP("data/njamicon.bmp");
    if (!m_Icon)
    {
        LogFile("FAILED.\n");
        LogFile((const char *)SDL_GetError());
        return false;
    }
    LogFile("OK.\n");
    //SDL_WM_SetIcon(m_Icon, NULL);
	SDL_Window* window_ = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
	if (window_ == NULL) {
		return false;
	}

	render_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
	m_Screen = SDL_CreateRGBSurface(0, 800, 600, 24, 0xFF, 0xFF << 8, 0xFF << 16, 0);
	texture_ = SDL_CreateTexture(render_, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, 800, 600);
    
    // hide mouse cursor in fullscreen
    if (Fullscreen)
        SDL_ShowCursor(SDL_DISABLE);

    

    // Loading various resources...
    LogFile("Loading resources:\n");

    // load .bmp for main menu
    SDL_Surface *temp;
    LogFile("Loading Main Menu image...");
    temp = IMG_Load("data/mainmenu.jpg");
    if (!temp)
    {
        LogFile("FAILED.\n");
        LogFile((const char *)SDL_GetError());
        return false;
    }
    LogFile("OK.\n");

    // Convert image to video format
    m_MainMenuImage = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_FreeSurface(temp);
    if ( m_MainMenuImage == NULL )
    {
        fprintf(stderr, "Couldn't convert main menu image: %s\n",   SDL_GetError());
        return false;
    }

    // load .bmp for options menu
    LogFile("Loading Options Menu image...");
    temp = IMG_Load("data/options.jpg");
    if (!temp)
    {
        LogFile("FAILED.\n");
        LogFile((const char *)SDL_GetError());
        return false;
    }
    LogFile("OK.\n");

    // Convert image to video format
    m_OptionsMenuImage = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_FreeSurface(temp);
    if ( m_OptionsMenuImage == NULL )
    {
        fprintf(stderr, "Couldn't convert options menu image: %s\n", SDL_GetError());
        return false;
    }

    // load .bmp for network-send screen
    LogFile("Loading Net-send image...");
    m_NetSendImage = IMG_Load("data/netsend.jpg");
    if (!m_NetSendImage)
    {
        LogFile("FAILED.\n");
        LogFile((const char *)SDL_GetError());
        return false;
    }
    LogFile("OK.\n");

    LogFile("Loading Hi-score image...");
    m_HiScoresImage = IMG_Load("data/hiscore.jpg");
    if (!m_HiScoresImage)
    {
        LogFile("FAILED.\n");
        LogFile((const char *)SDL_GetError());
        return false;
    }
    LogFile("OK.\n");

    LogFile("Loading levelset entry image...");
    m_LevelsetImage = IMG_Load("data/levelset.jpg");
    if (!m_LevelsetImage)
    {
        LogFile("FAILED.\n");
        LogFile((const char *)SDL_GetError());
        return false;
    }
    LogFile("OK.\n");

    LogFile("Loading Gameover image...");
    m_GameOverImage = IMG_Load("data/gameover.jpg");
    if (!m_GameOverImage)
    {
        LogFile("FAILED.\n");
        LogFile((const char *)SDL_GetError());
        return false;
    }
    LogFile("OK.\n");

    // load .bmp for network-send screen
    LogFile("Loading Net-lobby image...");
    m_NetLobbyImage = SDL_LoadBMP("data/network.bmp");
    if (!m_NetLobbyImage)
    {
        LogFile("FAILED.\n");
        LogFile((const char *)SDL_GetError());
        return false;
    }
    LogFile("OK.\n");

    // load .bmp for enter server IP screen
    LogFile("Loading enter server IP image...");
    m_NetEnterImage = IMG_Load("data/enter-ip.jpg");
    if (!m_NetEnterImage)
    {
        LogFile("FAILED.\n");
        LogFile((const char *)SDL_GetError());
        return false;
    }
    LogFile("OK.\n");

    // load .bmp for stats screen
    LogFile("Loading Stats screen image...");
    temp = IMG_Load("data/stats.jpg");
    if (!temp)
    {
        LogFile("FAILED.\n");
        LogFile((const char *)SDL_GetError());
        return false;
    }
    LogFile("OK.\n");

    // Convert image to video format
    m_StatsImage = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_FreeSurface(temp);
    if ( m_StatsImage == NULL )
    {
        fprintf(stderr, "Couldn't convert stats screen image: %s\n", SDL_GetError());
        return false;
    }

    // load .bmp for sprites and menu selector
    LogFile("Loading Sprites image...");
    temp = SDL_LoadBMP("data/sprites.bmp");
    if (!temp)
    {
        LogFile("FAILED.\n");
        LogFile((const char *)SDL_GetError());
        return false;
    }

    // setting color key for sprites: black is transparent
    Uint32 black = SDL_MapRGB(temp->format, 0, 0, 0);
    SDL_SetColorKey(temp, SDL_TRUE, black);
    LogFile("OK.\n");

    // Convert image to video format
    m_SpriteImage = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_FreeSurface(temp);
    if ( m_SpriteImage == NULL )
    {
        fprintf(stderr, "Couldn't convert sprite image: %s\n", SDL_GetError());
        return false;
    }

    // loading skins, first check how many is there
    char Filename[18];
    m_NumberOfSkins = 0;
    while (true)
    {
        sprintf(Filename, "skins/Back%03d.bmp\0", m_NumberOfSkins);
        FILE *fp = fopen(Filename, "r");
        if (!fp)
            break;

        fclose(fp);
        m_NumberOfSkins++;
    }

    LogFile("Loading skins:\n");
    if (m_NumberOfSkins > 0)        // alloc memory for pointers
        m_Skins = new SDL_Surface *[m_NumberOfSkins];

    for (int i=0; i<m_NumberOfSkins; i++)
    {
        sprintf(Filename, "skins/Back%03d.bmp\0", i);
        LogFile((const char*)Filename);
        temp = SDL_LoadBMP(Filename);
        if (!temp)
        {
            LogFile("...FAILED.\n");
            LogFile((const char *)SDL_GetError());
            return false;
        }

        m_Skins[i] = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_RGBA8888, 0);
        SDL_FreeSurface(temp);
        if ( m_Skins[i] == NULL )
        {
            fprintf(stderr, "Couldn't convert skin image: %s\n", SDL_GetError());
            return false;
        }

        LogFile("...OK.\n");
    }

    LogFile("Loading Fonts:\n");
    m_FontBlue = new NjamFont("data/font-blue.bmp", 6, 9);
    m_FontYellow = new NjamFont("data/font-yellow.bmp", 10, 15);

    // Load user settings from .conf file
    LogFile("Opening njam.conf file.\n");
#ifndef __linux__
    char *home = getenv("HOME");
#endif
    std::string njamconf("njam.conf");
    if (home)
        njamconf = std::string(home) + "/.njamconf";
    FILE *fp = fopen(njamconf.c_str(), "r");
    if (fp)
    {
        LogFile("Reading njam.conf file.\n");
        char buff[20];
        while (true)
        {
            fgets(buff, 20, fp);
            if (feof(fp))
                break;

            if (buff[1] == '=')
            {
                int i;
                switch (buff[0])
                {
                    case 'M':   m_GameOptions.PlayMusic = (buff[2] == '1');     break;
                    case 'S':   m_GameOptions.PlaySound = (buff[2] == '1');     break;
                    case 'U':   m_GameOptions.UsedSkin = (buff[2] - '0');   break;
                    case 'I':   // serverIP
                        for (i=2; buff[i] != '\n' && buff[i] && i<17; i++)
                            m_GameOptions.ServerIP[i-2] = buff[i];
                        m_GameOptions.ServerIP[i] = '\0';
                        break;
                    default:
                        printf("Unknown configuration file option: %c\n", buff[0]);
                }
            }
        }
        fclose(fp);
    }
    else
        LogFile("Failed to open conf file.\n");

    // create default hiscore
    LogFile("Creating default hiscore.\n");
    char DefaultNames[10][10] = {
        "MILAN",        "TANJA",
        "DULIO",        "DJORDJE",
        "CLAUS",        "ENZO",
        "JOLAN",        "JAAP",
        "MARTIN",       "RICHARD"   };
    for (int i=0; i<10; i++)
    {
        TopTenScores[i].points = (int)(18 - 1.8*i) * 220;
        TopTenScores[i].level = 18 - (int)(1.9*i);
        sprintf(TopTenScores[i].name, "%s\0", DefaultNames[i]);
    }

    // load hiscore from file (if any)
    // format: NAME#POINTS#LEVEL#
    if (highscore_fp)
    {
        LogFile("Reading /var/games/njam.hs\n");
        char buff[40];
        int number = 0;
        while (!feof(highscore_fp) && number < 10)
        {
            fgets(buff, 40, highscore_fp);
            int i = 0, last;

            // name
            while (buff[i] != '#' && i < 9)
            {
                TopTenScores[number].name[i] = buff[i];
                i++;
            }
            TopTenScores[number].name[i] = '\0';

            i++;
            last = i;
            while (buff[i] != '#' && i < 40)
                i++;
            if (i >= 40)
                break;
            buff[i] = '\0';
            TopTenScores[number].points = atoi(buff+last);
            i++;
            last = i;
            while (buff[i] != '#' && i < 40)
                i++;
            if (i >= 40)
                break;
            buff[i] = '\0';
            TopTenScores[number].level = atoi(buff+last);

            number++;
        }
    }

    LogFile("Opening audio...");
    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 1, 2048) == -1)
    {
        m_GameOptions.PlayMusic = false;
        m_GameOptions.PlaySound = false;
        LogFile("Failed.\n");
        printf("Mix_OpenAudio: %s\n", Mix_GetError());
    }
    else
    {
        m_AudioAvailable = true;
        LogFile("OK.\n");

        printf("Reserving 4 channels for sfx...\n");
        int reserved = Mix_ReserveChannels(4);
        if(reserved != 4)
        {
            printf("Reserved %d channels from default mixing.\n",reserved);
            printf("4 channels were not reserved!\n");
        }

        printf("Loading song: data/satisfy.xm...\n");
        m_MainMenuMusic = Mix_LoadMUS("data/satisfy.xm");;
        if(!m_MainMenuMusic)
            printf("ERROR: Mix_LoadMUS(): %s\n", Mix_GetError());

        printf("Loading song: data/ritam.s3m...\n");
        m_GameMusic1 = Mix_LoadMUS("data/ritam.s3m");;
        if(!m_GameMusic1)
            printf("ERROR: Mix_LoadMUS(): %s\n", Mix_GetError());

        printf("Loading song: data/dali.xm...\n");
        m_GameMusic2 = Mix_LoadMUS("data/dali.xm");;
        if(!m_GameMusic2)
            printf("ERROR: Mix_LoadMUS(): %s\n", Mix_GetError());

        printf("Loading samples...");
        m_Sounds[0] = Mix_LoadWAV("data/dead.wav");
        m_Sounds[1] = Mix_LoadWAV("data/dead2.wav");
        m_Sounds[2] = Mix_LoadWAV("data/dead3.wav");
        m_Sounds[3] = Mix_LoadWAV("data/kill.wav");
        m_Sounds[4] = Mix_LoadWAV("data/kill2.wav");
        m_Sounds[5] = Mix_LoadWAV("data/kill3.wav");
        m_Sounds[6] = Mix_LoadWAV("data/killply.wav");
        m_Sounds[7] = Mix_LoadWAV("data/mapend.wav");
        m_Sounds[8] = Mix_LoadWAV("data/mapend2.wav");
        m_Sounds[9] = Mix_LoadWAV("data/juice.wav");
        m_Sounds[10] = Mix_LoadWAV("data/teleport.wav");
        m_Sounds[11] = Mix_LoadWAV("data/invisible.wav");
        m_Sounds[12] = Mix_LoadWAV("data/trapdoor.wav");
        m_Sounds[13] = Mix_LoadWAV("data/freeze.wav");
        m_Sounds[14] = Mix_LoadWAV("data/tripleding.wav");
        m_Sounds[15] = Mix_LoadWAV("data/bonus.wav");
        m_Sounds[16] = Mix_LoadWAV("data/50pts.wav");
        for (int i=0; i<17; i++)
            if (!m_Sounds[i])
                printf("ERROR loading sample nr. %d\n", i);
        printf("done.\n");
    }

    return true;
}
//-----------------------------------------------------------------------------
bool NjamEngine::MenuItemSelected(int& SelectedMenuItem)
{
    if (SelectedMenuItem == m_NumberOfMenuItems-1)      // last menu item = EXIT
    {
        m_ActiveMenu = mtMainMenu;                      // return to main menu...
        m_NumberOfMenuItems = 8;                        // ...which has 7 items
        SelectedMenuItem = 0;
        return true;
    }

    if (m_ActiveMenu == mtMainMenu)                     // process all items in main menu
    {
        if (SelectedMenuItem == 6)                      // "LevelEditor" selected
        {
            LevelEditor();
            return true;
        }

        if (SelectedMenuItem == 5)                      // "Options" menu item selected
        {
            m_ActiveMenu = mtOptionsMenu;
            m_NumberOfMenuItems = 4;                    // Options menu has 4 items
            SelectedMenuItem = 0;
            return true;
        }
        else                                            // some of the games is selected
        {                                               // also other menu items can get here...
            // play the game
            StartGame((GameType)SelectedMenuItem);
            if (m_NetworkAvailable)
                Disconnect();       // safe to call - checks everthing

            // play music again (if not playing already)
            if (m_AudioAvailable && m_MainMenuMusic && m_GameOptions.PlayMusic && !Mix_PlayingMusic() && Mix_PlayMusic(m_MainMenuMusic, -1)==-1)
                printf("ERROR: njam.cpp: MenuItemSelected: Mix_PlayMusic: %s\n", Mix_GetError());

            return true;
        }
    }

    if (m_ActiveMenu == mtOptionsMenu)                  // process all items in Options menu
    {
        if (SelectedMenuItem == 0)      // music on/off
        {
            m_GameOptions.PlayMusic = !m_GameOptions.PlayMusic;
            if (!m_AudioAvailable)
                m_GameOptions.PlayMusic = false;

            if (m_GameOptions.PlayMusic)
            {
                if (m_AudioAvailable && m_MainMenuMusic && Mix_PlayMusic(m_MainMenuMusic, -1)==-1)
                    printf("ERROR: njam.cpp: MenuItemSelected: Mix_PlayMusic: %s\n", Mix_GetError());
            }
            else if (m_AudioAvailable && m_MainMenuMusic && Mix_PlayingMusic())
                Mix_HaltMusic();
        }
        else if (SelectedMenuItem == 1)     // sound on/off
        {
            m_GameOptions.PlaySound = !m_GameOptions.PlaySound;
            if (!m_AudioAvailable)
                m_GameOptions.PlaySound = false;
        }
        else if (SelectedMenuItem == 2)     // skins on/off
        {
            m_GameOptions.UsedSkin++;
            if (m_GameOptions.UsedSkin > m_NumberOfSkins)
                m_GameOptions.UsedSkin = 0;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NjamEngine::ScrollText()
{
    const char scroll_text[] = "NJAM BY MILAN BABUSKOV               USE ARROW KEYS TO NAVIGATE MENU            HIT ENTER TO SELECT MENU ITEM               VISIT  NJAM.SOURCEFORGE.NET  ONLINE                NJAM IS A FREE GAME, BUT IF YOU WISH TO SUPPORT DEVELOPMENT YOU ARE WELCOME TO SEND SOME MONEY. FIVE USD OR EURO IS QUITE ENOUGH. CONTACT ME AT MBABUSKOV@YAHOO.COM FOR MORE INFO.      ";
    m_ScrollOffset-=2;

    if (m_ScrollOffset < -3600)     // end of message
        m_ScrollOffset = 820;

    m_FontYellow->WriteTextXY(m_Screen, m_ScrollOffset, 10, scroll_text);
}
//-----------------------------------------------------------------------------
void NjamEngine::Start()
{
    // play main menu .mod
    if (m_AudioAvailable && m_GameOptions.PlayMusic && Mix_PlayMusic(m_MainMenuMusic, -1) == -1)
        printf("ERROR: njam.cpp: Mix_PlayMusic: %s\n", Mix_GetError());

    int SelectedMenuItem = 0;
    m_NumberOfMenuItems = 8;    // 7 options in main menu (can be different for other menus, but last option must be: Exit or Back)
    m_ActiveMenu = mtMainMenu;  // MainMenu is active (I used enum to make code readable)

    while (true)    // loop main menu
    {
        // this screen has animations, so we want it to look the same on all machines
        Uint32 StartTime = SDL_GetTicks();

        if (0 != SDL_BlitSurface(m_MainMenuImage, NULL, m_Screen, NULL))    // draw main image
        {
            LogFile("Failed to blit main menu image.\n");
            LogFile((const char *)SDL_GetError());
            return;
        }

        // if options -- draw options
        if (m_ActiveMenu == mtOptionsMenu)
        {
            SDL_Rect src,dest;
            dest.x = 50;
            dest.y = 120;
            if (0 != SDL_BlitSurface(m_OptionsMenuImage, NULL, m_Screen, &dest))
            {
                LogFile("Failed to blit options menu image.\n");
                LogFile((const char *)SDL_GetError());
                return;
            }

            // draw boxes for selected options
            for (int i=0; i<2; i++)
            {
                bool option;
                switch (i)
                {
                    case 0:     option = m_GameOptions.PlayMusic;   break;
                    case 1:     option = m_GameOptions.PlaySound;   break;
                }

                NjamSetRect(dest, 180, 128+35*i, 45, 32);
                if (!option)
                {
                    dest.x = 230;
                    dest.w = 80;
                }
                SDL_FillRect(m_Screen, &dest, SDL_MapRGB(m_Screen->format, 255, 255, 255));
                dest.x += 2;
                dest.y += 2;
                NjamSetRect(src, dest.x - 50, dest.y - 120, dest.w - 4, dest.h - 4);
                SDL_BlitSurface(m_OptionsMenuImage, &src, m_Screen, &dest);
            }

            // draw selected skin, or random
            if (m_GameOptions.UsedSkin == 0)
            {
                NjamSetRect(dest, 110, 202, 302, 27);
                SDL_FillRect(m_Screen, &dest, SDL_MapRGB(m_Screen->format, 200, 200, 20));
                NjamSetRect(dest, 111, 203, 300, 25);
                SDL_FillRect(m_Screen, &dest, SDL_MapRGB(m_Screen->format, 0, 0, 0));
                m_FontYellow->WriteTextXY(m_Screen, 200, 208, "R A N D O M");
            }
            else
            {
                NjamSetRect(dest, 110, 202, 302, 27);
                SDL_FillRect(m_Screen, &dest, SDL_MapRGB(m_Screen->format, 0, 0, 0));
                NjamSetRect(dest, 111, 203);
                SDL_BlitSurface(m_Skins[m_GameOptions.UsedSkin - 1], NULL, m_Screen, &dest);
            }
        }

        // draw menu selector - at pos: SelectedMenuItem
        SDL_Rect rsrc, rdest;
        NjamSetRect(rsrc, 100, 0, 25, 25);
        NjamSetRect(rdest, 22, 130 + 35 * SelectedMenuItem);
        if (0 != SDL_BlitSurface(m_SpriteImage, &rsrc, m_Screen, &rdest))
        {
            LogFile("Failed to blit menu selector image.\n");
            LogFile((const char *)SDL_GetError());
            return;
        }

        DoScript();     // update info window
        ScrollText();   // update text that scrolls at the top of screen

        // draw hiscore table
        m_FontBlue->WriteTextXY(m_Screen, 714, 470, "LEVEL SCORE");
        for (int i=0; i<10; i++)
        {
            if (i == m_LastHiscore)
            {
                SDL_Rect dst;
                NjamSetRect(dst, 658, 479+i*10, 124, 11);
                SDL_FillRect(m_Screen, &dst, SDL_MapRGB(m_Screen->format, 0, 0, 100));
            }

            char text[100];
            sprintf(text, "%-11s %2d %5d\0", TopTenScores[i].name, TopTenScores[i].level, TopTenScores[i].points);
            m_FontBlue->WriteTextXY(m_Screen, 660, 480+i*10, text);
        }

        // draw a version number
        m_FontBlue->WriteTextXY(m_Screen, 92, 85, "VERSION 1.25");

        // handle a keypress
        SDL_Event event;
        while(SDL_PollEvent(&event))     // Loop until there are no events left on the queue
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    throw 2;
                case SDL_KEYDOWN:                                   // Handle a KEYDOWN event
                    if (event.key.keysym.sym==SDLK_ESCAPE)
                    {
                        if (mtMainMenu == m_ActiveMenu)             // ESC - exit game
                            return;

                        SelectedMenuItem = m_NumberOfMenuItems - 1;
                        if (!MenuItemSelected(SelectedMenuItem))    // as if last menu item is selected (= Exit from submenu)
                            return;
                    }
                    else if (event.key.keysym.sym==SDLK_UP)         // up arrow
                    {
                        if (SelectedMenuItem > 0)
                            SelectedMenuItem--;
                    }
                    else if (event.key.keysym.sym==SDLK_DOWN)       // down arrow
                    {
                        if (SelectedMenuItem < m_NumberOfMenuItems-1)
                            SelectedMenuItem++;
                    }
                    else if (event.key.keysym.sym==SDLK_RETURN || event.key.keysym.sym==SDLK_KP_ENTER)  // ENTER
                    {
                        if (SelectedMenuItem == m_NumberOfMenuItems-1 && m_ActiveMenu == mtMainMenu)        // Exit in main = exit game
                            return;

                        if (!MenuItemSelected(SelectedMenuItem))    // show options/run game/whatever, returns false if fatal error
                            return;
                    }
                    break;
            }
        }


        // About 30fps. So that it doesn't fly on fast machines...
        while (StartTime + 30 > SDL_GetTicks())
            SDL_Delay(2);

		Render(m_Screen);
    }
}
//---------------------------------------------------------------------------
