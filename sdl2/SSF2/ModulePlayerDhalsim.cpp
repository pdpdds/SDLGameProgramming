#include "ModulePlayerDhalsim.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "SDL2/SDL.h"
#include "ModuleCollisions.h"
#include "ModuleParticleSystem.h"
#include "ModuleSceneBison.h"
#include "ModuleAudio.h"

#include <fstream>

// Basic Module Operations

// Constructors

ModulePlayerDhalsim::ModulePlayerDhalsim(PlayerID playerID, bool start_enabled)
	: Module(start_enabled),
	m_PlayerState(PlayerState_Idle),
	m_AreCollidingPlayers(false),
	m_Hitted(false),
	m_HeadHitted(false),
	m_LegHitted(false),
	m_Life(200),
	m_Wins(0),
	m_Win(false),
	m_Dead(false),
	m_Time0(false),
	m_StartingCombo(ComboTypes_ComboNothing),
	m_DamageType(DamageType_None),
	m_DistanceJumped(0),
	m_JumpAttacked(false),
	m_AlreadyHitted(false)
{
	m_PlayerID = playerID;

	SetUpAnimations();
	SetUpPlayer();

	m_LastShotTimer = SDL_GetTicks();

	m_ModuleComboDetection = new ModuleComboDetection(m_PlayerID);
}

// Destructor
ModulePlayerDhalsim::~ModulePlayerDhalsim()
{
	delete m_ModuleComboDetection;
}

void ModulePlayerDhalsim::SetUpAnimations()
{
	Json::Value root;

	std::filebuf fb;
	if (fb.open("Game/dhalsim_data.json", std::ios::in))
	{
		std::istream is(&fb);
		is >> root;
		fb.close();
	}

	// Movement animations
	SetPlayerAnimationDataFromJSON(idle, root["idle"][0]);
	SetPlayerAnimationDataFromJSON(forward, root["forward"][0]);
	SetPlayerAnimationDataFromJSON(backward, root["backward"][0]);
	SetPlayerAnimationDataFromJSON(crouching, root["crouching"][0]);
	SetPlayerAnimationDataFromJSON(block, root["block"][0]);
	SetPlayerAnimationDataFromJSON(crouch_block, root["crouch-block"][0]);
	SetPlayerAnimationDataFromJSON(jump, root["jump"][0]);

	// Far attacks animations
	SetPlayerAnimationDataFromJSON(L_punch, root["L_punch"][0]);
	SetPlayerAnimationDataFromJSON(M_punch, root["M_punch"][0]);
	SetPlayerAnimationDataFromJSON(H_punch, root["H_punch"][0]);
	SetPlayerAnimationDataFromJSON(L_kick, root["L_kick"][0]);
	SetPlayerAnimationDataFromJSON(M_kick, root["M_kick"][0]);
	SetPlayerAnimationDataFromJSON(H_kick, root["H_kick"][0]);

	// Near attacks
	SetPlayerAnimationDataFromJSON(F_L_punch, root["F_L_punch"][0]);
	SetPlayerAnimationDataFromJSON(F_M_punch, root["F_M_punch"][0]);
	SetPlayerAnimationDataFromJSON(F_H_punch, root["F_H_punch"][0]);
	SetPlayerAnimationDataFromJSON(F_L_kick, root["F_L_kick"][0]);
	SetPlayerAnimationDataFromJSON(F_M_kick, root["F_M_kick"][0]);
	SetPlayerAnimationDataFromJSON(F_H_kick, root["F_H_kick"][0]);

	// Especial position attacks
	SetPlayerAnimationDataFromJSON(crouch_punch, root["crouch_punch"][0]);
	SetPlayerAnimationDataFromJSON(crouch_kick, root["crouch_kick"][0]);
	SetPlayerAnimationDataFromJSON(jump_punch, root["jump_punch"][0]);
	SetPlayerAnimationDataFromJSON(jump_kick, root["jump_kick"][0]);

	// Hit animation
	SetPlayerAnimationDataFromJSON(hit, root["hit"][0]);
	SetPlayerAnimationDataFromJSON(face_hit, root["face_hit"][0]);
	SetPlayerAnimationDataFromJSON(crouch_hit, root["crouch_hit"][0]);
	SetPlayerAnimationDataFromJSON(air_hit, root["air_hit"][0]);

	// End animations
	SetPlayerAnimationDataFromJSON(ko, root["ko"][0]);
	SetPlayerAnimationDataFromJSON(victory1, root["victory1"][0]);
	SetPlayerAnimationDataFromJSON(victory2, root["victory2"][0]);
	SetPlayerAnimationDataFromJSON(time_out, root["time_out"][0]);

	// Special attacks
	SetPlayerAnimationDataFromJSON(yoga_fire, root["yoga_fire"][0]);
	SetPlayerAnimationDataFromJSON(yoga_flame, root["yoga_flame"][0]);
	SetPlayerAnimationDataFromJSON(yoga_mummy, root["yoga_mummy"][0]);
	SetPlayerAnimationDataFromJSON(yoga_spear, root["yoga_spear"][0]);

	// particle animations
	SetParticleAnimationDataFromJSON(m_Particle, root["particle"][0]);
	SetParticleAnimationDataFromJSON(m_DestroyParticle, root["destroy-particle"][0]);
}

void ModulePlayerDhalsim::SetUpPlayer()
{
	if (m_PlayerID == PlayerID_One)
	{
		m_Position.x = 150;
		m_Position.y = 200;

		m_PlayerCollider.type = ColliderType_BodyPlayerOne;
		m_ColliderHead.type = m_ColliderBody.type = m_ColliderLegs.type = ColliderType_PlayerOne;
		m_ColliderAttack.type = ColliderType_AttackPlayerOne;
	}
	else
	{
		m_Position.x = 300;
		m_Position.y = 200;

		m_PlayerCollider.type = ColliderType_BodyPlayerTwo;
		m_ColliderHead.type = m_ColliderBody.type = m_ColliderLegs.type = ColliderType_PlayerTwo;
		m_ColliderAttack.type = ColliderType_AttackPlayerTwo;
	}
	m_PlayerCollider.module = m_ColliderHead.module = m_ColliderBody.module = m_ColliderLegs.module = m_ColliderAttack.module = this;
}

// Load assets
bool ModulePlayerDhalsim::Start()
{
	LOG("Loading Dhalsim");

	if (m_PlayerID == PlayerID_One)
	{
		m_Graphics = App->textures->Load("Game/dhalsim.png");
		m_OtherPlayer = App->player_two;
	}
	else
	{
		m_Graphics = App->textures->Load("Game/dhalsim_recolor.png");
		m_OtherPlayer = App->player_one;
	}

	AddColliders();
	LoadAudioSounds();
	DecideLookingPosition();

	if (m_ModuleComboDetection)
	{
		m_ModuleComboDetection->Start();
	}

	return true;
}

void ModulePlayerDhalsim::AddColliders()
{
	App->collisions->AddCollider(&m_PlayerCollider);
	App->collisions->AddCollider(&m_ColliderHead);
	App->collisions->AddCollider(&m_ColliderBody);
	App->collisions->AddCollider(&m_ColliderLegs);
	App->collisions->AddCollider(&m_ColliderAttack);
}

void ModulePlayerDhalsim::LoadAudioSounds()
{
	m_AudioIdYogaFire = App->audio->LoadFx("Game/yoga_fire.wav");
	m_AudioIdYogaFlame = App->audio->LoadFx("Game/yoga_flame.wav");
	m_AudioIdDead = App->audio->LoadFx("Game/dhalsim_dead.wav");
	m_AudioIdLAttack = App->audio->LoadFx("Game/SF2_hit_1.wav");;
	m_AudioIdMAttack = App->audio->LoadFx("Game/SF2_hit_2.wav");;
	m_AudioIdHAttack = App->audio->LoadFx("Game/SF2_hit_3.wav");;
	m_AudioIdLImpact = App->audio->LoadFx("Game/SF2_impact_1.wav");;
	m_AudioIdMImpact = App->audio->LoadFx("Game/SF2_impact_2.wav");;
	m_AudioIdHImpact = App->audio->LoadFx("Game/SF2_impact_3.wav");;
}

void ModulePlayerDhalsim::DecideLookingPosition()
{
	if (m_OtherPlayer->GetPosition().x > m_Position.x)
	{
		m_LookingDirection = LookingDirection_Right;
	}
	else
	{
		m_LookingDirection = LookingDirection_Left;
	}
}

// Unload assets
bool ModulePlayerDhalsim::CleanUp()
{
	LOG("Unloading Base player");

	App->textures->Unload(m_Graphics);

	return true;
}

// PreUpdate
UpdateStatus ModulePlayerDhalsim::PreUpdate()
{
	bool near = false;

	if (m_ModuleComboDetection)
	{
		m_ModuleComboDetection->PreUpdate();
	}

	DecideLookingPosition();

	if ((m_OtherPlayer->GetPosition().x - m_Position.x) < 50 &&
		(m_OtherPlayer->GetPosition().x - m_Position.x) > -50)
	{
		near = true;
	}

	if (m_Dead)
	{
		if (m_PlayerState != PlayerState_Ko)
		{
			m_PlayerState = PlayerState_Ko;
			m_OtherPlayer->SetWin(true);

		}
		return UpdateStatus_Continue;
	}
	else if (m_Time0)
	{
		m_PlayerState = PlayerState_TimeOut;
	}
	else if (m_Win &&
		(m_PlayerState == PlayerState_Idle ||
		m_PlayerState == PlayerState_Crouching ||
		m_PlayerState == PlayerState_WalkingForward ||
		m_PlayerState == PlayerState_WalkingBackward))
	{
		if (m_PlayerState != PlayerState_Win1 && m_PlayerState != PlayerState_Win2)
		{
			if (m_Wins == 0)
			{
				++m_Wins;
				m_PlayerState = PlayerState_Win1;
			}
			else
			{
				++m_Wins;
				m_PlayerState = PlayerState_Win2;
			}
		}
		return UpdateStatus_Continue;
	}
	else if (m_PlayerState == PlayerState_BlockingHitted || m_PlayerState == PlayerState_CrouchBlockingHitted);
	else if (m_LegHitted && m_PlayerState == PlayerState_Blocking)
	{
		m_PlayerState = PlayerState_Hit;
	}
	else if (m_Hitted && m_PlayerState == PlayerState_Blocking)
	{
		m_PlayerState = PlayerState_BlockingHitted;
		block.RestartFrames();
	}
	else if (m_Hitted && m_PlayerState == PlayerState_CrouchBlocking)
	{
		m_PlayerState = PlayerState_CrouchBlockingHitted;
		crouch_block.RestartFrames();
	}
	else if ((m_Hitted || m_HeadHitted) && (m_PlayerState == PlayerState_Jumping || m_PlayerState == PlayerState_JumpPunch || m_PlayerState == PlayerState_JumpKick || m_PlayerState == PlayerState_AirHitted))
	{
		m_PlayerState = PlayerState_AirHitted;
	}
	else if ((m_Hitted || m_HeadHitted) && (m_PlayerState == PlayerState_Crouching || m_PlayerState == PlayerState_CrouchPunch || m_PlayerState == PlayerState_CrouchKick))
	{
		m_PlayerState = PlayerState_CrouchHit;
	}
	else if (m_HeadHitted)
	{
		m_PlayerState = PlayerState_FaceHit;
	}
	else if (m_Hitted)
	{
		m_PlayerState = PlayerState_Hit;
	}

	switch (m_PlayerState)
	{

	case PlayerState_Idle:

		if (m_StartingCombo == ComboTypes_ComboYogaFire)
		{
			App->audio->PlayFx(m_AudioIdYogaFire);
			m_PlayerState = PlayerState_YogaFire;
		}
		else if (m_StartingCombo == ComboTypes_ComboYogaFlame)
		{
			App->audio->PlayFx(m_AudioIdYogaFlame);
			m_PlayerState = PlayerState_YogaFlame;
		}
		else if (GetPlayerInput(InputType_Right) &&
			GetPlayerInput(InputType_Up))
		{
			m_DistanceJumped = 0;
			m_GoingUp = true;
			m_PlayerState = PlayerState_Jumping;
			m_DirectionJump = DirectionJumping_Right;
		}
		else if (GetPlayerInput(InputType_Left) &&
			GetPlayerInput(InputType_Up))
		{
			m_DistanceJumped = 0;
			m_GoingUp = true;
			m_PlayerState = PlayerState_Jumping;
			m_DirectionJump = DirectionJumping_Left;
		}
		else if (GetPlayerInput(InputType_Right) &&
			IsLookingRight())
		{
			m_PlayerState = PlayerState_WalkingForward;
		}
		else if (GetPlayerInput(InputType_Right) &&
			m_OtherPlayer->IsAttacking())
		{
			m_PlayerState = PlayerState_Blocking;
		}
		else if (GetPlayerInput(InputType_Right))
		{
			m_PlayerState = PlayerState_WalkingBackward;
		}
		else if (GetPlayerInput(InputType_Right) &&
			IsLookingRight() && m_OtherPlayer->IsAttacking())
		{
			m_PlayerState = PlayerState_Blocking;
		}
		else if (GetPlayerInput(InputType_Left) &&
			IsLookingRight())
		{
			m_PlayerState = PlayerState_WalkingBackward;
		}
		else if (GetPlayerInput(InputType_Left))
		{
			m_PlayerState = PlayerState_WalkingForward;
		}
		else if (GetPlayerInput(InputType_Down))
		{
			m_PlayerState = PlayerState_Crouching;
		}
		else if (GetPlayerInput(InputType_Up))
		{
			m_DistanceJumped = 0;
			m_GoingUp = true;
			m_PlayerState = PlayerState_Jumping;
			m_DirectionJump = DirectionJumping_NoMove;
		}
		if (near && m_StartingCombo == ComboTypes_ComboNothing)
		{
			if (GetPlayerInput(InputType_LPunch))
			{
				m_PlayerState = PlayerState_ForwardLowPunch;
				App->audio->PlayFx(m_AudioIdLAttack);
			}
			else if (GetPlayerInput(InputType_LKick))
			{
				m_PlayerState = PlayerState_ForwardLowKick;
				App->audio->PlayFx(m_AudioIdLAttack);
			}
			else if (GetPlayerInput(InputType_MPunch))
			{
				m_PlayerState = PlayerState_ForwardMediumPunch;
				App->audio->PlayFx(m_AudioIdMAttack);
			}
			else if (GetPlayerInput(InputType_MKick))
			{
				m_PlayerState = PlayerState_ForwardMediumKick;
				App->audio->PlayFx(m_AudioIdMAttack);
			}
			else if (GetPlayerInput(InputType_HPunch))
			{
				m_PlayerState = PlayerState_ForwardHighPunch;
				App->audio->PlayFx(m_AudioIdHAttack);
			}
			else if (GetPlayerInput(InputType_HKick))
			{
				m_PlayerState = PlayerState_ForwardHighKick;
				App->audio->PlayFx(m_AudioIdHAttack);
			}
		}
		else if (m_StartingCombo == ComboTypes_ComboNothing)
		{
			if (GetPlayerInput(InputType_LPunch))
			{
				m_PlayerState = PlayerState_LowPunch;
				App->audio->PlayFx(m_AudioIdLAttack);
			}
			else if (GetPlayerInput(InputType_LKick))
			{
				m_PlayerState = PlayerState_LowKick;
				App->audio->PlayFx(m_AudioIdLAttack);
			}
			else if (GetPlayerInput(InputType_MPunch))
			{
				m_PlayerState = PlayerState_MediumPunch;
				App->audio->PlayFx(m_AudioIdMAttack);
			}
			else if (GetPlayerInput(InputType_MKick))
			{
				m_PlayerState = PlayerState_MediumKick;
				App->audio->PlayFx(m_AudioIdMAttack);
			}
			else if (GetPlayerInput(InputType_HPunch))
			{
				m_PlayerState = PlayerState_HighPunch;
				App->audio->PlayFx(m_AudioIdHAttack);
			}
			else if (GetPlayerInput(InputType_HKick))
			{
				m_PlayerState = PlayerState_HighKick;
				App->audio->PlayFx(m_AudioIdHAttack);
			}
		}
		break;

	case PlayerState_WalkingForward:

		if (m_StartingCombo == ComboTypes_ComboYogaFire)
		{
			App->audio->PlayFx(m_AudioIdYogaFire);
			m_PlayerState = PlayerState_YogaFire;
		}
		else if (m_StartingCombo == ComboTypes_ComboYogaFlame)
		{
			App->audio->PlayFx(m_AudioIdYogaFlame);
			m_PlayerState = PlayerState_YogaFlame;
		}
		else if (m_StartingCombo == ComboTypes_AerialComboPunch)
		{
			m_PlayerState = PlayerState_YogaMummy;
		}
		else if (GetPlayerInput(InputType_Right) &&
			GetPlayerInput(InputType_Up))
		{
			m_DistanceJumped = 0;
			m_GoingUp = true;
			m_PlayerState = PlayerState_Jumping;
			m_DirectionJump = DirectionJumping_Right;
		}
		else if (GetPlayerInput(InputType_Left) &&
			GetPlayerInput(InputType_Up))
		{
			m_DistanceJumped = 0;
			m_GoingUp = true;
			m_PlayerState = PlayerState_Jumping;
			m_DirectionJump = DirectionJumping_Left;
		}
		else if (GetPlayerInput(InputType_Right))
		{
			if (IsLookingRight())
				m_PlayerState = PlayerState_WalkingForward;
			else if (m_OtherPlayer->IsAttacking())
				m_PlayerState = PlayerState_Blocking;
			else
				m_PlayerState = PlayerState_WalkingBackward;
		}
		else if (GetPlayerInput(InputType_Left))
		{
			if (IsLookingLeft())
				m_PlayerState = PlayerState_WalkingForward;
			else if (m_OtherPlayer->IsAttacking())
				m_PlayerState = PlayerState_Blocking;
			else
				m_PlayerState = PlayerState_WalkingBackward;
		}
		else if (GetPlayerInput(InputType_Down))
		{
			m_PlayerState = PlayerState_Crouching;
		}
		else
			m_PlayerState = PlayerState_Idle;

		if (near && m_StartingCombo == ComboTypes_ComboNothing)
		{
			if (GetPlayerInput(InputType_LPunch))
			{
				m_PlayerState = PlayerState_ForwardLowPunch;
				App->audio->PlayFx(m_AudioIdLAttack);
			}
			else if (GetPlayerInput(InputType_LKick))
			{
				m_PlayerState = PlayerState_ForwardLowKick;
				App->audio->PlayFx(m_AudioIdLAttack);
			}
			else if (GetPlayerInput(InputType_MPunch))
			{
				m_PlayerState = PlayerState_ForwardMediumPunch;
				App->audio->PlayFx(m_AudioIdMAttack);
			}
			else if (GetPlayerInput(InputType_MKick))
			{
				m_PlayerState = PlayerState_ForwardMediumKick;
				App->audio->PlayFx(m_AudioIdMAttack);
			}
			else if (GetPlayerInput(InputType_HPunch))
			{
				m_PlayerState = PlayerState_ForwardHighPunch;
				App->audio->PlayFx(m_AudioIdHAttack);
			}
			else if (GetPlayerInput(InputType_HKick))
			{
				m_PlayerState = PlayerState_ForwardHighKick;
				App->audio->PlayFx(m_AudioIdHAttack);
			}
		}
		else if (m_StartingCombo == ComboTypes_ComboNothing)
		{
			if (GetPlayerInput(InputType_LPunch))
			{
				m_PlayerState = PlayerState_LowPunch;
				App->audio->PlayFx(m_AudioIdLAttack);
			}
			else if (GetPlayerInput(InputType_LKick))
			{
				m_PlayerState = PlayerState_LowKick;
				App->audio->PlayFx(m_AudioIdLAttack);
			}
			else if (GetPlayerInput(InputType_MPunch))
			{
				m_PlayerState = PlayerState_MediumPunch;
				App->audio->PlayFx(m_AudioIdMAttack);
			}
			else if (GetPlayerInput(InputType_MKick))
			{
				m_PlayerState = PlayerState_MediumKick;
				App->audio->PlayFx(m_AudioIdMAttack);
			}
			else if (GetPlayerInput(InputType_HPunch))
			{
				m_PlayerState = PlayerState_HighPunch;
				App->audio->PlayFx(m_AudioIdHAttack);
			}
			else if (GetPlayerInput(InputType_HKick))
			{
				m_PlayerState = PlayerState_HighKick;
				App->audio->PlayFx(m_AudioIdHAttack);
			}
		}
		break;

	case PlayerState_WalkingBackward:

		if (m_StartingCombo == ComboTypes_ComboYogaFire)
		{
			App->audio->PlayFx(m_AudioIdYogaFire);
			m_PlayerState = PlayerState_YogaFire;
		}
		else if (m_StartingCombo == ComboTypes_ComboYogaFlame)
		{
			App->audio->PlayFx(m_AudioIdYogaFlame);
			m_PlayerState = PlayerState_YogaFlame;
		}
		else if (m_OtherPlayer->IsAttacking())
		{
			m_PlayerState = PlayerState_Blocking;
			break;
		}
		else if (GetPlayerInput(InputType_Right) &&
			GetPlayerInput(InputType_Up))
		{
			m_DistanceJumped = 0;
			m_GoingUp = true;
			m_PlayerState = PlayerState_Jumping;
			m_DirectionJump = DirectionJumping_Right;
		}
		else if (GetPlayerInput(InputType_Left) &&
			GetPlayerInput(InputType_Up))
		{
			m_DistanceJumped = 0;
			m_GoingUp = true;
			m_PlayerState = PlayerState_Jumping;
			m_DirectionJump = DirectionJumping_Left;
		}
		else if (GetPlayerInput(InputType_Right))
		{
			if (IsLookingRight())
				m_PlayerState = PlayerState_WalkingForward;
			else
				m_PlayerState = PlayerState_WalkingBackward;
		}
		else if (GetPlayerInput(InputType_Left))
		{
			if (IsLookingRight())
				m_PlayerState = PlayerState_WalkingBackward;
			else
				m_PlayerState = PlayerState_WalkingForward;
		}
		else if (GetPlayerInput(InputType_Down))
		{
			m_PlayerState = PlayerState_Crouching;
		}
		else
		{
			m_PlayerState = PlayerState_Idle;
		}
		if (near && m_StartingCombo == ComboTypes_ComboNothing)
		{
			if (GetPlayerInput(InputType_LPunch))
			{
				m_PlayerState = PlayerState_ForwardLowPunch;
				App->audio->PlayFx(m_AudioIdLAttack);
			}
			else if (GetPlayerInput(InputType_LKick))
			{
				m_PlayerState = PlayerState_ForwardLowKick;
				App->audio->PlayFx(m_AudioIdLAttack);
			}
			else if (GetPlayerInput(InputType_MPunch))
			{
				m_PlayerState = PlayerState_ForwardMediumPunch;
				App->audio->PlayFx(m_AudioIdMAttack);
			}
			else if (GetPlayerInput(InputType_MKick))
			{
				m_PlayerState = PlayerState_ForwardMediumKick;
				App->audio->PlayFx(m_AudioIdMAttack);
			}
			else if (GetPlayerInput(InputType_HPunch))
			{
				m_PlayerState = PlayerState_ForwardHighPunch;
				App->audio->PlayFx(m_AudioIdHAttack);
			}
			else if (GetPlayerInput(InputType_HKick))
			{
				m_PlayerState = PlayerState_ForwardHighKick;
				App->audio->PlayFx(m_AudioIdHAttack);
			}
		}
		else if (m_StartingCombo == ComboTypes_ComboNothing)
		{
			if (GetPlayerInput(InputType_LPunch))
			{
				m_PlayerState = PlayerState_LowPunch;
				App->audio->PlayFx(m_AudioIdLAttack);
			}
			else if (GetPlayerInput(InputType_LKick))
			{
				m_PlayerState = PlayerState_LowKick;
				App->audio->PlayFx(m_AudioIdLAttack);
			}
			else if (GetPlayerInput(InputType_MPunch))
			{
				m_PlayerState = PlayerState_MediumPunch;
				App->audio->PlayFx(m_AudioIdMAttack);
			}
			else if (GetPlayerInput(InputType_MKick))
			{
				m_PlayerState = PlayerState_MediumKick;
				App->audio->PlayFx(m_AudioIdMAttack);
			}
			else if (GetPlayerInput(InputType_HPunch))
			{
				m_PlayerState = PlayerState_HighPunch;
				App->audio->PlayFx(m_AudioIdHAttack);
			}
			else if (GetPlayerInput(InputType_HKick))
			{
				m_PlayerState = PlayerState_HighKick;
				App->audio->PlayFx(m_AudioIdHAttack);
			}
		}

		break;

	case PlayerState_Jumping:

		if (m_DistanceJumped == 0 && !m_GoingUp)
		{
			m_AlreadyHitted = false;
			m_PlayerState = PlayerState_Idle;
			m_JumpAttacked = false;
			jump.RestartFrames();
		}
		else if (m_StartingCombo == ComboTypes_AerialComboPunch && !m_JumpAttacked)
		{
			m_AlreadyHitted = false;
			m_JumpAttacked = true;
			m_PlayerState = PlayerState_YogaMummy;
			if (IsLookingRight())
				m_DirectionMummy = DirectionJumping_Right;
			else
				m_DirectionMummy = DirectionJumping_Left;
		}
		else if (m_StartingCombo == ComboTypes_AerialComboKick && !m_JumpAttacked)
		{
			m_AlreadyHitted = false;
			m_JumpAttacked = true;
			m_PlayerState = PlayerState_YogaSpear;
			if (IsLookingRight())
				m_DirectionMummy = DirectionJumping_Right;
			else
				m_DirectionMummy = DirectionJumping_Left;
		}
		else
		{
			if (((GetPlayerInput(InputType_LPunch)) ||
				(GetPlayerInput(InputType_MPunch)) ||
				(GetPlayerInput(InputType_HPunch))) &&
				!m_JumpAttacked)
			{
				m_JumpAttacked = true;
				m_PlayerState = PlayerState_JumpPunch;
			}

			else if (((GetPlayerInput(InputType_LKick)) ||
				(GetPlayerInput(InputType_MKick)) ||
				(GetPlayerInput(InputType_HKick))) &&
				!m_JumpAttacked)
			{
				m_JumpAttacked = true;
				m_PlayerState = PlayerState_JumpKick;
			}
		}
		break;


	case PlayerState_JumpPunch:

		if (m_DistanceJumped == 0)
		{
			m_AlreadyHitted = false;
			m_JumpAttacked = false;
			m_PlayerState = PlayerState_Idle;
			jump.RestartFrames();
			jump_punch.RestartFrames();
		}
		else if (jump_punch.IsEnded())
		{
			jump_punch.RestartFrames();
			m_PlayerState = PlayerState_Jumping;
		}
		break;

	case PlayerState_JumpKick:
		if (m_DistanceJumped == 0)
		{
			m_AlreadyHitted = false;
			m_JumpAttacked = false;
			m_PlayerState = PlayerState_Idle;
			jump.RestartFrames();
			jump_kick.RestartFrames();
		}
		else if (jump_kick.IsEnded())
		{
			jump_kick.RestartFrames();
			m_PlayerState = PlayerState_Jumping;
		}
		break;


	case PlayerState_Crouching:

		if ((GetPlayerInput(InputType_Right))
			&& m_OtherPlayer->IsAttacking() && IsLookingLeft())
			m_PlayerState = PlayerState_CrouchBlocking;
		else if ((GetPlayerInput(InputType_Left))
			&& m_OtherPlayer->IsAttacking() && IsLookingRight())
			m_PlayerState = PlayerState_CrouchBlocking;
		else if ((GetPlayerInput(InputType_LPunch)) ||
			(GetPlayerInput(InputType_MPunch)) ||
			(GetPlayerInput(InputType_HPunch)))
			m_PlayerState = PlayerState_CrouchPunch;

		else if ((GetPlayerInput(InputType_LKick)) ||
			(GetPlayerInput(InputType_MKick)) ||
			(GetPlayerInput(InputType_HKick)))
			m_PlayerState = PlayerState_CrouchKick;

		else if (!(GetPlayerInput(InputType_Down)))
		{
			m_PlayerState = PlayerState_Idle;
			crouching.RestartFrames();
		}
		break;

	case PlayerState_LowPunch:

		if (L_punch.IsEnded())
		{
			L_punch.RestartFrames();
			m_PlayerState = PlayerState_Idle;
		}
		break;

	case PlayerState_LowKick:

		if (L_kick.IsEnded())
		{
			L_kick.RestartFrames();
			m_PlayerState = PlayerState_Idle;
		}
		break;

	case PlayerState_MediumPunch:

		if (M_punch.IsEnded())
		{
			M_punch.RestartFrames();
			m_PlayerState = PlayerState_Idle;
		}
		break;

	case PlayerState_MediumKick:

		if (M_kick.IsEnded())
		{
			M_kick.RestartFrames();
			m_PlayerState = PlayerState_Idle;
		}
		break;

	case PlayerState_HighPunch:

		if (H_punch.IsEnded())
		{
			H_punch.RestartFrames();
			m_PlayerState = PlayerState_Idle;
		}
		break;

	case PlayerState_HighKick:

		if (H_kick.IsEnded())
		{
			H_kick.RestartFrames();
			m_PlayerState = PlayerState_Idle;
		}
		break;

	case PlayerState_CrouchPunch:

		if (crouch_punch.IsEnded())
		{
			crouch_punch.RestartFrames();
			m_PlayerState = PlayerState_Crouching;
		}
		break;

	case PlayerState_CrouchKick:

		if (crouch_kick.IsEnded())
		{
			crouch_kick.RestartFrames();
			m_PlayerState = PlayerState_Crouching;
		}
		break;

	case PlayerState_ForwardLowPunch:

		if (F_L_punch.IsEnded())
		{
			F_L_punch.RestartFrames();
			m_PlayerState = PlayerState_Idle;
		}
		break;

	case PlayerState_ForwardLowKick:

		if (F_L_kick.IsEnded())
		{
			F_L_kick.RestartFrames();
			m_PlayerState = PlayerState_Idle;
		}
		break;

	case PlayerState_ForwardMediumPunch:

		if (F_M_punch.IsEnded())
		{
			F_M_punch.RestartFrames();
			m_PlayerState = PlayerState_Idle;
		}
		break;

	case PlayerState_ForwardMediumKick:

		if (F_M_kick.IsEnded())
		{
			F_M_kick.RestartFrames();
			m_PlayerState = PlayerState_Idle;
		}
		break;

	case PlayerState_ForwardHighPunch:

		if (F_H_punch.IsEnded())
		{
			F_H_punch.RestartFrames();
			m_PlayerState = PlayerState_Idle;
		}
		break;

	case PlayerState_ForwardHighKick:

		if (F_H_kick.IsEnded())
		{
			F_H_kick.RestartFrames();
			m_PlayerState = PlayerState_Idle;
		}
		break;

	case PlayerState_Hit:

		if (hit.IsEnded())
		{
			hit.RestartFrames();
			if (m_DistanceJumped > 0){
				m_GoingUp = false;
				m_PlayerState = PlayerState_Jumping;
			}
			else
				m_PlayerState = PlayerState_Idle;
			m_Hitted = false;
			m_HeadHitted = false;
			m_LegHitted = false;
			m_AlreadyHitted = false;
		}
		break;

	case PlayerState_CrouchHit:

		if (crouch_hit.IsEnded())
		{
			crouch_hit.RestartFrames();
			m_PlayerState = PlayerState_Crouching;
			m_Hitted = false;
			m_HeadHitted = false;
			m_LegHitted = false;
			m_AlreadyHitted = false;
		}
		break;

	case PlayerState_FaceHit:

		if (face_hit.IsEnded())
		{
			face_hit.RestartFrames();
			m_PlayerState = PlayerState_Idle;
			m_Hitted = false;
			m_HeadHitted = false;
			m_LegHitted = false;
			m_AlreadyHitted = false;
		}
		break;

	case PlayerState_YogaFire:
		if (yoga_fire.GetCurrentFrameNumber() == 3 && SDL_GetTicks() - m_LastShotTimer > 500)
		{
			if (IsLookingRight())
			{
				iPoint particlePosition(m_Position.x + 42 + 14, m_Position.y - 54 + 10);
				App->particles->newParticle(particlePosition, m_Graphics, m_Particle, m_DestroyParticle, 2);
			}
			else
			{
				iPoint particlePosition(m_Position.x - 42 - 14, m_Position.y - 54 + 10);
				App->particles->newParticle(particlePosition, m_Graphics, m_Particle, m_DestroyParticle, -2);
			}
			m_LastShotTimer = SDL_GetTicks();
		}

		if (yoga_fire.IsEnded())
		{
			yoga_fire.RestartFrames();
			m_PlayerState = PlayerState_Idle;
		}
		break;

	case PlayerState_YogaFlame:

		if (yoga_flame.IsEnded())
		{
			yoga_flame.RestartFrames();
			m_PlayerState = PlayerState_Idle;

		}
		break;

	case PlayerState_YogaMummy:

		m_GoingUp = false;
		if (m_DistanceJumped == 0)
		{
			m_AlreadyHitted = false;
			yoga_mummy.RestartFrames();
			m_PlayerState = PlayerState_Idle;
			m_JumpAttacked = false;
		}
		else if (yoga_mummy.IsEnded() && m_DistanceJumped > 0)
		{
			yoga_mummy.RestartFrames();
			m_PlayerState = PlayerState_Jumping;
			m_JumpAttacked = true;
		}
		else if (yoga_mummy.IsEnded())
		{
			yoga_mummy.RestartFrames();
			m_PlayerState = PlayerState_Idle;
			m_JumpAttacked = false;
		}
		break;

	case PlayerState_YogaSpear:

		m_GoingUp = false;
		if (m_DistanceJumped == 0)
		{
			m_AlreadyHitted = false;
			yoga_spear.RestartFrames();
			m_PlayerState = PlayerState_Idle;
			m_JumpAttacked = false;
		}
		else if (yoga_spear.IsEnded() && m_DistanceJumped > 0)
		{
			yoga_spear.RestartFrames();
			m_PlayerState = PlayerState_Jumping;
			m_JumpAttacked = true;
		}
		else if (yoga_spear.IsEnded())
		{
			yoga_spear.RestartFrames();
			m_PlayerState = PlayerState_Idle;
			m_JumpAttacked = false;
		}
		break;

	case PlayerState_Blocking:
		if (!m_OtherPlayer->IsAttacking() ||
			(IsLookingRight() && !(GetPlayerInput(InputType_Left))) ||
			(IsLookingLeft() && !(GetPlayerInput(InputType_Right))))
		{
			block.RestartFrames();
			m_PlayerState = PlayerState_Idle;
		}
		else if (m_OtherPlayer->IsAttacking() &&
			(GetPlayerInput(InputType_Down)))
		{
			block.RestartFrames();
			m_PlayerState = PlayerState_CrouchBlocking;
		}
		else if (block.IsEnded())
			block.RestartFrames();
		break;

	case PlayerState_CrouchBlocking:
		if ((!m_OtherPlayer->IsAttacking() ||
			(IsLookingRight() && !(GetPlayerInput(InputType_Left))) ||
			(IsLookingLeft() && !(GetPlayerInput(InputType_Right))))
			&& (GetPlayerInput(InputType_Down)))
		{
			crouch_block.RestartFrames();
			m_PlayerState = PlayerState_Crouching;
		}
		else if (!(GetPlayerInput(InputType_Down)) && (!m_OtherPlayer->IsAttacking() ||
			(IsLookingRight() && !(GetPlayerInput(InputType_Left))) ||
			(IsLookingLeft() && !(GetPlayerInput(InputType_Right)))))
		{
			crouch_block.RestartFrames();
			m_PlayerState = PlayerState_Idle;
		}
		else if (m_OtherPlayer->IsAttacking() && !(GetPlayerInput(InputType_Down)) &&
			((IsLookingRight() && (GetPlayerInput(InputType_Left))) ||
			(IsLookingLeft() && (GetPlayerInput(InputType_Right)))))
		{
			crouch_block.RestartFrames();
			m_PlayerState = PlayerState_Blocking;
		}
		else if (crouch_block.IsEnded())
			crouch_block.RestartFrames();
		break;

	case PlayerState_BlockingHitted:
		if (block.IsEnded())
		{
			m_Hitted = false;
			m_HeadHitted = false;
			m_LegHitted = false;
			m_AlreadyHitted = false;
			block.RestartFrames();
			m_PlayerState = PlayerState_Blocking;
		}
		break;

	case PlayerState_CrouchBlockingHitted:
		if (crouch_block.IsEnded())
		{
			m_Hitted = false;
			m_HeadHitted = false;
			m_LegHitted = false;
			m_AlreadyHitted = false;
			crouch_block.RestartFrames();
			m_PlayerState = PlayerState_CrouchBlocking;
		}
		break;

	case PlayerState_AirHitted:
		if (air_hit.IsEnded())
		{
			m_Hitted = false;
			m_HeadHitted = false;
			m_LegHitted = false;
			air_hit.RestartFrames();
			m_GoingUp = false;
			m_PlayerState = PlayerState_Jumping;
		}
		break;
	}

	m_StartingCombo = ComboTypes_ComboNothing;

	return UpdateStatus_Continue;
}

// Update
UpdateStatus ModulePlayerDhalsim::Update()
{
	SDL_Rect aux;
	int pivot;
	Collider_player_structure cps;

	if (m_OtherPlayer->IsPlayerInCameraLimit() &&
		((App->renderer->ScreenLeftLimit() && m_OtherPlayer->GetLookingDirection() == LookingDirection_Right) ||
		(App->renderer->ScreenRightLimit() && m_OtherPlayer->GetLookingDirection() == LookingDirection_Left)) &&
		((m_OtherPlayer->GetPlayerState() == PlayerState_AirHitted) ||
		(m_OtherPlayer->GetPlayerState() == PlayerState_CrouchHit) ||
		(m_OtherPlayer->GetPlayerState() == PlayerState_Hit) ||
		(m_OtherPlayer->GetPlayerState() == PlayerState_FaceHit) ||
		(m_OtherPlayer->GetPlayerState() == PlayerState_BlockingHitted) ||
		(m_OtherPlayer->GetPlayerState() == PlayerState_CrouchBlockingHitted)) &&
		(m_PlayerState != PlayerState_JumpKick) && (m_PlayerState != PlayerState_JumpPunch) && 
		(m_PlayerState != PlayerState_Jumping) && (m_PlayerState != PlayerState_AirHitted))
	{
		if (IsLookingRight() && (m_OtherPlayer->GetPlayerState() == PlayerState_AirHitted))
			MovePlayer(-3);
		else if (IsLookingRight())
			MovePlayer(-1);
		else if (m_OtherPlayer->GetPlayerState() == PlayerState_AirHitted)
			MovePlayer(3);
		else
			MovePlayer(1);
	}

	switch (m_PlayerState)
	{
	case PlayerState_Idle:
		aux = idle.GetCurrentFrame();
		pivot = idle.GetCurrentPivot();
		cps = idle.GetCurrentCollider();
		idle.NextFrame();
		break;

	case PlayerState_WalkingForward:
		if (!m_AreCollidingPlayers)
		{
			if (IsLookingRight())
				MovePlayer(2);
			else
				MovePlayer(-2);
		}
		aux = forward.GetCurrentFrame();
		pivot = forward.GetCurrentPivot();
		cps = forward.GetCurrentCollider();
		forward.NextFrame();
		break;

	case PlayerState_WalkingBackward:
		if (IsLookingRight())
			MovePlayer(-1);
		else
			MovePlayer(1);
		aux = backward.GetCurrentFrame();
		pivot = backward.GetCurrentPivot();
		cps = backward.GetCurrentCollider();
		backward.NextFrame();
		break;

	case PlayerState_Crouching:
		aux = crouching.GetCurrentFrame();
		pivot = crouching.GetCurrentPivot();
		cps = crouching.GetCurrentCollider();
		crouching.NextFrame();
		break;

	case PlayerState_Jumping:
		if (m_GoingUp && m_DistanceJumped < 105)
		{
			m_DistanceJumped += 3;
		}
		else if (m_GoingUp)
			m_GoingUp = false;
		else
		{
			m_DistanceJumped -= 3;
		}

		if (m_DirectionJump == DirectionJumping_Right)
		{
			MovePlayer(1);
		}
		else if (m_DirectionJump == DirectionJumping_Left)
		{
			MovePlayer(-1);
		}

		if (m_DistanceJumped > 90)
		{
			aux = jump.frames[2].frame;
			pivot = jump.frames[2].pivot;
			cps = jump.frames[2].colliders;
		}
		else
		{
			aux = jump.frames[1].frame;
			pivot = jump.frames[1].pivot;
			cps = jump.frames[1].colliders;
		}

		break;


	case PlayerState_LowPunch:
		aux = L_punch.GetCurrentFrame();
		pivot = L_punch.GetCurrentPivot();
		cps = L_punch.GetCurrentCollider();
		L_punch.NextFrame();
		m_ColliderAttack.damageType = DamageType_LAttack;
		m_ColliderAttack.damage = 12;
		break;

	case PlayerState_LowKick:
		aux = L_kick.GetCurrentFrame();
		pivot = L_kick.GetCurrentPivot();
		cps = L_kick.GetCurrentCollider();
		L_kick.NextFrame();
		m_ColliderAttack.damageType = DamageType_LAttack;
		m_ColliderAttack.damage = 12;
		break;

	case PlayerState_MediumPunch:
		aux = M_punch.GetCurrentFrame();
		pivot = M_punch.GetCurrentPivot();
		cps = M_punch.GetCurrentCollider();
		M_punch.NextFrame();
		m_ColliderAttack.damageType = DamageType_MAttack;
		m_ColliderAttack.damage = 16;
		break;

	case PlayerState_MediumKick:
		aux = M_kick.GetCurrentFrame();
		pivot = M_kick.GetCurrentPivot();
		cps = M_kick.GetCurrentCollider();
		M_kick.NextFrame();
		m_ColliderAttack.damageType = DamageType_MAttack;
		m_ColliderAttack.damage = 16;
		break;

	case PlayerState_HighPunch:
		aux = H_punch.GetCurrentFrame();
		pivot = H_punch.GetCurrentPivot();
		cps = H_punch.GetCurrentCollider();
		H_punch.NextFrame();
		m_ColliderAttack.damageType = DamageType_HAttack;
		m_ColliderAttack.damage = 20;
		break;

	case PlayerState_HighKick:
		aux = H_kick.GetCurrentFrame();
		pivot = H_kick.GetCurrentPivot();
		cps = H_kick.GetCurrentCollider();
		H_kick.NextFrame();
		m_ColliderAttack.damageType = DamageType_HAttack;
		m_ColliderAttack.damage = 20;
		break;

	case PlayerState_ForwardLowPunch:
		aux = F_L_punch.GetCurrentFrame();
		pivot = F_L_punch.GetCurrentPivot();
		cps = F_L_punch.GetCurrentCollider();
		F_L_punch.NextFrame();
		m_ColliderAttack.damageType = DamageType_LAttack;
		m_ColliderAttack.damage = 14;
		break;

	case PlayerState_ForwardLowKick:
		aux = F_L_kick.GetCurrentFrame();
		pivot = F_L_kick.GetCurrentPivot();
		cps = F_L_kick.GetCurrentCollider();
		F_L_kick.NextFrame();
		m_ColliderAttack.damageType = DamageType_LAttack;
		m_ColliderAttack.damage = 14;
		break;

	case PlayerState_ForwardMediumPunch:
		aux = F_M_punch.GetCurrentFrame();
		pivot = F_M_punch.GetCurrentPivot();
		cps = F_M_punch.GetCurrentCollider();
		F_M_punch.NextFrame();
		m_ColliderAttack.damageType = DamageType_MAttack;
		m_ColliderAttack.damage = 18;
		break;

	case PlayerState_ForwardMediumKick:
		aux = F_M_kick.GetCurrentFrame();
		pivot = F_M_kick.GetCurrentPivot();
		cps = F_M_kick.GetCurrentCollider();
		F_M_kick.NextFrame();
		m_ColliderAttack.damageType = DamageType_MAttack;
		m_ColliderAttack.damage = 18;
		break;

	case PlayerState_ForwardHighPunch:
		aux = F_H_punch.GetCurrentFrame();
		pivot = F_H_punch.GetCurrentPivot();
		cps = F_H_punch.GetCurrentCollider();
		F_H_punch.NextFrame();
		m_ColliderAttack.damageType = DamageType_HAttack;
		m_ColliderAttack.damage = 22;
		break;

	case PlayerState_ForwardHighKick:
		aux = F_H_kick.GetCurrentFrame();
		pivot = F_H_kick.GetCurrentPivot();
		cps = F_H_kick.GetCurrentCollider();
		F_H_kick.NextFrame();
		m_ColliderAttack.damageType = DamageType_HAttack;
		m_ColliderAttack.damage = 22;
		break;

	case PlayerState_CrouchPunch:
		aux = crouch_punch.GetCurrentFrame();
		pivot = crouch_punch.GetCurrentPivot();
		cps = crouch_punch.GetCurrentCollider();
		crouch_punch.NextFrame();
		m_ColliderAttack.damageType = DamageType_MAttack;
		m_ColliderAttack.damage = 16;
		break;

	case PlayerState_CrouchKick:
		if (IsLookingRight())
		{
			MovePlayer(2);
		}
		else
		{
			MovePlayer(-2);
		}
		aux = crouch_kick.GetCurrentFrame();
		pivot = crouch_kick.GetCurrentPivot();
		cps = crouch_kick.GetCurrentCollider();
		crouch_kick.NextFrame();
		m_ColliderAttack.damageType = DamageType_MAttack;
		m_ColliderAttack.damage = 16;
		break;

	case PlayerState_JumpPunch:
		if (m_GoingUp && m_DistanceJumped < 105)
		{
			m_DistanceJumped += 3;
		}
		else if (m_GoingUp)
			m_GoingUp = false;
		else
		{
			m_DistanceJumped -= 3;
		}

		if (m_DirectionJump == DirectionJumping_Right)
		{
			MovePlayer(1);
		}
		else if (m_DirectionJump == DirectionJumping_Left)
		{
			MovePlayer(-1);
		}

		aux = jump_punch.GetCurrentFrame();
		pivot = jump_punch.GetCurrentPivot();
		cps = jump_punch.GetCurrentCollider();
		jump_punch.NextFrame();
		m_ColliderAttack.damageType = DamageType_MAttack;
		m_ColliderAttack.damage = 18;
		break;


	case PlayerState_JumpKick:
		if (m_GoingUp && m_DistanceJumped < 105)
		{
			m_DistanceJumped += 3;
		}
		else if (m_GoingUp)
			m_GoingUp = false;
		else
		{
			m_DistanceJumped -= 3;
		}

		if (m_DirectionJump == DirectionJumping_Right)
		{
			MovePlayer(1);
		}
		else if (m_DirectionJump == DirectionJumping_Left)
		{
			MovePlayer(-1);
		}

		aux = jump_kick.GetCurrentFrame();
		pivot = jump_kick.GetCurrentPivot();
		cps = jump_kick.GetCurrentCollider();
		jump_kick.NextFrame();
		m_ColliderAttack.damageType = DamageType_MAttack;
		m_ColliderAttack.damage = 18;
		break;

	case PlayerState_Hit:
		if (IsLookingLeft())
		{
			MovePlayer(1);
		}
		else
		{
			MovePlayer(-1);
		}
		aux = hit.GetCurrentFrame();
		pivot = hit.GetCurrentPivot();
		cps = hit.GetCurrentCollider();
		hit.NextFrame();
		break;

	case PlayerState_CrouchHit:
		if (IsLookingLeft())
		{
			MovePlayer(1);
		}
		else
		{
			MovePlayer(-1);
		}
		aux = crouch_hit.GetCurrentFrame();
		pivot = crouch_hit.GetCurrentPivot();
		cps = crouch_hit.GetCurrentCollider();
		crouch_hit.NextFrame();
		break;

	case PlayerState_FaceHit:
		if (IsLookingLeft())
		{
			MovePlayer(1);
		}
		else
		{
			MovePlayer(-1);
		}
		aux = face_hit.GetCurrentFrame();
		pivot = face_hit.GetCurrentPivot();
		cps = face_hit.GetCurrentCollider();
		face_hit.NextFrame();
		break;

	case PlayerState_Ko:
		if (m_DistanceJumped > 0)
		{
			m_DistanceJumped -= 3;
		}
		if (!ko.IsEnded()) {
			if (IsLookingRight())
				MovePlayer(-1);
			else
				MovePlayer(1);
		}
		aux = ko.GetCurrentFrame();
		pivot = ko.GetCurrentPivot();
		cps = ko.GetCurrentCollider();
		ko.NextFrame();

		break;

	case PlayerState_Win1:
		if (m_DistanceJumped > 0)
		{
			m_DistanceJumped -= 3;
		}
		aux = victory1.GetCurrentFrame();
		pivot = victory1.GetCurrentPivot();
		cps = victory1.GetCurrentCollider();
		victory1.NextFrame();
		break;

	case PlayerState_Win2:
		if (m_DistanceJumped > 0)
		{
			m_DistanceJumped -= 3;
		}
		aux = victory2.GetCurrentFrame();
		pivot = victory2.GetCurrentPivot();
		cps = victory2.GetCurrentCollider();
		victory2.NextFrame();
		break;

	case PlayerState_TimeOut:
		if (m_DistanceJumped > 0)
		{
			m_DistanceJumped -= 3;
		}
		aux = time_out.GetCurrentFrame();
		pivot = time_out.GetCurrentPivot();
		cps = time_out.GetCurrentCollider();
		time_out.NextFrame();
		break;

	case PlayerState_YogaFire:
		aux = yoga_fire.GetCurrentFrame();
		pivot = yoga_fire.GetCurrentPivot();
		cps = yoga_fire.GetCurrentCollider();
		yoga_fire.NextFrame();
		break;

	case PlayerState_YogaFlame:
		aux = yoga_flame.GetCurrentFrame();
		pivot = yoga_flame.GetCurrentPivot();
		cps = yoga_flame.GetCurrentCollider();
		yoga_flame.NextFrame();
		m_ColliderAttack.damageType = DamageType_MAttack;
		m_ColliderAttack.damage = 28;
		break;

	case PlayerState_YogaMummy:

		m_DistanceJumped -= 3;
		if (m_DirectionMummy == DirectionJumping_Right)
		{
			MovePlayer(2);
		}
		else if (m_DirectionMummy == DirectionJumping_Left)
		{
			MovePlayer(-2);
		}

		aux = yoga_mummy.GetCurrentFrame();
		pivot = yoga_mummy.GetCurrentPivot();
		cps = yoga_mummy.GetCurrentCollider();
		yoga_mummy.NextFrame();
		m_ColliderAttack.damageType = DamageType_MAttack;
		m_ColliderAttack.damage = 18;
		break;

	case PlayerState_YogaSpear:

		m_DistanceJumped -= 3;
		if (m_DirectionMummy == DirectionJumping_Right)
		{
			MovePlayer(2);
		}
		else if (m_DirectionMummy == DirectionJumping_Left)
		{
			MovePlayer(-2);
		}

		aux = yoga_spear.GetCurrentFrame();
		pivot = yoga_spear.GetCurrentPivot();
		cps = yoga_spear.GetCurrentCollider();
		yoga_spear.NextFrame();
		m_ColliderAttack.damageType = DamageType_MAttack;
		m_ColliderAttack.damage = 18;
		break;

	case PlayerState_Blocking:
		aux = block.GetCurrentFrame();
		pivot = block.GetCurrentPivot();
		cps = block.GetCurrentCollider();
		block.NextFrame();
		break;

	case PlayerState_CrouchBlocking:
		aux = crouch_block.GetCurrentFrame();
		pivot = crouch_block.GetCurrentPivot();
		cps = crouch_block.GetCurrentCollider();
		crouch_block.NextFrame();
		break;

	case PlayerState_BlockingHitted:
		if (IsLookingLeft())
		{
			MovePlayer(2);
		}
		else
		{
			MovePlayer(-2);
		}
		aux = block.GetCurrentFrame();
		pivot = block.GetCurrentPivot();
		cps = block.GetCurrentCollider();
		block.NextFrame();
		break;

	case PlayerState_CrouchBlockingHitted:
		if (IsLookingLeft())
		{
			MovePlayer(1);
		}
		else
		{
			MovePlayer(-1);
		}
		aux = crouch_block.GetCurrentFrame();
		pivot = crouch_block.GetCurrentPivot();
		cps = crouch_block.GetCurrentCollider();
		crouch_block.NextFrame();
		break;

	case PlayerState_AirHitted:
		if (IsLookingLeft())
		{
			MovePlayer(2);
			m_DirectionJump = DirectionJumping_Right;
		}
		else
		{
			MovePlayer(-2);
			m_DirectionJump = DirectionJumping_Left;
		}
		m_DistanceJumped += 3;
		m_GoingUp = true;
		aux = air_hit.GetCurrentFrame();
		pivot = air_hit.GetCurrentPivot();
		cps = air_hit.GetCurrentCollider();
		air_hit.NextFrame();
		break;

	}

	m_PlayerCollider.rect = cps.Position_collider;
	m_ColliderHead.rect = cps.Collider_head;
	m_ColliderBody.rect = cps.Collider_body;
	m_ColliderLegs.rect = cps.Collider_legs;
	m_ColliderAttack.rect = cps.Collider_attack;

	if (IsLookingRight())
	{
		m_PlayerCollider.rect.x += m_Position.x;
		m_PlayerCollider.rect.y += m_Position.y - m_DistanceJumped;
		m_ColliderHead.rect.x += m_Position.x;
		m_ColliderHead.rect.y += m_Position.y - m_DistanceJumped;
		m_ColliderBody.rect.x += m_Position.x;
		m_ColliderBody.rect.y += m_Position.y - m_DistanceJumped;
		m_ColliderLegs.rect.x += m_Position.x;
		m_ColliderLegs.rect.y += m_Position.y - m_DistanceJumped;
		m_ColliderAttack.rect.x += m_Position.x;
		m_ColliderAttack.rect.y += m_Position.y - m_DistanceJumped;
		App->renderer->Blit(m_Graphics, m_Position.x - pivot, m_Position.y - aux.h - m_DistanceJumped, &(aux), 1.0f, SDL_FLIP_NONE);
	}
	else
	{
		m_PlayerCollider.rect.x = m_Position.x - m_PlayerCollider.rect.x - m_PlayerCollider.rect.w;
		m_PlayerCollider.rect.y += m_Position.y - m_DistanceJumped;
		m_ColliderHead.rect.x = m_Position.x - m_ColliderHead.rect.x - m_ColliderHead.rect.w;
		m_ColliderHead.rect.y += m_Position.y - m_DistanceJumped;
		m_ColliderBody.rect.x = m_Position.x - m_ColliderBody.rect.x - m_ColliderBody.rect.w;
		m_ColliderBody.rect.y += m_Position.y - m_DistanceJumped;
		m_ColliderLegs.rect.x = m_Position.x - m_ColliderLegs.rect.x - m_ColliderLegs.rect.w;
		m_ColliderLegs.rect.y += m_Position.y - m_DistanceJumped;
		m_ColliderAttack.rect.x = m_Position.x - m_ColliderAttack.rect.x - m_ColliderAttack.rect.w;
		m_ColliderAttack.rect.y += m_Position.y - m_DistanceJumped;
		App->renderer->Blit(m_Graphics, m_Position.x - aux.w + pivot, m_Position.y - aux.h - m_DistanceJumped, &(aux), 1.0f, SDL_FLIP_HORIZONTAL);
	}
	
	return UpdateStatus_Continue;
}

// PostUpdate
UpdateStatus ModulePlayerDhalsim::PostUpdate()
{
	m_AreCollidingPlayers = false;
	return UpdateStatus_Continue;
}

// On Collision
void ModulePlayerDhalsim::OnCollision(Collider* c1, Collider* c2)
{
	if (!m_Win && !m_Dead && !m_Time0){
		if (c1->type == ColliderType_BodyPlayerOne && c2->type == ColliderType_BodyPlayerTwo)
			m_AreCollidingPlayers = true;
		else if (c1->type == ColliderType_BodyPlayerTwo && c2->type == ColliderType_BodyPlayerOne)
			m_AreCollidingPlayers = true;
		else if (c1->type == ColliderType_PlayerOne && c2->type == ColliderType_AttackPlayerTwo && c2->rect.w * c2->rect.h != 0 && !m_AlreadyHitted)
		{
			if (!m_Hitted && m_PlayerState != PlayerState_Blocking && m_PlayerState != PlayerState_CrouchBlocking && m_PlayerState != PlayerState_BlockingHitted && m_PlayerState != PlayerState_CrouchBlockingHitted)
			{
				m_Life -= c2->damage;
				m_AlreadyHitted = true;
			}
			else if ((m_OtherPlayer->GetPlayerState() == PlayerState_CrouchPunch || m_OtherPlayer->GetPlayerState() == PlayerState_CrouchKick) && m_PlayerState != PlayerState_CrouchBlocking && m_PlayerState != PlayerState_CrouchBlockingHitted)
			{
				m_Life -= c2->damage;
				m_AlreadyHitted = true;
				m_LegHitted = true;
			}

			if (m_Life < 0)
			{
				App->scene_bison->RestartScene(m_OtherPlayer->m_Wins + 1);
				App->audio->PlayFx(m_AudioIdDead);
				m_Dead = true;
				m_Life = 0;
			}
			else if (!m_Hitted)
			{
				if (c2->damageType == DamageType_LAttack)
					App->audio->PlayFx(m_AudioIdLImpact);
				else if (c2->damageType == DamageType_MAttack)
					App->audio->PlayFx(m_AudioIdMImpact);
				else if (c2->damageType == DamageType_HAttack)
					App->audio->PlayFx(m_AudioIdHImpact);
			}

			m_Hitted = true;
			if (c1 == &m_ColliderHead)
				m_HeadHitted = true;
		}
		else if (c1->type == ColliderType_PlayerTwo && c2->type == ColliderType_AttackPlayerOne  && c2->rect.w * c2->rect.h != 0 && !m_AlreadyHitted)
		{
			if (!m_Hitted && m_PlayerState != PlayerState_Blocking && m_PlayerState != PlayerState_CrouchBlocking && m_PlayerState != PlayerState_BlockingHitted && m_PlayerState != PlayerState_CrouchBlockingHitted)
			{
				m_Life -= c2->damage;
				m_AlreadyHitted = true;
			}
			else if ((m_OtherPlayer->GetPlayerState() == PlayerState_CrouchPunch || m_OtherPlayer->GetPlayerState() == PlayerState_CrouchKick) && m_PlayerState != PlayerState_CrouchBlocking && m_PlayerState != PlayerState_CrouchBlockingHitted)
			{
				m_Life -= c2->damage;
				m_AlreadyHitted = true;
				m_LegHitted = true;
			}


			if (m_Life < 0)
			{
				App->scene_bison->RestartScene(m_OtherPlayer->m_Wins + 1);
				App->audio->PlayFx(m_AudioIdDead);
				m_Dead = true;
				m_Life = 0;
			}
			else if (!m_Hitted)
			{
				if (c2->damageType == DamageType_LAttack)
					App->audio->PlayFx(m_AudioIdLImpact);
				else if (c2->damageType == DamageType_MAttack)
					App->audio->PlayFx(m_AudioIdMImpact);
				else if (c2->damageType == DamageType_HAttack)
					App->audio->PlayFx(m_AudioIdHImpact);
			}


			m_Hitted = true;
			if (c1 == &m_ColliderHead)
				m_HeadHitted = true;
		}
		else if ((c1->type == ColliderType_PlayerOne || c1->type == ColliderType_AttackPlayerOne) && c2->type == ColliderType_Particles && c2->rect.w * c2->rect.h != 0)
		{
			if (!m_Hitted && m_PlayerState != PlayerState_Blocking && m_PlayerState != PlayerState_CrouchBlocking && m_PlayerState != PlayerState_BlockingHitted && m_PlayerState != PlayerState_CrouchBlockingHitted)
			{
				m_Life -= 18;
				App->audio->PlayFx(m_AudioIdLImpact);
			}

			if (m_Life < 0)
			{
				App->scene_bison->RestartScene(m_OtherPlayer->m_Wins + 1);
				App->audio->PlayFx(m_AudioIdDead);
				m_Dead = true;
				m_Life = 0;
			}

			m_Hitted = true;
			if (c1 == &m_ColliderHead)
				m_HeadHitted = true;
		}
		else if ((c1->type == ColliderType_PlayerTwo || c1->type == ColliderType_AttackPlayerTwo) && c2->type == ColliderType_Particles && c2->rect.w * c2->rect.h != 0)
		{
			if (!m_Hitted && m_PlayerState != PlayerState_Blocking && m_PlayerState != PlayerState_CrouchBlocking && m_PlayerState != PlayerState_BlockingHitted && m_PlayerState != PlayerState_CrouchBlockingHitted)
			{
				m_Life -= 18;
				App->audio->PlayFx(m_AudioIdLImpact);
			}

			if (m_Life < 0)
			{
				App->scene_bison->RestartScene(m_OtherPlayer->m_Wins + 1);
				App->audio->PlayFx(m_AudioIdDead);
				m_Dead = true;
				m_Life = 0;
			}

			m_Hitted = true;
			if (c1 == &m_ColliderHead)
				m_HeadHitted = true;
		}
	}
}

// Returns true if the player is the limit of the camera
bool ModulePlayerDhalsim::IsPlayerInCameraLimit() const
{
	if (App->renderer->camera.x <= -((m_PlayerCollider.rect.x - m_PlayerCollider.rect.w)*SCREEN_SIZE))
		return true;
	if (App->renderer->camera.x - App->renderer->camera.w >= -((m_PlayerCollider.rect.x + m_PlayerCollider.rect.w * 2)*SCREEN_SIZE))
		return true;
	return false;
}

// Moves horizontally the player, it can move the camera
void ModulePlayerDhalsim::MovePlayer(int distance)
{
	if (distance > 0)
	{
		if (!App->renderer->ScreenRightLimit())
		{
			if (!m_OtherPlayer->IsPlayerInCameraLimit())
			{
				m_Position.x += distance;
				if (App->renderer->camera.x - App->renderer->camera.w >= -((m_PlayerCollider.rect.x + m_PlayerCollider.rect.w*2)*SCREEN_SIZE))
					App->renderer->camera.x -= SCREEN_SIZE;
			}
			else
			{
				if (App->renderer->camera.x - App->renderer->camera.w < -((m_PlayerCollider.rect.x + m_PlayerCollider.rect.w*2)*SCREEN_SIZE))
					m_Position.x += distance;
			}
		}
		else
		{
			if (App->renderer->camera.x - App->renderer->camera.w < -((m_PlayerCollider.rect.x + m_PlayerCollider.rect.w*2)*SCREEN_SIZE))
				m_Position.x += distance;
		}
	}
	else
	{
		if (!App->renderer->ScreenLeftLimit())
		{
			if (!m_OtherPlayer->IsPlayerInCameraLimit())
			{
				m_Position.x += distance;
				if (App->renderer->camera.x <= -(m_PlayerCollider.rect.x*SCREEN_SIZE) + (m_PlayerCollider.rect.w*SCREEN_SIZE))
					App->renderer->camera.x += SCREEN_SIZE;
			}
			else
			{
				if (App->renderer->camera.x > -(m_PlayerCollider.rect.x*SCREEN_SIZE) + (m_PlayerCollider.rect.w*SCREEN_SIZE))
					m_Position.x += distance;
			}
		}
		else
		{
			if (App->renderer->camera.x > -(m_PlayerCollider.rect.x*SCREEN_SIZE) + (m_PlayerCollider.rect.w*SCREEN_SIZE))
				m_Position.x += distance;
		}
	}
}

// Resets the player states
void ModulePlayerDhalsim::restartPlayer(bool everything)
{

	if (m_PlayerID == PlayerID_One)
	{
		m_Position.x = 150;
		m_Position.y = 200;
	}
	else
	{
		m_Position.x = 300;
		m_Position.y = 200;
	}

	if (everything)
		m_Wins = 0;

	m_PlayerState = PlayerState_Idle;
	m_AreCollidingPlayers = false;
	m_Hitted = false;
	m_HeadHitted = false;
	m_Life = 200;
	m_Win = false;
	m_Dead = false;
	m_Time0 = false;
	m_StartingCombo = ComboTypes_ComboNothing;

	ko.RestartFrames();
	victory1.RestartFrames();
	victory2.RestartFrames();

	m_DistanceJumped = 0;
	m_JumpAttacked = false;
	m_LegHitted = false;
	m_AlreadyHitted = false;
}

// Get and Set variables

iPoint ModulePlayerDhalsim::GetPosition() const
{
	return m_Position;
}

bool ModulePlayerDhalsim::IsAttacking() const
{
	if (m_PlayerState == PlayerState_LowPunch ||
		m_PlayerState == PlayerState_LowKick ||
		m_PlayerState == PlayerState_MediumPunch ||
		m_PlayerState == PlayerState_MediumKick ||
		m_PlayerState == PlayerState_HighPunch ||
		m_PlayerState == PlayerState_HighKick ||
		m_PlayerState == PlayerState_CrouchPunch ||
		m_PlayerState == PlayerState_CrouchKick ||
		m_PlayerState == PlayerState_JumpPunch ||
		m_PlayerState == PlayerState_JumpKick ||
		m_PlayerState == PlayerState_ForwardLowPunch ||
		m_PlayerState == PlayerState_ForwardLowKick ||
		m_PlayerState == PlayerState_ForwardMediumPunch ||
		m_PlayerState == PlayerState_ForwardMediumKick ||
		m_PlayerState == PlayerState_ForwardHighPunch ||
		m_PlayerState == PlayerState_ForwardHighKick ||
		m_PlayerState == PlayerState_YogaFire ||
		m_PlayerState == PlayerState_YogaFlame ||
		m_PlayerState == PlayerState_YogaMummy ||
		m_PlayerState == PlayerState_YogaSpear)
	{
		return true;
	}
	else if (App->particles->GetNumberParticles() > 0)
	{
		if (m_OtherPlayer->GetLookingDirection() == LookingDirection_Right)
		{
			for (int i = 0; i < App->particles->GetNumberParticles(); ++i)
			{
				if (App->particles->GetParticlePosition(i).x > m_OtherPlayer->m_Position.x && App->particles->GetParticleSpeed(i) < 0)
					return true;
			}
		}
		else
		{
			for (int i = 0; i < App->particles->GetNumberParticles(); ++i)
			{
				if (App->particles->GetParticlePosition(i).x < m_OtherPlayer->m_Position.x && App->particles->GetParticleSpeed(i) > 0)
					return true;
			}
		}
	}
	return false;
}

bool ModulePlayerDhalsim::GetPlayerInput(InputType actionKey) const
{
	if (m_PlayerID == PlayerID_One)
	{
		switch (actionKey)
		{
		case InputType_Up:
			return App->input->GetKey(SDL_SCANCODE_I) == KEY_REPEAT;
			break;
		case InputType_Down:
			return App->input->GetKey(SDL_SCANCODE_K) == KEY_REPEAT;
			break;
		case InputType_Left:
			return App->input->GetKey(SDL_SCANCODE_J) == KEY_REPEAT;
			break;
		case InputType_Right:
			return App->input->GetKey(SDL_SCANCODE_L) == KEY_REPEAT;
			break;
		case InputType_LPunch:
			return App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN;
			break;
		case InputType_LKick:
			return App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN;
			break;
		case InputType_MPunch:
			return App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN;
			break;
		case InputType_MKick:
			return App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN;
			break;
		case InputType_HPunch:
			return App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN;
			break;
		case InputType_HKick:
			return App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN;
			break;
		}
	}
	else
	{
		switch (actionKey)
		{
		case InputType_Up:
			return (App->input->GetButton(SDL_CONTROLLER_AXIS_LEFTX) == KEY_REPEAT || App->input->yDir == -1 || App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT);
			break;
		case InputType_Down:
			return (App->input->GetButton(SDL_CONTROLLER_AXIS_LEFTY) == KEY_REPEAT || App->input->yDir == 1 || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT);
			break;
		case InputType_Left:
			return (App->input->GetButton(SDL_CONTROLLER_AXIS_RIGHTX) == KEY_REPEAT || App->input->xDir == -1 || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT);
			break;
		case InputType_Right:
			return (App->input->GetButton(SDL_CONTROLLER_AXIS_RIGHTY) == KEY_REPEAT || App->input->xDir == 1 || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT);
			break;
		case InputType_LPunch:
			return (App->input->GetButton(12) == KEY_DOWN) || (App->input->GetKey(SDL_SCANCODE_KP_4) == KEY_DOWN);
			break;
		case InputType_LKick:
			return (App->input->GetButton(10) == KEY_DOWN) || (App->input->GetKey(SDL_SCANCODE_KP_7) == KEY_DOWN);
			break;
		case InputType_MPunch:
			return (App->input->GetButton(11) == KEY_DOWN) || (App->input->GetKey(SDL_SCANCODE_KP_5) == KEY_DOWN);
			break;
		case InputType_MKick:
			return (App->input->GetButton(13) == KEY_DOWN) || (App->input->GetKey(SDL_SCANCODE_KP_8) == KEY_DOWN);
			break;
		case InputType_HPunch:
			return (App->input->GetButton(8) == KEY_DOWN) || (App->input->GetKey(SDL_SCANCODE_KP_6) == KEY_DOWN);
			break;
		case InputType_HKick:
			return (App->input->GetButton(9) == KEY_DOWN) || (App->input->GetKey(SDL_SCANCODE_KP_9) == KEY_DOWN);
			break;
		}
	}
	return false;
}

void ModulePlayerDhalsim::SetSDLRectFromData(SDL_Rect& sdl_rect, const Json::Value& jsonValue)
{
	sdl_rect.x = jsonValue[0].asInt();
	sdl_rect.y = jsonValue[1].asInt();
	sdl_rect.w = jsonValue[2].asInt();
	sdl_rect.h = jsonValue[3].asInt();
}

void ModulePlayerDhalsim::SetPlayerAnimationDataFromJSON(Animation& animation, Json::Value& jsonValue)
{
	int numberOfFrames = jsonValue.get("numberOfFrames", "0").asInt();

	for (int i = 0; i < numberOfFrames; ++i)
	{
		Json::Value currentValue = jsonValue["frames"][i];
		AnimationStructure animationStructure;

		SetSDLRectFromData(animationStructure.frame, currentValue["frame"]);

		animationStructure.pivot = currentValue["pivot"].asInt();

		SetSDLRectFromData(animationStructure.colliders.Position_collider, currentValue["position-collider"]);
		SetSDLRectFromData(animationStructure.colliders.Collider_head, currentValue["head-collider"]);
		SetSDLRectFromData(animationStructure.colliders.Collider_body, currentValue["body-collider"]);
		SetSDLRectFromData(animationStructure.colliders.Collider_legs, currentValue["legs-collider"]);
		SetSDLRectFromData(animationStructure.colliders.Collider_attack, currentValue["attack-collider"]);

		animationStructure.duration = currentValue["duration"].asInt();

		animation.frames.push_back(animationStructure);
		animation.loop = jsonValue.get("loop", true).asBool();
	}
}

void ModulePlayerDhalsim::SetParticleAnimationDataFromJSON(ParticleAnimation& particleAnimation, Json::Value& jsonValue)
{
	int numberOfFrames = jsonValue.get("numberOfFrames", "0").asInt();

	for (int i = 0; i < numberOfFrames; ++i)
	{
		Json::Value currentValue = jsonValue["frames"][i];
		ParticleAnimationStructure particleAnimationStructure;

		SetSDLRectFromData(particleAnimationStructure.frame, currentValue["frame"]);

		particleAnimationStructure.pivot = currentValue["pivot"].asInt();

		SetSDLRectFromData(particleAnimationStructure.collider, currentValue["collider"]);

		particleAnimationStructure.duration = currentValue["duration"].asInt();

		particleAnimation.frames.push_back(particleAnimationStructure);
		particleAnimation.loop = jsonValue.get("loop", true).asBool();
	}
}

int ModulePlayerDhalsim::GetWins() const
{
	return m_Wins;
}

void ModulePlayerDhalsim::SetWins(int wins)
{
	m_Wins = wins;
}

int ModulePlayerDhalsim::GetLife() const
{
	return m_Life;
}

void ModulePlayerDhalsim::SetLife(int life)
{
	m_Life = life;
}

void ModulePlayerDhalsim::DecreseLife(int life)
{
	m_Life -= life;
}

bool ModulePlayerDhalsim::IsWin() const
{
	return m_Win;
}

void ModulePlayerDhalsim::SetWin(bool win)
{
	m_Win = win;
}

bool ModulePlayerDhalsim::IsDead() const
{
	return m_Dead;
}

void ModulePlayerDhalsim::SetDead(bool dead)
{
	m_Dead = dead;
}

bool ModulePlayerDhalsim::IsTime0() const
{
	return m_Time0;
}

void ModulePlayerDhalsim::SetTime0(bool time_0)
{
	m_Time0 = time_0;
}

bool ModulePlayerDhalsim::IsJumping() const
{
	// TODO: only considering if the player is jumping normally (not combos, not hits)
	return m_PlayerState == PlayerState_Jumping;
}

bool ModulePlayerDhalsim::IsMovingBack() const
{
	bool isMovingBack = false;
	isMovingBack = isMovingBack || (GetPlayerInput(InputType_Left) && IsLookingRight());
	isMovingBack = isMovingBack || (GetPlayerInput(InputType_Right) && IsLookingLeft());
	return isMovingBack;
}

bool ModulePlayerDhalsim::IsMovingForward() const
{
	bool isMovingForward = false;
	isMovingForward = isMovingForward || (GetPlayerInput(InputType_Right) && IsLookingRight());
	isMovingForward = isMovingForward || (GetPlayerInput(InputType_Left) && IsLookingLeft());
	return isMovingForward;
}

bool ModulePlayerDhalsim::IsPunchInput() const
{
	bool isPunchInput = false;
	isPunchInput = isPunchInput || GetPlayerInput(InputType_LPunch);
	isPunchInput = isPunchInput || GetPlayerInput(InputType_MPunch);
	isPunchInput = isPunchInput || GetPlayerInput(InputType_HPunch);
	return isPunchInput;
}

bool ModulePlayerDhalsim::IsKickInput() const
{
	bool isKickInput = false;
	isKickInput = isKickInput || GetPlayerInput(InputType_LKick);
	isKickInput = isKickInput || GetPlayerInput(InputType_MKick);
	isKickInput = isKickInput || GetPlayerInput(InputType_HKick);
	return isKickInput;
}

LookingDirection ModulePlayerDhalsim::GetLookingDirection() const
{
	return m_LookingDirection;
}

bool ModulePlayerDhalsim::IsLookingRight() const
{
	return m_LookingDirection == LookingDirection_Right;
}

bool ModulePlayerDhalsim::IsLookingLeft() const
{
	return m_LookingDirection == LookingDirection_Left;
}

void ModulePlayerDhalsim::SetLookingRight(LookingDirection lookingDirection)
{
	m_LookingDirection = lookingDirection;
}

PlayerState ModulePlayerDhalsim::GetPlayerState() const
{
	return m_PlayerState;
}

void ModulePlayerDhalsim::SetPlayerState(PlayerState playerState)
{
	m_PlayerState = playerState;
}

ComboTypes ModulePlayerDhalsim::GetStartingCombo() const
{
	return m_StartingCombo;
}

void ModulePlayerDhalsim::SetStartingCombo(ComboTypes starting_combo)
{
	m_StartingCombo = starting_combo;
}

int ModulePlayerDhalsim::GetDistanceJumped() const
{
	return m_DistanceJumped;
}
