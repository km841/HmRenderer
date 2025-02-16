#include <windows.h>
#include "HmRenderer.h"
#include "Application.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(54209);

	Application* pApplication = new Application();

	pApplication->Initialize(hInstance, 800, 600);
	pApplication->Start();
	
	delete pApplication;

	return 0;
}
