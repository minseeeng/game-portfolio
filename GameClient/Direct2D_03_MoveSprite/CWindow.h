#pragma once
#include <windows.h>

class CWindow
{
public:
	CWindow();
	~CWindow();

	HRESULT MakeRegisterClass(HINSTANCE hInstance);
	HRESULT Create(HINSTANCE hInstance, HWND& hWnd);
	HRESULT Show(int nCmdShow);

	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

private:
	HWND m_hWnd;
	HINSTANCE m_hInstance;
};

extern CWindow* g_cwindow;
