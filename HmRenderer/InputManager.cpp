#include "InputManager.h"

InputManager::InputManager()
	: m_hHwnd(0)
	, m_MousePos{}
	, m_PrevMousePos{}
{

}

InputManager::~InputManager()
{

}

void InputManager::Initialize(HWND _hHwnd)
{
	m_hHwnd = _hHwnd;
	m_States.resize(KEY_TYPE_COUNT, EKeyState::None);
}

void InputManager::Update()
{
	HWND hwnd = GetActiveWindow();

	if (m_hHwnd != hwnd)
	{
		for (int i = 0; i < KEY_TYPE_COUNT; ++i)
			m_States[i] = EKeyState::None;

		return;
	}
	else
	{
		m_PrevMousePos = m_MousePos;
		GetCursorPos(&m_MousePos);
		ScreenToClient(m_hHwnd, &m_MousePos);
	}

	BYTE asciiKeys[KEY_TYPE_COUNT] = {};
	if (false == GetKeyboardState(asciiKeys))
		return;

	for (int i = 0; i < KEY_TYPE_COUNT; ++i)
	{
		if (asciiKeys[i] & 0x80)
		{
			EKeyState& eState = m_States[i];

			if (EKeyState::Press == eState || EKeyState::Down == eState)
				eState = EKeyState::Press;
			else
				eState = EKeyState::Down;
		}

		else
		{
			EKeyState& eState = m_States[i];

			if (EKeyState::Press == eState || EKeyState::Down == eState)
				eState = EKeyState::Up;
			else
				eState = EKeyState::None;
		}
	}
}
