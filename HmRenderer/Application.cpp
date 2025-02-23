#include "Application.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "Graphics.h"

Application::Application()
	: m_hHwnd(0)
	, m_hInstance(0)
{
}

void Application::Initialize(HINSTANCE _hInstance, int _iWidth, int _iHeight)
{
	m_hInstance = _hInstance;
	InitializeWindows(_hInstance, _iWidth, _iHeight);

	GET_SINGLE(TimeManager)->Initialize();
	GET_SINGLE(InputManager)->Initialize(m_hHwnd);
	GET_SINGLE(Graphics)->Initialize(m_hHwnd, _iWidth, _iHeight);
}

void Application::Start()
{
	MSG msg;

	ZeroMemory(&msg, sizeof(MSG));

	HACCEL hAccelTable = LoadAccelerators(m_hInstance, MAKEINTRESOURCE(1));
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (WM_QUIT == msg.message)
				break;

			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

		}

		Update();
	}
}

void Application::Update()
{
	GET_SINGLE(TimeManager)->Update();
	GET_SINGLE(InputManager)->Update();
	GET_SINGLE(Graphics)->Update(DELTA_TIME);
	GET_SINGLE(Graphics)->Render(DELTA_TIME);
}

void Application::InitializeWindows(HINSTANCE _hInstance, int _iWidth, int _iHeight)
{
	wstring strName = _T("HmRenderer");

	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = strName.c_str();
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	Vec2 CenterPos;
	CenterPos.x = (float)(GetSystemMetrics(SM_CXSCREEN) - _iWidth) / 2;
	CenterPos.y = (float)(GetSystemMetrics(SM_CYSCREEN) - _iHeight) / 2;
	

	m_hHwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		strName.c_str(),
		strName.c_str(),
		WS_OVERLAPPEDWINDOW,
		(int)CenterPos.x, (int)CenterPos.y,
		_iWidth, 
		_iHeight, 
		NULL,
		NULL,
		m_hInstance,
		NULL 
	);

	ShowWindow(m_hHwnd, SW_SHOW);
	SetForegroundWindow(m_hHwnd);
	SetFocus(m_hHwnd);

	ShowCursor(true);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}