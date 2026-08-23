#include <windows.h>
#include <tchar.h>
#include "Game.h"

Game game;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = TEXT("MyWindowClass");
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wcex);

	HWND hWnd = CreateWindow(
		TEXT("MyWindowClass"),
		TEXT("Window Title Name"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		800, 600,
		NULL, NULL, hInstance, NULL
	);

	if (!hWnd)
		return 0;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	HDC hdc;
	PAINTSTRUCT ps;
	
	static HDC memdc;
	static HBITMAP bitmap;
	static HBITMAP oldmap;
	static int change = 1;
	static int dx;
	switch (message)
	{
	case WM_CREATE:
		SetTimer(hWnd, 1, 200, NULL);
		hdc = GetDC(hWnd);
		if ((game.LoadFile(_T("metalslug.bmp"), hdc)) == false)
			break;
		bitmap = game.BitMap();
		memdc = CreateCompatibleDC(hdc);
		oldmap = (HBITMAP)SelectObject(memdc, bitmap);
		ReleaseDC(hWnd, hdc);
		break;
	case WM_TIMER:
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		game.ChangeMotion(change);
		if((game.CheckMapSize(dx,rect)==false))
			break;
		InvalidateRgn(hWnd, NULL, TRUE);
	}
		break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		RECT rect;
		GetClientRect(hWnd, &rect);
		game.MoveMap(hdc, memdc, rect, dx);
		game.MoveCharacter(hdc, memdc, change);
		EndPaint(hWnd, &ps);
	}
		break;
	case WM_DESTROY:
		KillTimer(hWnd, 1);
		DeleteDC(memdc);
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}