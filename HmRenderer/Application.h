#pragma once
#include "Common.h"
class Application
{
public:
	Application();
	~Application() {}

	void Initialize(HINSTANCE _hInstance, int _iWidth, int _iHeight);
	void Start();
	void Update();

private:
	void InitializeWindows(HINSTANCE _hInstance, int _iWidth, int _iHeight);

private:
	HINSTANCE m_hInstance;
	HWND m_hHwnd;
	Vec2 m_ScreenSize;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);