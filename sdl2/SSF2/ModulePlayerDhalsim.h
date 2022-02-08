#ifndef __MODULEPLAYERDHALSIM_H__
#define __MODULEPLAYERDHALSIM_H__

#include "Module.h"
#include "Animation.h"
#include "ParticleAnimation.h"
#include "Globals.h"
#include "Point.h"
#include "ModuleCollisions.h"
#include <json/json.h>
#include "ModuleComboDetection.h"

#include <memory>

struct SDL_Texture;

class ModulePlayerDhalsim : public Module
{
public:
	ModulePlayerDhalsim(PlayerID playerID, bool start_enabled = true);
	virtual ~ModulePlayerDhalsim();

	virtual bool Start();
	virtual UpdateStatus PreUpdate();
	virtual UpdateStatus Update();
	virtual UpdateStatus PostUpdate();
	virtual bool CleanUp();

	bool IsAttacking() const;

	iPoint GetPosition() const;
	void OnCollision(Collider* c1, Collider* c2);

	virtual void restartPlayer(bool everything);
	void MovePlayer(int distance);
	bool IsPlayerInCameraLimit() const;

	virtual bool GetPlayerInput(InputType actionKey) const;

	int GetWins() const;
	void SetWins(int wins);

	int GetLife() const;
	void SetLife(int life);
	void DecreseLife(int life);

	bool IsWin() const;
	void SetWin(bool win);

	bool IsDead() const;
	void SetDead(bool dead);

	bool IsTime0() const;
	void SetTime0(bool time_0);

	bool IsJumping() const;

	bool IsMovingBack() const;
	bool IsMovingForward() const;

	bool IsPunchInput() const;
	bool IsKickInput() const;

	LookingDirection GetLookingDirection() const;
	void SetLookingRight(LookingDirection lookingDirection);
	bool IsLookingRight() const;
	bool IsLookingLeft() const;

	int GetDistanceJumped() const;

	PlayerState GetPlayerState() const;
	void SetPlayerState(PlayerState playerState);

	ComboTypes GetStartingCombo() const;
	void SetStartingCombo(ComboTypes starting_combo);


private:
	void SetUpAnimations();
	void SetUpPlayer();

	void SetSDLRectFromData(SDL_Rect& sdl_rect, const Json::Value& jsonValue);
	void SetPlayerAnimationDataFromJSON(Animation& animation, Json::Value& jsonValue);
	void SetParticleAnimationDataFromJSON(ParticleAnimation& particleAnimation, Json::Value& jsonValue);

	void TemporalFunction(ofstream& myfile, string s, Animation& animation);

	void AddColliders();
	void LoadAudioSounds();
	void DecideLookingPosition();

protected:
	SDL_Texture* m_Graphics = nullptr;
	iPoint m_Position;
	bool m_IsMovingRight;
	bool m_IsMovingLeft;
	Collider m_ColliderHead;
	Collider m_ColliderBody;
	Collider m_ColliderLegs;
	Collider m_ColliderAttack;
	Collider m_PlayerCollider;

	ParticleAnimation m_Particle;
	ParticleAnimation m_DestroyParticle;

	bool m_AreCollidingPlayers;
	Uint32 m_LastShotTimer;

	Animation idle;
	Animation forward;
	Animation backward;
	Animation crouching;
	Animation block;
	Animation crouch_block;
	Animation jump;
	Animation L_punch;
	Animation L_kick;
	Animation M_punch;
	Animation M_kick;
	Animation H_punch;
	Animation H_kick;
	Animation F_L_punch;
	Animation F_M_punch;
	Animation F_H_punch;
	Animation F_L_kick;
	Animation F_M_kick;
	Animation F_H_kick;
	Animation crouch_punch;
	Animation crouch_kick;
	Animation jump_punch;
	Animation jump_kick;
	Animation hit;
	Animation face_hit;
	Animation crouch_hit;
	Animation air_hit;
	Animation ko;
	Animation victory1;
	Animation victory2;
	Animation time_out;
	Animation yoga_fire;
	Animation yoga_flame;
	Animation yoga_mummy;
	Animation yoga_spear;

	bool m_Hitted;
	bool m_HeadHitted;
	bool m_LegHitted;
	bool m_AlreadyHitted;

	int m_DistanceJumped;
	bool m_GoingUp;
	bool m_JumpAttacked;

	int m_AudioIdYogaFire;
	int m_AudioIdYogaFlame;
	int m_AudioIdDead;
	int m_AudioIdLAttack;
	int m_AudioIdMAttack;
	int m_AudioIdHAttack;
	int m_AudioIdLImpact;
	int m_AudioIdMImpact;
	int m_AudioIdHImpact;

	DamageType m_DamageType;

	DirectionJumping m_DirectionJump;
	DirectionJumping m_DirectionMummy;

	ModulePlayerDhalsim* m_OtherPlayer;

	int m_Wins;
	int m_Life;
	bool m_Win;
	bool m_Dead;
	bool m_Time0;

	LookingDirection m_LookingDirection;

	PlayerState m_PlayerState;
	ComboTypes m_StartingCombo;

	PlayerID m_PlayerID;

	ModuleComboDetection* m_ModuleComboDetection;
};

#endif //Dhalsim

