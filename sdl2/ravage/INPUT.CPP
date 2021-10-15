
/*-------------------------------------------------------*/
/*  Input Manager                                        */
/*  [c]copyright 1995 by AlphaHelix                      */
/*                                                       */
/*                                                       */
/*-------------------------------------------------------*/

#include <stdio.h>

#include "SDL_BASE.HPP"
#include "ERROR.HPP"
#define AH_INPUTMANAGER
#include "INPUT.HPP"
#include "RAVAGE.HPP"

// Globaler Schrott

#define MAXINPUTS          4     // Number of input devices supported.

int quit_requested = 0; //Try to quit game if true.

#ifdef PANDORA
static SDL_Cursor *fakeCursor = NULL;
#endif // PANDORA

// Next struct is a REGISTER structure for all input devices available.
// A device must be registered in this structure to get active.
// I know the idea is good but it's not implemented strict enough to earn
// the title CLEAN PROGRAMMING.
struct Device {
   int      locked;              // Device has been locked.
   char     *name;               // Device's name.
   void     (*getmotion)(int &, int &, int &);
};


//-------------------------------------------------------------------
// Keyboard interface routines.
//-------------------------------------------------------------------
// Private data.
// Defined movement keys.
static int  key_left = SDL_SCANCODE_LEFT;
static int  key_right = SDL_SCANCODE_RIGHT;
static int  key_up = SDL_SCANCODE_UP;
static int  key_down = SDL_SCANCODE_DOWN;
static int  key_fire = SDL_SCANCODE_SPACE;

/*---------------------------------------------------------
 Function: keyboard_wait

 Description:
  Waits until all keyboard keys have been released.
---------------------------------------------------------*/
void keyboard_wait(void)
{
	int x, y, fire;

	do
	{
		input_update();
		getkeyboard(x, y, fire);
	}
	while(x || y || fire || key[KEY_ESC]);
}

void escape_wait(void)
{
	do
	{
		input_update();
	}
	while(key[KEY_ESC]);
}

void getkeyboard(int &x, int &y, int &fire)
{
	const Uint8 *keyState = SDL_GetKeyboardState(NULL);

   x = 0; y = 0; fire = 0;

   if (keyState[key_left]) x--;
   if (keyState[key_right]) x++;
   if (keyState[key_up]) y--;
   if (keyState[key_down]) y++;
   if (keyState[key_fire]) fire = 1;
}

void setkeys(int left, int right, int up, int down, int fire)
{
   key_left = left;
   key_right = right;
   key_up = up;
   key_down = down;
   key_fire = fire;
}

static void initkeyboard(void)
{
   keyboard = 1;     // Indicate "keyboard present".
}

static void shutkeyboard(void)
{
   keyboard = 0;
}


//-------------------------------------------------------------------
// Joystick interface routines.
//-------------------------------------------------------------------

volatile int button_released;

static int  joy_left = -1;
static int  joy_right = -1;
static int  joy_up = -1;
static int  joy_down = -1;
static int  joy_fire = -1;

SDL_Joystick* joystick = NULL;
bool joystickFire = false; //Is the fire button pressed for the joystick
bool joystickLeft = false;
bool joystickRight = false;
bool joystickUp = false;
bool joystickDown = false;

static   int   xAxis0 = -32768/4, yAxis0 = -32767/4, xAxis1 = 32767/4, yAxis1 = 32767/4;         // Joystick corner values.

void setbuttons(int left, int right, int up, int down, int fire)
{
   joy_left = left;
   joy_right = right;
   joy_up = up;
   joy_down = down;
   joy_fire = fire;
}

static int measure(int &x, int &y, int &fire)
{
// Start new measure.
	x = SDL_JoystickGetAxis(joystick, 0);
	y = SDL_JoystickGetAxis(joystick, 1);

	if(joystickUp) y+=yAxis1 + 1;
	if(joystickDown) y+=yAxis0 - 1;
	if(joystickLeft) x+=xAxis0 - 1;
	if(joystickRight) x+=xAxis1 + 1;

// Check FIRE button.
   fire = (int)joystickFire;

   return 0;

}

/*---------------------------------------------------------
 Function: joypad_wait

 Description:
 Waits until the joypad fire button has been released.
---------------------------------------------------------*/
void joypad_wait(void)
{
   int   x, y, fire;

   do {
		input_update();
      measure(x, y, fire);
   } while (fire);
}


void getjoypad(int &x, int &y, int &fire)
{
   int   dx, dy;

   x = y = 0;
   measure(dx, dy, fire);

   if (dx < xAxis0) x--;
   if (dy < yAxis0) y--;
   if (dx > xAxis1) x++;
   if (dy > yAxis1) y++;
}


int calibrate_joypad(void)
{
   int   x, y, fire;
   int   yes;

   yes = measure(x, y, fire);
   xAxis0 = x - (x / 2);
   xAxis1 = x + (x / 2);
   yAxis0 = y - (y / 2);
   yAxis1 = y + (y / 2);
   return yes;
}

int detect_joypad(int numJoy)
{
	SDL_Event event;
	Uint32 startTime = 0;
	const int detectionTime = 4000;
	
	SDL_Joystick **checkJoystick = NULL;
	int i = 0;
	int chosen = -1;

	if(numJoy <= 0) return chosen;

	checkJoystick = (SDL_Joystick **)malloc(sizeof(SDL_Joystick *) * numJoy);

	for(i = 0; i < numJoy; i++)
	{
		*(checkJoystick + i) = SDL_JoystickOpen(i);
		SDL_JoystickEventState(SDL_ENABLE);

		if (checkJoystick) {
			printf("Opened Joystick %d\n", i);
			printf("Name: %s\n", SDL_JoystickNameForIndex(i));
			printf("Number of Axes: %d\n", SDL_JoystickNumAxes(*(checkJoystick + i)));
			printf("Number of Buttons: %d\n", SDL_JoystickNumButtons(*(checkJoystick + i)));
			printf("Number of Balls: %d\n", SDL_JoystickNumBalls(*(checkJoystick + i)));

		} else {
			printf("Couldn't open Joystick %d\n", i);
		}

		if(!SDL_strcmp(SDL_JoystickNameForIndex(i), settings.joystickName))
		{
			return i;
		}
	}

	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, 
                         "Joystick detection",
                         "Multiple joysticks detected. Please press a button from the joystick you wish to use. (within the next 4 seconds after pressing OK)",
                         NULL);

	startTime = SDL_GetTicks();

	while(((SDL_GetTicks() - startTime) < detectionTime) && chosen == -1)
	{
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_JOYBUTTONDOWN:
				case SDL_JOYBUTTONUP:

				chosen = event.jbutton.which;

				break;

				default:
				break;
			}
		}
	}

	for(i = 0; i < numJoy; i++)
	{
		SDL_JoystickClose(*(checkJoystick + i));
	}

	return chosen;
}


void initjoypad(void)
{
	int num = 0;
	int i = 0;
	int chosen = 0;

	printf("Joypads detected %d\n", SDL_NumJoysticks());

	if ((num = SDL_NumJoysticks()) > 0) {

		if(num > 1) chosen = detect_joypad(num);
		
#ifdef PANDORA
		if(!SDL_strcmp(SDL_JoystickNameForIndex(chosen), "touchscreen"))
		{
			printf("Touchscreen is ignored as joystick.\n");
			joypad = 0;
			return;
		}
#endif

		joystick = SDL_JoystickOpen(chosen);

		if(joystick) 
		{
			joypad = 1;
			printf("Using joystick: %s\n", SDL_JoystickNameForIndex(chosen));
			strncpy(settings.joystickName, SDL_JoystickNameForIndex(chosen), 254);
		}
		else printf("Couldn't open Joystick %d\n", chosen);
	}

}

void shutjoypad(void)
{
	if (SDL_JoystickGetAttached(joystick)) {
		SDL_JoystickClose(joystick);

		joypad = 0;
	}
}


//-------------------------------------------------------------------
// Mouse interface routines.
//-------------------------------------------------------------------

static int  m_sensx;          // Mouse sensitivity x direction.
static int  m_sensy;          // Mouse sensitivity y direction.
static int  m_sens;           // Mouse sensitivity level. 0=low, 2=high.
static int m_limx0, m_limx1, m_limy0, m_limy1; //Current mouse limits

/*---------------------------------------------------------
 Function: getmouse

 Description:
 Get mouse button state and mouse position.
---------------------------------------------------------*/
void getmousexy(int &b, int &x, int &y)
{
   Uint32 buttonState = SDL_GetMouseState(&x, &y);

   b = buttonState & SDL_BUTTON(1); //Check if left mouse button is pressed.

   //Fix pointer to screen ratio
   x = (int)(x * render.getXRatio()) - render.getViewportX() - BORDER; //Also adjust for BORDER

   y = (int)(y * render.getYRatio()) - render.getViewportY();

   //Constrain mouse position
   if(x < m_limx0) x = m_limx0;
   else if(x > m_limx1) x = m_limx1;
   if(y < m_limy0) y = m_limy0;
   else if (y > m_limy1) y = m_limy1;

}

// Return the net mouse displacement since the last call to this function.
void getmousemotion(int &dx, int &dy)
{
	SDL_GetRelativeMouseState(&dx, &dy);
}



/*---------------------------------------------------------
 Function: setmousexy

 Description:
 Set mouse position
---------------------------------------------------------*/
void setmousexy(int x, int y)
{
	SDL_WarpMouseInWindow(NULL, x, y); //Warp mouse in the window of current mouse focus.
}

void setmouselimits(int x0, int y0, int x1, int y1)
{

// Set horizontal limits first.
   m_limx0 = x0;
   m_limx1 = x1;

// Go for the vertical limits now.
	m_limy0 = y0;
	m_limy1 = y1;
}


/*---------------------------------------------------------
 Function: mouse_wait

 Description:
 Waits until all mouse buttons have been released.
---------------------------------------------------------*/
void mouse_wait(void)
{
   int   x, y, b;

   getmousexy(b, x, y);
   while (b)
	{
		input_update();
		getmousexy(b, x, y);
	}
}

void setmousesens(int sens)
{
   switch (sens) {
   case M_LOW:
      m_sensx = 2;
      m_sensy = 3;
      break;
   case M_MEDIUM:
      m_sensx = 1;
      m_sensy = 2;
      break;
   case M_HIGH:
      m_sensx = 0;
      m_sensy = 1;
   }
   m_sens = sens;
}

int getmousesens(void)
{
   return m_sens;
}

void getmouse(int &x, int &y, int &fire)
{
   int   dx, dy;

   x = y = 0;
   getmousexy(fire, dx, dy);
   getmousemotion(dx, dy);

   if (dx > m_sensx) x++;
   if (dx < -m_sensx) x--;
   if (dy > m_sensy) y++;
   if (dy < -m_sensy) y--;
}


void initmouse(void)
{
	mouse = 1;
	setmousesens(M_MEDIUM);

	#ifdef PANDORA
	Uint8 noData = 0;
	fakeCursor = SDL_CreateCursor(&noData, &noData, 8, 1, 0, 0);
	SDL_ShowCursor(SDL_ENABLE);
	SDL_SetCursor(fakeCursor);
	#else
	//SDL_ShowCursor(SDL_DISABLE);
	#endif
}

void shutmouse(void)
{
   if (mouse) {
      mouse = 0;

      #ifdef PANDORA
      SDL_FreeCursor(fakeCursor);
      #endif // PANDORA
   }
}


//-------------------------------------------------------------------
// Input Manager code & data.
//-------------------------------------------------------------------

static int     ndevices;               // Number of devices available.
static Device  device[MAXINPUTS];      // Device description.

void input_update(void)
{
	SDL_Event event;

	keys = 0;
	button_released = -1;

	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
            case SDL_QUIT:
                quit_requested = 1;
            break;

			case SDL_JOYBUTTONDOWN:

			if(event.jbutton.button == joy_left) joystickLeft = true;
			else if(event.jbutton.button == joy_right) joystickRight = true;
			else if(event.jbutton.button == joy_up) joystickUp = true;
			else if(event.jbutton.button == joy_down) joystickDown = true;
			else if(event.jbutton.button == joy_fire) joystickFire = true;

			if(joy_fire == -1) joystickFire = true;

			break;

			case SDL_JOYBUTTONUP:

			button_released = event.jbutton.button;

			if(event.jbutton.button == joy_left) joystickLeft = false;
			else if(event.jbutton.button == joy_right) joystickRight = false;
			else if(event.jbutton.button == joy_up) joystickUp = false;
			else if(event.jbutton.button == joy_down) joystickDown = false;
			else if(event.jbutton.button == joy_fire) joystickFire = false;

			if(joy_fire == -1) joystickFire = false;

			break;

			case SDL_MOUSEBUTTONDOWN:
			break;

			case SDL_MOUSEBUTTONUP:
			break;

			case SDL_KEYDOWN:
				keys++;

				key_pressed = (int)event.key.keysym.scancode;

				switch(event.key.keysym.scancode)
				{
					case SDL_SCANCODE_ESCAPE:
						key[KEY_ESC] = 1;
					break;

					default:
                    break;
				}

			break;

			case SDL_KEYUP:
				key_released = (int)event.key.keysym.scancode;

				switch(event.key.keysym.scancode)
				{
					case SDL_SCANCODE_ESCAPE:
						key[KEY_ESC] = 0;
					break;

					default:
                    break;
				}

			break;

			case SDL_WINDOWEVENT:

				switch(event.window.event)
				{
					case SDL_WINDOWEVENT_ENTER:
					window.enableMouseFocus();
					break;

					case SDL_WINDOWEVENT_LEAVE:
					window.disableMouseFocus();
					break;

					case SDL_WINDOWEVENT_FOCUS_LOST:
					window.disableInputFocus();
					break;

					case SDL_WINDOWEVENT_FOCUS_GAINED:
					window.enableInputFocus();
					break;

					default:
                    break;
				}

			break;

            default:
            break;
		}
	}
}

/*---------------------------------------------------------
 Function: input_wait

 Description:
 Wait until all enabled input devices have been released.
---------------------------------------------------------*/
void input_wait(void)
{
   if (keyboard) keyboard_wait();
   if (mouse) mouse_wait();
   if (joypad) joypad_wait();
}

/*---------------------------------------------------------
 Function: input_fire

 Description:
 Waits until fire button on any enabled device has been
 pressed.
---------------------------------------------------------*/
void input_fire(void)
{
   int  j, k, m;
   int  x, y;

   k = j = m = 0;
   do {
		input_update();

      if (keyboard) getkeyboard(x, y, k);
      if (joypad) getjoypad(x, y, j);
      if (mouse) getmouse(x, y, m);
	} while ((!k && !j && !m) || !window.isFocus());
}


// Some device handling routines.
void (*getdevice(int n))(int &, int &, int &)
{
   return device[n].getmotion;
}

char *getdevicename(int n)
{
   return device[n].name;
}

// Scan all available devices starting with device n
// and return the next UNLOCKED device greater than n.
// Returning -1 if none could be found.
int nextdevice(int n)
{
   int   i;

   i = ndevices;
   do {
      if (++n >= ndevices) n = 0;
   } while (--i && device[n].locked);
   if (device[n].locked) return -1;
   return n;
}

// Lock device n.
// returning >0 if ok. 0 if lock attempt failed.
int lockdevice(int n)
{
   if (n >= ndevices) return 0;
   device[n].locked = 1;
   return n;
}

int unlockdevice(int n)
{
   if (n >= ndevices) return 0;
   device[n].locked = 0;
   return n;
}

// Dummy input device. The NONE device.
void getnone(int &x, int &y, int &fire)
{
   x = y = fire = 0;
}

/*---------------------------------------------------------
 Function: *

 Description:
 Initialize the INPUT SYSTEM.
 Prepare all available inputs and fill in device structures.
---------------------------------------------------------*/
void initinputs(int input)
{
   ndevices = 0;

// Dummy input device.
   device[ndevices].locked = 0;
   device[ndevices].name = "None";
   device[ndevices].getmotion = getnone;
   ndevices++;

   keyboard = joypad = mouse = 0;

// KEYBOARD.
   if (input & R_INPUT_KEYBOARD) {
      initkeyboard();
      if (keyboard) {
         device[ndevices].locked = 0;
         device[ndevices].name = "Keyboard";
         device[ndevices].getmotion = getkeyboard;
         ndevices++;
      }
   }

// JOYPAD.
   if (input & R_INPUT_JOYPAD) {
      initjoypad();
      if (joypad) {
         device[ndevices].locked = 0;
         device[ndevices].name = "Joypad";
         device[ndevices].getmotion = getjoypad;
         ndevices++;
      }
   }

// MOUSE.
   if (input & R_INPUT_MOUSE) {
      initmouse();
      if (mouse) {
         device[ndevices].locked = 0;
         device[ndevices].name = "Mouse";
         device[ndevices].getmotion = getmouse;
         ndevices++;
      }
   }
}

void shutinputs(void)
{
   shutmouse();
   shutjoypad();
   shutkeyboard();
}



