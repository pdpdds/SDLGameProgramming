/*   
 * Copyright (C) 2008 - 2011 The OpenTitus team
 *
 * Authors:
 * Eirik Stople
 *
 * "Titus the Fox: To Marrakech and Back" (1992) and
 * "Lagaf': Les Aventures de Moktar - Vol 1: La Zoubida" (1991)
 * was developed by, and is probably copyrighted by Titus Software,
 * which, according to Wikipedia, stopped buisness in 2005.
 *
 * OpenTitus is not affiliated with Titus Software.
 *
 * OpenTitus is  free software; you can redistribute  it and/or modify
 * it under the  terms of the GNU General  Public License as published
 * by the Free  Software Foundation; either version 3  of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the GNU
 * General Public License for more details.
 */

/* globals.h
 * Global variables
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include <SDL2/SDL.h>
#include "definitions.h"
#include "level.h"
#include "dingoo.h"

//To simplify porting:
#ifdef _DINGUX

/*
#define DINGOO_BUTTON_UP            SDLK_UP
#define DINGOO_BUTTON_DOWN          SDLK_DOWN
#define DINGOO_BUTTON_RIGHT         SDLK_RIGHT
#define DINGOO_BUTTON_LEFT          SDLK_LEFT
#define DINGOO_BUTTON_R             SDLK_BACKSPACE
#define DINGOO_BUTTON_L             SDLK_TAB
#define DINGOO_BUTTON_A             SDLK_LCTRL
#define DINGOO_BUTTON_B             SDLK_LALT
#define DINGOO_BUTTON_X             SDLK_SPACE
#define DINGOO_BUTTON_Y             SDLK_LSHIFT
#define DINGOO_BUTTON_SELECT        SDLK_ESCAPE
#define DINGOO_BUTTON_START         SDLK_RETURN
#define DINGOO_BUTTON_END           SDLK_UNKNOWN
*/

#define KEY_F1          SDLK_F1 //Loose a life, not in use
#define KEY_F2          SDLK_F2 //Game over, not in use
#define KEY_STATUS      DINGOO_BUTTON_A //Energy + status page
#define KEY_LEFT        DINGOO_BUTTON_LEFT //Left
#define KEY_RIGHT       DINGOO_BUTTON_RIGHT //Right
#define KEY_UP          DINGOO_BUTTON_UP //Up
#define KEY_DOWN        DINGOO_BUTTON_DOWN //Down
#define KEY_JUMP        DINGOO_BUTTON_X //Jump
#define KEY_SPACE       DINGOO_BUTTON_B //Space
#define KEY_ENTER       DINGOO_BUTTON_START //Enter
#define KEY_RETURN      DINGOO_BUTTON_START //Return
#define KEY_ESC         DINGOO_BUTTON_SELECT //Quit
#define KEY_P           DINGOO_BUTTON_START //Toggle pause (A button)
#define KEY_CHEAT       DINGOO_BUTTON_L //Toggle cheat noclip/godmode
#define KEY_SKIPLEVEL   DINGOO_BUTTON_R //Skip level
#define KEY_DEBUG       DINGOO_BUTTON_Y //Toggle debug mode
#define KEY_MUSIC       DINGOO_BUTTON_Y //Toggle music

#else

#define KEY_F1 SDLK_F1 //Loose a life
#define KEY_F2 SDLK_F2 //Game over
#define KEY_E SDLK_e //Display energy
#define KEY_F4 SDLK_F4 //Status page
#define KEY_LEFT SDLK_LEFT //Left
#define KEY_RIGHT SDLK_RIGHT //Right
#define KEY_UP SDLK_UP //Up
#define KEY_DOWN SDLK_DOWN //Down
#define KEY_JUMP SDLK_UP //Up
#define KEY_SPACE SDLK_SPACE //Space
#define KEY_ENTER SDLK_KP_ENTER //Enter
#define KEY_RETURN SDLK_RETURN //Return
#define KEY_ESC SDLK_ESCAPE //Quit
#define KEY_P SDLK_p //Toggle pause
#define KEY_NOCLIP SDLK_n //Toggle noclip
#define KEY_GODMODE SDLK_g //Toggle godmode
#define KEY_DEBUG SDLK_d //Toggle debug mode
#define KEY_MUSIC SDLK_F3 //Toggle music

#endif


#define TEST_ZONE 4
#define MAX_X 4
#define MAX_Y 12
#define MAP_LIMIT_Y -1
#define S_LINES 12
#define S_COLUMNS 20
#define LIMIT_TIMER 22
#define ALERT_X 32
#define screen_width 20
#define screen_height 12
#define FIRST_OBJET 30
#define FIRST_NMI 101
#define MAXIMUM_BONUS 100
#define MAXIMUM_ENERGY 16
#define MAXIMUM_DTRP 4
#define GESTION_X 40
#define GESTION_Y 20
#define MAX_SPEED_DEAD 20

#define HFLAG_NOWALL 0
#define HFLAG_WALL 1
#define HFLAG_BONUS 2
#define HFLAG_DEADLY 3
#define HFLAG_CODE 4
#define HFLAG_PADLOCK 5
#define HFLAG_LEVEL14 6

#define FFLAG_NOFLOOR 0
#define FFLAG_FLOOR 1
#define FFLAG_SSFLOOR 2
#define FFLAG_SFLOOR 3
#define FFLAG_VSFLOOR 4
#define FFLAG_DROP 5
#define FFLAG_LADDER 6
#define FFLAG_BONUS 7
#define FFLAG_WATER 8
#define FFLAG_FIRE 9
#define FFLAG_SPIKES 10
#define FFLAG_CODE 11
#define FFLAG_PADLOCK 12
#define FFLAG_LEVEL14 13

#define CFLAG_NOCEILING 0
#define CFLAG_CEILING 1
#define CFLAG_LADDER 2
#define CFLAG_PADLOCK 3
#define CFLAG_DEADLY 4

#define uint8 unsigned char
#define uint16 unsigned short int
#define int8 signed char
#define int16 signed short int


uint8 RESETLEVEL_FLAG;
bool GAMEOVER_FLAG; //triggers a game over
uint8 BAR_FLAG; //timer for health bar
bool X_FLAG; //true if left or right key is pressed
bool Y_FLAG; //true if up or down key is pressed
uint8 CHOC_FLAG; //headache timer
uint8 action; //player sprite array
uint8 KICK_FLAG; //hit/burn timer
bool GRANDBRULE_FLAG; //If set, player will be "burned" when hit (fireballs)
bool LADDER_FLAG; //True if in a ladder
bool PRIER_FLAG; //True if player is forced into kneestanding because of low ceiling
uint8 SAUT_FLAG; //6 if free fall or in the middle of a jump, decremented if on solid surface. Must be 0 to initiate a jump.
uint8 LAST_ORDER; //Last action (kneestand + jump = silent walk)
uint8 FURTIF_FLAG; //Silent walk timer
bool DROP_FLAG; //True if an object is throwed forward
bool DROPREADY_FLAG;
bool CARRY_FLAG; //true if carrying something (add 16 to player sprite)
bool POSEREADY_FLAG;
uint8 ACTION_TIMER; //Frames since last action change
//TITUS_sprite sprite; //Player sprite
//TITUS_sprite sprite2; //Secondary player sprite (throwed objects, "hit" when object hits an enemy, smoke when object hits the floor)
uint8 INVULNERABLE_FLAG; //When non-zero, boss is invulnerable
uint8 TAPISFLY_FLAG; //When non-zero, the flying carpet is flying
uint8 CROSS_FLAG; //When non-zero, fall through certain floors (after key down)
uint8 GRAVITY_FLAG; //When zero, skip object gravity function
uint8 FUME_FLAG; //Smoke when object hits the floor
Uint8 *keystate; //Keyboard state
uint8 LIFE; //Lives
uint8 YFALL;
bool POCKET_FLAG;
bool PERMUT_FLAG; //If false, there are no animated tiles on the screen?
uint8 loop_cycle; //Increased every loop in game loop
uint8 tile_anim; //Current tile animation (0-1-2), changed every 4th game loop cycle
uint8 BITMAP_X; //Screen offset (X) in tiles
uint8 BITMAP_XM; //Point to the left tile in the tile screen (0 to 19)
uint8 BITMAP_Y; //Screen offset (Y) in tiles
uint8 BITMAP_YM; //Point to the top tile in the tile screen (0 to 11)
bool XSCROLL_CENTER; //If true, the screen will scroll in X
int16 XLIMIT_SCROLL; //If scrolling: scroll until player is in this tile (X)
int16 XLIMIT; //The engine will not scroll past this tile before the player have crossed the line (X)
bool YSCROLL_CENTER; //If true, the screen will scroll in Y
uint8 YLIMIT_SCROLL; //If scrolling: scroll until player is in this tile (Y)
uint8 ALTITUDE_ZERO; //The engine will not scroll below this tile before the player have gone below (Y)
int LAST_CLOCK; //Used for fixed framerate
uint16 IMAGE_COUNTER; //Increased every loop in game loop (0 to 0x0FFF)
int8 SENSX; //1: walk right, 0: stand still, -1: walk left, triggers the ACTION_TIMER if it changes
uint8 SAUT_COUNT; //Incremented from 0 to 3 when accelerating while jumping, stop acceleration upwards if >= 3
bool NOSCROLL_FLAG;
bool NEWLEVEL_FLAG; //Finish a level
uint8 BIGNMI_NBR; //Number of bosses that needs to be killed to finish
uint8 TAUPE_FLAG; //Used for enemies walking and popping up
uint8 TAPISWAIT_FLAG; //Flying carpet state
uint8 SEECHOC_FLAG; //Counter when hit
bool NFC_FLAG; //Skip NO_FAST_CPU
uint8 BIGNMI_POWER; //Lives of the boss
bool boss_alive; //True if the boss is alive
uint8 AUDIOMODE;

bool GODMODE; //If true, the player will not interfere with the enemies
bool NOCLIP; //If true, the player will move noclip
bool DISPLAYLOOPTIME; //If true, display loop time in milliseconds
uint8 LOOPTIME; //Loop time
uint8 SUBTIME[16]; //Sub time
uint16 FPS; //Frames pr second
uint16 FPS_LAST; //Frames pr second
uint16 LAST_CLOCK_CORR; //Correction to LAST_CLOCK

typedef struct {
    bool enabled;
    uint16 NUM;
} SPRITE;

SPRITE sprites[256]; 

typedef struct {
    bool enabled;
    uint16 NUM;
} SPRITEDATA;

SPRITEDATA spritedata[256]; 

uint16 level_code[16];

#endif

