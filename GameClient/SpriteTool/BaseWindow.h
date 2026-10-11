#pragma once
#include <windows.h>

class CBaseWindow
{
public:
	CBaseWindow();
	~CBaseWindow();

	bool Create(const wchar_t* className, const wchar_t* windowName, DWORD style, int x, int y, int width, int height, HWND hParent, HINSTANCE hInstance, int menuID = 0);
	HRESULT Show(int nCmdShow);
	HWND GetHwnd() { return m_hWnd; }

protected:
	virtual LRESULT CALLBACK MainProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	static LRESULT CALLBACK SetUpProc(HWND, UINT, WPARAM, LPARAM);

protected:
	HWND m_hWnd;
	HINSTANCE m_hInstance;
};

