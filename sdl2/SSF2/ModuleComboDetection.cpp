#include "Application.h"
#include "ModuleInput.h"
#include "ModuleComboDetection.h"
#include "ModulePlayerDhalsim.h"
#include "SDL2/SDL.h"

ModuleComboDetection::ModuleComboDetection(PlayerID playerID, bool start_enabled)
	: Module(start_enabled),
	m_PreviousState(PreviousComboState_Nothing),
	m_TypeOfCombo(ComboTypes_ComboNothing),
	m_PreviousStateCountdown(0),
	m_FirstStateCountdown(0),
	m_IsAirCombo(false)
{
	m_PlayerID = playerID;
}

ModuleComboDetection::~ModuleComboDetection()
{

}

bool ModuleComboDetection::Start()
{
	if (m_PlayerID == PlayerID_One)
	{
		m_ModulePlayerDhalsim = App->player_one;
	}
	else
	{
		m_ModulePlayerDhalsim = App->player_two;
	}

	return true;
}

UpdateStatus ModuleComboDetection::PreUpdate()
{
	UpdateCounters();
	UpdateComboState();

	return UpdateStatus_Continue;
}

void ModuleComboDetection::UpdateCounters()
{
	--m_PreviousStateCountdown;
	--m_FirstStateCountdown;
}

void ModuleComboDetection::UpdateComboState()
{
	switch (m_PreviousState)
	{
		case PreviousComboState_Nothing:
			UpdateComboStateNothing();
			break;

		case PreviousComboState_Back:
			UpdateComboStateBack();
			break;

		case PreviousComboState_DownBack:
			UpdateComboStateDownBack();
			break;

		case PreviousComboState_Down:
			UpdateComboStateDown();
			break;

		case PreviousComboState_DownFront:
			UpdateComboStateDownFront();
			break;

		case PreviousComboState_Front:
			UpdateComboStateFront();
			break;
	}
}

void ModuleComboDetection::UpdateComboStateNothing()
{
	m_IsAirCombo = m_ModulePlayerDhalsim->IsJumping();

	if (m_ModulePlayerDhalsim->GetPlayerInput(InputType_Down))
	{
		// Change magic numbers
		SetStartingComboParameters(PreviousComboState_Down, ComboTypes_ComboYogaFire);
	}

	if (m_ModulePlayerDhalsim->GetPlayerInput(InputType_Down) && m_ModulePlayerDhalsim->IsJumping() && m_ModulePlayerDhalsim->IsMovingBack())
	{
		SetStartingComboParameters(PreviousComboState_DownBack, ComboTypes_AerialCombo);
	}

	if (m_ModulePlayerDhalsim->IsMovingBack())
	{
		SetStartingComboParameters(PreviousComboState_Back, ComboTypes_ComboYogaFlame);
	}
}

void ModuleComboDetection::UpdateComboStateBack()
{
	if (IsCountdownReached())
	{
		RestartCombo();
	}
	else if (m_ModulePlayerDhalsim->GetPlayerInput(InputType_Down) && m_ModulePlayerDhalsim->IsMovingBack())
	{
		m_PreviousState = PreviousComboState_DownBack;
		StartPreviousStateCountdown();
	}
	else if (m_ModulePlayerDhalsim->IsMovingBack())
	{
		StartPreviousStateCountdown();
	}
}

void ModuleComboDetection::UpdateComboStateDownBack()
{
	if (IsCountdownReached())
	{
		RestartCombo();
	}
	else if (m_ModulePlayerDhalsim->GetPlayerInput(InputType_Down) && m_ModulePlayerDhalsim->IsMovingBack())
	{
		StartPreviousStateCountdown();
	}
	else if ((m_ModulePlayerDhalsim->GetPlayerInput(InputType_Down)))
	{
		m_PreviousState = PreviousComboState_Down;
		StartPreviousStateCountdown();
	}
}

void ModuleComboDetection::UpdateComboStateDown()
{
	if (IsCountdownReached())
	{
		RestartCombo();
	}
	else if (m_ModulePlayerDhalsim->GetPlayerInput(InputType_Down) && m_ModulePlayerDhalsim->IsMovingForward())
	{
		m_PreviousState = PreviousComboState_DownFront;
		StartPreviousStateCountdown();
	}
	else if (m_ModulePlayerDhalsim->GetPlayerInput(InputType_Down))
	{
		StartPreviousStateCountdown();
		StartFirstStateCountdown();
	}
}

void ModuleComboDetection::UpdateComboStateDownFront()
{
	if (IsCountdownReached())
	{
		RestartCombo();
	}
	else if (IsAerialComboPossible() && m_ModulePlayerDhalsim->IsPunchInput())
	{
		m_ModulePlayerDhalsim->SetStartingCombo(ComboTypes_AerialComboPunch);
		RestartCombo();
	}
	else if (IsAerialComboPossible() && m_ModulePlayerDhalsim->IsKickInput())
	{
		m_ModulePlayerDhalsim->SetStartingCombo(ComboTypes_AerialComboKick);
		RestartCombo();
	}
	else if (m_ModulePlayerDhalsim->GetPlayerInput(InputType_Down) && m_ModulePlayerDhalsim->IsMovingForward())
	{
		StartPreviousStateCountdown();
	}
	else if (m_ModulePlayerDhalsim->IsMovingForward())
	{
		m_PreviousState = PreviousComboState_Front;
		StartPreviousStateCountdown();
	}
}

void ModuleComboDetection::UpdateComboStateFront()
{
	if (IsCountdownReached())
	{
		RestartCombo();
	}
	else if (m_ModulePlayerDhalsim->IsPunchInput())
	{
		if (IsAerialComboPossible())
		{
			m_ModulePlayerDhalsim->SetStartingCombo(ComboTypes_AerialComboPunch);
		}
		else if (m_TypeOfCombo == ComboTypes_ComboYogaFire)
		{
			m_ModulePlayerDhalsim->SetStartingCombo(ComboTypes_ComboYogaFire);
		}
		else if (m_TypeOfCombo == ComboTypes_ComboYogaFlame)
		{
			m_ModulePlayerDhalsim->SetStartingCombo(ComboTypes_ComboYogaFlame);
		}

		RestartCombo();
	}
	else if (m_ModulePlayerDhalsim->IsKickInput())
	{
		if (IsAerialComboPossible())
		{
			m_ModulePlayerDhalsim->SetStartingCombo(ComboTypes_AerialComboKick);
		}

		RestartCombo();
	}
	else if (m_ModulePlayerDhalsim->IsMovingForward())
	{
		StartPreviousStateCountdown();
	}
}

void ModuleComboDetection::SetStartingComboParameters(PreviousComboState previousState, ComboTypes typeOfCombo)
{
	m_PreviousState = previousState;
	m_TypeOfCombo = typeOfCombo;

	StartFirstStateCountdown();
	StartPreviousStateCountdown();
}

bool ModuleComboDetection::IsCountdownReached() const
{
	return m_PreviousStateCountdown <= 0 || m_FirstStateCountdown <= 0;
}

bool ModuleComboDetection::IsAerialComboPossible() const
{
	bool isAerialComboPossible = true;
	isAerialComboPossible = isAerialComboPossible && m_IsAirCombo;
	isAerialComboPossible = isAerialComboPossible && m_ModulePlayerDhalsim->IsJumping();
	isAerialComboPossible = isAerialComboPossible && m_ModulePlayerDhalsim->GetDistanceJumped() > 20; //TODO: expose magic number
	return isAerialComboPossible;
}

void ModuleComboDetection::StartFirstStateCountdown()
{
	m_FirstStateCountdown = COMBO_STARTING_COUNTDOWNT;
}

void ModuleComboDetection::StartPreviousStateCountdown()
{
	m_PreviousStateCountdown = COMBO_NEW_COUNTDOWN;
}

void ModuleComboDetection::RestartCombo()
{
	m_IsAirCombo = false;
	m_PreviousState = PreviousComboState_Nothing;
	m_FirstStateCountdown = 0;
	m_PreviousStateCountdown = 0;
}