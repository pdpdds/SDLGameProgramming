
/*-------------------------------------------------------*/
/*                                                       */
/* [c]copyright 1996 by AlphaHelix                       */
/*                                                       */
/* This is the mainmodule of RAVAGE. And I wrote it.     */
/* Yes. I wrote it on my own.                            */
/*                                                       */
/*-------------------------------------------------------*/

#include <malloc.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <climits>
#include <string.h>

#define AH_MAINMODULE
#include "ERROR.HPP"
#include "MEMORY.HPP"
#include "COMPATIBILITY.HPP"
#include "SDL_BASE.HPP"

#include "INPUT.HPP"
#include "DISKIO.HPP"
#include "RAVAGE.HPP"
#include "SOUND.HPP"

#include "XLIB.HPP"

// Some variables.
const char copyright[] = "RAVAGE registered 1.1 [c]copyright 1996 by AlphaHelix.";

using namespace std;

/*---------------------------------------------------------
 Function: cmdline

 Description:
 Concatenate all command arguments to one string.
 So easy search can be done now.
---------------------------------------------------------*/
#define CMDLEN       40             // Max allowed length of command line.
static char    cmd[CMDLEN];         // Command line arguments.
int incmd(char *text)
{
   return (int)strstr(cmd, text);
}

static int cmdnumber(char *text, int def)
{
   char  *i = strstr(cmd, text);

   if (!i) return def;
   return i[strlen(text)] - '0';
}

#ifdef LINUX
static void strlwr(char *str)
{
    while(*str != '\0')
    {
        if(isupper(*str)) *str = tolower(*str);
        str++;
    }
}
#endif

static void cmdline(int argc, char *argv[])
{
   int   i;

// Concatenate all command strings together in 'cmd'.
   cmd[0] = '\0';			// Clear string.
   for (i = 1; i < argc; i++) {
      if (strlen(cmd)+strlen(argv[i]) < CMDLEN)
	 strcat(cmd, argv[i]);
   }
   strlwr(cmd);

}

/*---------------------------------------------------------
 Function: loadsettings & savesettings

 Description:
 These functions are used to save\load the configuration
 the user has choosen.
---------------------------------------------------------*/
static void loadsettings(char *file)
{
   myFile   myfile;

// Fill in settings structure with FACTORY DEFAULTS.

// Sound settings.
   settings.mod_vol = 20;
   settings.wav_vol = 35;
   settings.loop_music = 1;

// Control settings.
// mouse.
   settings.msens = M_MEDIUM;
// keyboard
   settings.key[0] = SDL_SCANCODE_LEFT;
   settings.key[1] = SDL_SCANCODE_RIGHT;
   settings.key[2] = SDL_SCANCODE_UP;
   settings.key[3] = SDL_SCANCODE_DOWN;
   settings.key[4] = SDL_SCANCODE_SPACE;

   settings.device[0] = 1;       // Player 1 input: Keyboard.
   settings.device[1] = 3;       // Player 2 input: Mouse.

// joystick
	strncpy(settings.joystickName, "No Joystick", 11);

   settings.joypadbutton[0] = -1;
   settings.joypadbutton[1] = -1;
   settings.joypadbutton[2] = -1;
   settings.joypadbutton[3] = -1;
   settings.joypadbutton[4] = -1;

// Graphix
   settings.shadows = SH_ALL;
	settings.screen_mode = 0;

// Try to load configuration file.
   if (myfile.open(file)) {
      myfile.read(&settings, sizeof(Settings));
      myfile.close();
   }
}

static void savesettings(char *file)
{
   myFile   myfile;

   if (!myfile.creat(file)) return;
   myfile.write(&settings, sizeof(Settings));
   myfile.close();
}

/*---------------------------------------------------------
 Function: powerdown

 Description:
 Function called upon exit of the program.
 Does all the needed housekeeping AFTER the
 registered exit functions (reg_exit) have been
 called.
---------------------------------------------------------*/
static void powerdown(void)
{
#ifdef DEBUG
   memory_usage();
#endif

   cout << "Object peak usage: " << object_usage() << " of "
        << OBJ_MAXOBJECTS << " used." << endl;
   cout << "Weapon peak usage: " << weapon_usage() << " of "
        << G_MAXWEAPONS << " used." << endl;
   cout << "Enemy peak usage: " << enemy_usage() << " of "
        << G_MAXENEMIES << " used." << endl;

   SDL_Quit();

   cout << endl;
   cout << "Thank you for playing RAVAGE." << endl << endl;
}


/*---------------------------------------------------------
 Function: powerup

 Description:
 Initialize the system.
 - SDL
 - memory manager
 - timer
 - sound card
 - graphics
 - inputs (keyboard, mouse, joystick)
---------------------------------------------------------*/
static void powerup(void)
{
    SDL_version sVersion;

	// Init SDL

	if (SDL_Init(SDL_INIT_FLAGS) != 0)
	{
		fprintf(stderr, "\nUnable to initialize SDL:  %s\n", SDL_GetError());
		return;
	}

	SDL_GetVersion(&sVersion);

	printf("SDL linked %d.%d.%d\n", sVersion.major, sVersion.minor, sVersion.patch);


	atexit(powerdown);

// Grow heap.
   {
		char *ptr;
      ptr = new char[MEMNEEDED*1024];
      if (ptr == 0) error("Not enough memory available.");
      delete []ptr;
   }

// Init File Sytem.
   initfilesystem(); reg_exit(shutfilesystem);

// Load configuration.
	loadsettings(CONFIGFILE);

	if (incmd("/j")) strncpy(settings.joystickName, "No Joystick", 11); // Reset the default joystick

// Initialize Inputs.
   initinputs(INPUT_FLAGS);

   reg_exit(shutinputs);
   if (!lockdevice(settings.device[0])) {
      settings.device[0] = nextdevice(0);
      lockdevice(settings.device[0]);
   }
   if (!lockdevice(settings.device[1])) {
      settings.device[1] = nextdevice(0);
      lockdevice(settings.device[1]);
   }

// Define keys according to loaded settings.
   setkeys(settings.key[0], settings.key[1],
            settings.key[2], settings.key[3], settings.key[4]);

	setbuttons(settings.joypadbutton[0], settings.joypadbutton[1],
            settings.joypadbutton[2], settings.joypadbutton[3], settings.joypadbutton[4]);

   if (joypad) cout << "Joypad detected." << endl;
   if (mouse) cout << "Mouse detected." << endl;

// Init Sound system.
   initsound(); reg_exit(shutsound);
   s_setmodvol(settings.mod_vol);
   s_setwavvol(settings.wav_vol);

	// Enter graphics.
   initgraphics(settings.screen_mode); reg_exit(shutgraphics);

// Init OBJECT system.
   initobjects(); reg_exit(shutobjects);
   shadow_level(settings.shadows);
// Init game classes.
   initgclass(); reg_exit(shutgclass);


// No game is in progress.
   gstate.ready = 0;

// Process command line.
// First (DJ)Ammann cheats.
   cheatlevel = 0;
   if (incmd("/at")) cheatlevel |= CHEAT_INVUL;   // absolut toedlich.
   if (incmd("/vv")) cheatlevel |= CHEAT_MONEY;   // vertammi vertori.
// Level jumping.
   start_stage = cmdnumber("/l", 1);
	
   if (start_stage > MAXLEVELS) error("/l parameter out of range.");

#ifdef RECORDER
   recorder = 0;
   if (incmd("/rec")) recorder = 1;
#endif

}

int main(int argc, char *argv[])
{
   int   request;          // Returned request from MAIN MENU.



// Display some message.
   cout << endl << copyright << endl << endl;

// Prepare to access the command line.
   cmdline(argc, argv);

// Start the system.
   powerup();

// Open FilePool.
   filepool[0].open("RAV0.DAT");
   filepool[1].open("RAV1.DAT");
   filepool[2].open("RAV2.DAT");
   filepool[3].open("RAV3.DAT");

	//Load palette
	void *p = (void *)loadfile("demo1\\DEMO1.CPA", NULL);
	palette.set(p);
	delete p;

// Show TITLE screen.
   title();

// Play loop.
   request = M_QUIT;
   do {
      switch (request) {
      case M_PLAY:
// Enable RECORDER if needed.
         #ifdef RECORDER
            if (recorder) rec_enable();
         #endif
         play(0);    // Play NEW game.
         break;
      case M_LOAD:
         play(1);    // Load game.
         break;
      case M_SOUND:  // Reinitialize Sound System.
         s_setmodvol(settings.mod_vol);
         s_setwavvol(settings.wav_vol);
         break;
      }
      if (request == M_SOUND) {
         request = menu(SOUNDMENU, 0);
      } else {
         request = menu(MAINMENU, 0);
      }
   } while (request != M_QUIT);

// Close FilePool.
   filepool[3].close();
   filepool[2].close();
   filepool[1].close();
   filepool[0].close();

   end_ravage();
   return 0;
}

int end_ravage(void)
{
   sMap[0].destroy();
   sMap[1].destroy();
	
	destroy_SpritePools();

    // Done for the game. Back to OS.
   reg_down();                // Execute registered shutdown functions.
   savesettings(CONFIGFILE);  // Save current game settings.

   exit(0);                   // Exit nicely.
   return 0;
}
