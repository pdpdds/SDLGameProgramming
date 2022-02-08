#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "SDL2/SDL_rect.h"

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

enum PlayerID
{
	PlayerID_One,
	PlayerID_Two
};

enum ComboTypes
{
	ComboTypes_ComboYogaFire,
	ComboTypes_ComboYogaFlame,
	ComboTypes_AerialCombo,
	ComboTypes_AerialComboPunch,
	ComboTypes_AerialComboKick,
	ComboTypes_ComboNothing
};

enum ColliderType
{
	ColliderType_PlayerOne,
	ColliderType_BodyPlayerOne,
	ColliderType_AttackPlayerOne,
	ColliderType_PlayerTwo,
	ColliderType_BodyPlayerTwo,
	ColliderType_AttackPlayerTwo,
	ColliderType_Particles,
	ColliderType_Others
};

enum UpdateStatus
{
	UpdateStatus_Continue = 1,
	UpdateStatus_Stop,
	UpdateStatus_Error
};

enum DirectionJumping
{
	DirectionJumping_NoMove,
	DirectionJumping_Left,
	DirectionJumping_Right
};

enum PlayerState
{
	PlayerState_Idle,
	PlayerState_WalkingForward,
	PlayerState_WalkingBackward,
	PlayerState_Jumping,
	PlayerState_Crouching,
	PlayerState_Hit,
	PlayerState_FaceHit,
	PlayerState_CrouchHit,
	PlayerState_Ko,
	PlayerState_TimeOut,
	PlayerState_Win1,
	PlayerState_Win2,
	PlayerState_LowPunch,
	PlayerState_LowKick,
	PlayerState_MediumPunch,
	PlayerState_MediumKick,
	PlayerState_HighPunch,
	PlayerState_HighKick,
	PlayerState_CrouchPunch,
	PlayerState_CrouchKick,
	PlayerState_JumpPunch,
	PlayerState_JumpKick,
	PlayerState_ForwardLowPunch,
	PlayerState_ForwardLowKick,
	PlayerState_ForwardMediumPunch,
	PlayerState_ForwardMediumKick,
	PlayerState_ForwardHighPunch,
	PlayerState_ForwardHighKick,
	PlayerState_YogaFire,
	PlayerState_YogaFlame,
	PlayerState_YogaMummy,
	PlayerState_YogaSpear,
	PlayerState_Blocking,
	PlayerState_CrouchBlocking,
	PlayerState_BlockingHitted,
	PlayerState_CrouchBlockingHitted,
	PlayerState_AirHitted
};

enum DamageType
{
	DamageType_LAttack,
	DamageType_MAttack,
	DamageType_HAttack,
	DamageType_None
};

enum InputType
{
	InputType_Up,
	InputType_Down,
	InputType_Left,
	InputType_Right,
	InputType_LPunch,
	InputType_LKick,
	InputType_MPunch,
	InputType_MKick,
	InputType_HPunch,
	InputType_HKick
};

enum LookingDirection
{
	LookingDirection_Right,
	LookingDirection_Left,
	LookingDirection_Invalid
};

// Deletes a buffer
#define RELEASE( x ) \
    {\
       if( x != nullptr )\
       {\
         delete x;\
	     x = nullptr;\
       }\
    }

// Deletes an array of buffers
#define RELEASE_ARRAY( x ) \
	{\
       if( x != nullptr )\
       {\
           delete[] x;\
	       x = nullptr;\
		 }\
	 }

// Configuration -----------
#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 224
#define SCREEN_SIZE 3
#define FULLSCREEN false
#define VSYNC true
#define TITLE "Super Street Fighter 2 - Tribute"

#endif //__GLOBALS_H__