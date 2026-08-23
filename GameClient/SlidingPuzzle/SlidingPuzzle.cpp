#include<windows.h>
#include<tchar.h>
#include <time.h>

#define BLANK -1
#define CELL 110

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
		600, 600,
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

void ShuffleBoard(int board[][5], int& blankR, int &blankC)
{
	int cnt = 0;

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			board[i][j] = cnt++;
		}
	}

	board[4][4] = BLANK;

	for (int i = 0; i < 300; i++)
	{
		int a = rand() % 5;
		int b = rand() % 5;
		int c = rand() % 5;
		int d = rand() % 5;

		if (board[a][b] == BLANK || board[c][d] == BLANK)
			continue;

		int temp = board[a][b];
		board[a][b] = board[c][d];
		board[c][d] = temp;
	}

	blankC = 4;
	blankR = 4;
}

void MakeBoard(HDC hdc, HDC memdc, int board[][5], BITMAP bitmap)
{
	int sw = bitmap.bmWidth / 5;  
	int sh = bitmap.bmHeight / 5;

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if (board[i][j] == BLANK)
				continue;

			StretchBlt(hdc, i * CELL, j * CELL, CELL, CELL,
				memdc, (board[i][j] / 5) * sw, (board[i][j] % 5) * sh, sw, sh, SRCCOPY);
		}
	}
}

bool ClickBoard(int my, int mx, int &blankR, int &blankC, int board[][5])
{
	int i = mx / CELL;
	int j = my / CELL;
	
	if (i < 0 || i >= 5 || j < 0 || j >= 5)
		return false;

	int dx[4] = { 1,0,-1,0 };
	int dy[4] = { 0,-1,0,1 };

	for (int d = 0; d < 4; d++)
	{
		int nx = i + dx[d];
		int ny = j + dy[d];
		if (nx == blankC && ny == blankR)
		{
			int pixel = board[i][j];
			board[i][j] = board[blankC][blankR];
			board[blankC][blankR] = pixel;

			blankC = i;
			blankR = j;
			return true;
		}
	}
	return false;
}

bool CheckWin(int board[][5])
{
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if (board[i][j] == BLANK)
				continue;

			if (board[i][j] != 5 * i + j)
				return false;
		}
	}
	return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc,memdc;
	PAINTSTRUCT ps;
	static HBITMAP hBitmap;
	static BITMAP bitmap;

	static int mx;
	static int my;

	static int board[5][5];
	static int blankR;
	static int blankC;

	switch (message)
	{
	case WM_CREATE:
	{
		srand((unsigned int)time(NULL));
		hBitmap = (HBITMAP)LoadImage(((LPCREATESTRUCT)lParam)->hInstance, _T("puzzle.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		GetObject(hBitmap, sizeof(BITMAP), &bitmap); // 비트맵의 가로/세로 정보를 얻어오기 위한 구조체
		ShuffleBoard(board, blankR, blankC);
	}
		break;

	case WM_LBUTTONDOWN:
	{
		my = HIWORD(lParam);
		mx = LOWORD(lParam);
		
		if (ClickBoard(my, mx, blankR, blankC, board) == true)
			InvalidateRgn(hWnd, NULL, TRUE);

		if (CheckWin(board) == true)
			break;
	}
		break;

	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		memdc = CreateCompatibleDC(hdc);
		SelectObject(memdc, hBitmap);

		MakeBoard(hdc, memdc, board, bitmap);

		DeleteDC(memdc);
		EndPaint(hWnd, &ps);
	}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}