#pragma once
#include "Common.h"

enum class EKeyType
{
	UP = VK_UP,
	DOWN = VK_DOWN,
	LEFT = VK_LEFT,
	RIGHT = VK_RIGHT,

	Q = 'Q', W = 'W', E = 'E', R = 'R', T = 'T', Y = 'Y', U = 'U', I = 'I', O = 'O', P = 'P',
	A = 'A', S = 'S', D = 'D', F = 'F', G = 'G', H = 'H', J = 'J', K = 'K', L = 'L',
	Z = 'Z', X = 'X', C = 'C', V = 'V', B = 'B', N = 'N', M = 'M',

	NUM_0 = VK_NUMPAD0, NUM_1 = VK_NUMPAD1, NUM_2 = VK_NUMPAD2,
	NUM_3 = VK_NUMPAD3, NUM_4 = VK_NUMPAD4, NUM_5 = VK_NUMPAD5,
	NUM_6 = VK_NUMPAD6, NUM_7 = VK_NUMPAD7, NUM_8 = VK_NUMPAD8,
	NUM_9 = VK_NUMPAD9,

	N_0 = '0', N_1 = '1', N_2 = '2', N_3 = '3', N_4 = '4', N_5 = '5',
	N_6 = '6', N_7 = '7', N_8 = '8', N_9 = '9',

	ENTER = VK_RETURN,
	SPACE = VK_SPACE,
	TAB = VK_TAB,
	LCTRL = VK_LCONTROL,
	ALT = VK_MENU,
	SHIFT_L = VK_LSHIFT,

	LBUTTON = MK_LBUTTON,
	RBUTTON = MK_RBUTTON,

	SQUARE_BKT_L = VK_OEM_4,
	SQUARE_BKT_R = VK_OEM_6,
};

enum class EKeyState
{
	None,
	Press,
	Down,
	Up,
	End
};

enum
{
	KEY_TYPE_COUNT = static_cast<int>(UINT8_MAX + 1),
	KEY_STATE_COUNT = static_cast<int>(EKeyState::End),
};

class InputManager
{
	DECLARE_SINGLE(InputManager);

public:
	void Initialize(HWND _hHwnd);
	void Update();

	EKeyState GetState(EKeyType _eState) const { return m_States[static_cast<int>(_eState)]; }
	const POINT& GetMousePos() const { return m_MousePos; }
	const POINT& GetPrevMousePos() const { return m_PrevMousePos; }

	bool GetButtonNone(EKeyType _eKey) const { return EKeyState::None == GetState(_eKey); }
	bool GetButtonPress(EKeyType _eKey) const { return EKeyState::Press == GetState(_eKey); }
	bool GetButtonUp(EKeyType _eKey) const { return EKeyState::Up == GetState(_eKey); }
	bool GetButtonDown(EKeyType _eKey) const { return EKeyState::Down == GetState(_eKey); }

private:
	HWND m_hHwnd;
	std::vector<EKeyState> m_States;
	POINT m_MousePos;
	POINT m_PrevMousePos;
};

