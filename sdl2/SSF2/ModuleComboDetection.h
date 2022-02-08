#ifndef __MODULECOMBODETECTION_H__
#define __MODULECOMBODETECTION_H__

#include "Module.h"
#include <vector>

#define COMBO_NEW_COUNTDOWN 15
#define COMBO_STARTING_COUNTDOWNT 75

enum PreviousComboState
{
	PreviousComboState_Down,
	PreviousComboState_DownFront,
	PreviousComboState_Front,
	PreviousComboState_DownBack,
	PreviousComboState_Back,
	PreviousComboState_Nothing
};

class ModulePlayerDhalsim;

class ModuleComboDetection :
	public Module
{
public:
	ModuleComboDetection(PlayerID playerID, bool start_enabled = true);
	~ModuleComboDetection();

	bool Start();
	UpdateStatus PreUpdate();

private:
	void UpdateCounters();
	void UpdateComboState();

	void UpdateComboStateNothing();
	void UpdateComboStateBack();
	void UpdateComboStateDownBack();
	void UpdateComboStateDown();
	void UpdateComboStateDownFront();
	void UpdateComboStateFront();

	bool IsCountdownReached() const;
	bool IsAerialComboPossible() const;

	void SetStartingComboParameters(PreviousComboState previousState, ComboTypes typeOfCombo);
	void StartPreviousStateCountdown();
	void StartFirstStateCountdown();
	void RestartCombo();


	PlayerID m_PlayerID;
	ModulePlayerDhalsim* m_ModulePlayerDhalsim;

	PreviousComboState m_PreviousState;
	int m_PreviousStateCountdown;
	int m_FirstStateCountdown;
	ComboTypes m_TypeOfCombo;
	bool m_IsAirCombo;
};

#endif