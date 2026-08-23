#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include "CSprite.h"
#include "CWindow.h"
#include "CDevice.h"
#include "CRenderer.h"

#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2d1.lib")

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HWND g_hWnd = nullptr;
CDevice* g_cdevice = nullptr;
CWindow* g_pWindow = nullptr;
CRenderer* g_crenderer = nullptr;

template<class Interface>
inline void SafeRelease(Interface** ppInterfaceToRelease)
{
	if (*ppInterfaceToRelease != NULL)
	{
		(*ppInterfaceToRelease)->Release();
		(*ppInterfaceToRelease) = NULL;
	}
}

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	g_pWindow = new CWindow();

	if (FAILED(g_pWindow->MakeRegisterClass(hInstance)))
		return 0;

	if (FAILED(g_pWindow->Create(hInstance, g_hWnd)))
		return 0;

	if (FAILED(g_pWindow->Show(nCmdShow)))
		return 0;

	g_cdevice = new CDevice();
	if (FAILED(g_cdevice->Create(g_hWnd)))
		return 0;

	SetTimer(g_hWnd, 1, 100, NULL);

	g_crenderer = new CRenderer();
	if(FAILED(g_crenderer->Init(g_cdevice)))
		return 0;

	// Main message loop
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			g_crenderer->Render();
		}
	}

	delete g_crenderer;
	delete g_cdevice;
	delete g_pWindow;

	return (int)msg.wParam;
}