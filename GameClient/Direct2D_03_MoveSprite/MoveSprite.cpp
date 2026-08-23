#include <d2d1.h>
#include <wincodec.h>
#include "CWindow.h"
#include "Device.h"
#include "Renderer.h"
#include "Player.h"

#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2d1.lib")

HWND g_hWnd = nullptr;
Device* g_device = nullptr;
CWindow* g_cwindow = nullptr;
Renderer* g_renderer = nullptr;
Player* g_player = nullptr;

template<class Interface>
inline void SafeRelease(Interface** ppInterfaceToRelease)
{
	if (*ppInterfaceToRelease != NULL)
	{
		(*ppInterfaceToRelease)->Release();
		(*ppInterfaceToRelease) = NULL;
	}
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	g_cwindow = new CWindow();
	g_player = new Player();

	if (FAILED(g_cwindow->MakeRegisterClass(hInstance)))
		return 0;

	if (FAILED(g_cwindow->Create(hInstance, g_hWnd)))
		return 0;

	if (FAILED(g_cwindow->Show(nCmdShow)))
		return 0;

	g_device = new Device();
	if (FAILED(g_device->Create(g_hWnd)))
		return 0;

	SetTimer(g_hWnd, 1, 100, NULL);

	g_renderer = new Renderer();
	if (FAILED(g_renderer->Init(g_device)))
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
			g_renderer->Render(g_device);
		}
	}

	return (int)msg.wParam;
}