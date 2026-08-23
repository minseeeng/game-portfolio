#include <windows.h>
#include <string.h>
#include <tchar.h>
#include <math.h>

#define SQUARE 50
#define BOARDSIZE 10

enum
{
	EMPTY = 0,
	BLACK = 1,
	WHITE = 2,
};

enum
{
	NOBODY = 0,
	BLACKWIN = 1,
	WHITEWIN = 2,
};

int dRow[4] = { 1, 0, 1, -1 };
int dCol[4] = { 0, 1, 1, 1 };


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
		1500, 1000,
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

// 과제 : 아래 내용 함수화 
//         오목 판정
//         승리했다  MessageBox( hWnd, , , MB_OK) ;

int CheckOmok(int row, int col, int dRow, int dCol, int board[][BOARDSIZE])
{
	int cnt = 0;
	int nrow = row + dRow;
	int ncol = col + dCol;

	while ((nrow >= 0 && ncol >= 0 && nrow <= BOARDSIZE - 1 && ncol <= BOARDSIZE - 1 ) &&
		(board[row][col] == board[nrow][ncol]))
	{
		cnt++;
		nrow += dRow;
		ncol += dCol;
	}

	return cnt;
}

int WhoISWin(int row, int col, int board[][BOARDSIZE])
{
	for (int i = 0; i < 4; i++)
	{
		int a = CheckOmok(row, col, dRow[i], dCol[i], board);
		int b = CheckOmok(row, col, -dRow[i], -dCol[i], board);
		int total = (a + b) + 1;

		if (total >= 5)
		{
			if (board[row][col] == BLACK)
				return BLACKWIN;
			else
				return WHITEWIN;
		}
	}
	return NOBODY;
}

bool SetStone(int row, int col, int board[][BOARDSIZE], bool &color)
{
	if (col < 0 || row < 0 || col > BOARDSIZE-1 || row > BOARDSIZE-1)
		return false;

	if (board[row][col] != EMPTY)
		return false;

	color = !color;

	if (color == true)
		board[row][col] = BLACK;
	else
		board[row][col] = WHITE;

	return true;
}

void DrawTiles(HDC hdc, int x, int y,  HBRUSH hWBrush)
{
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hWBrush);

	for (int i = 0; i < BOARDSIZE; i++)
	{
		int dy = (i * SQUARE);
		for (int j = 0; j < BOARDSIZE; j++)
		{
			int dx = (j * SQUARE);
			Rectangle(hdc, x + dx, y + dy, x + (dx + SQUARE), y + (dy + SQUARE));
		}
	}

	SelectObject(hdc, oldBrush);
}

void DrawStones(HDC hdc, int board[][BOARDSIZE], int x, int y, HBRUSH hWBrush, HBRUSH hBBrush)
{
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBBrush);

	for (int i = 0; i < BOARDSIZE; i++)
	{
		for (int j = 0; j < BOARDSIZE; j++)
		{
			if (board[i][j] == EMPTY)
				continue;

			int leftX = j * SQUARE;
			int topY = i * SQUARE;
			int rightX = leftX + SQUARE;
			int bottomY = topY + SQUARE;

			if (board[i][j] == BLACK)
				(HBRUSH)SelectObject(hdc, hBBrush);
			else
				(HBRUSH)SelectObject(hdc, hWBrush);

			Ellipse(hdc, x + leftX, y + topY, x + rightX, y + bottomY);
		}
	}

	SelectObject(hdc, oldBrush);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	HBRUSH hWBrush, hBBrush;
	static int x, y;
	static int board[BOARDSIZE][BOARDSIZE];
	static bool color;
	static int win;
	switch (message)
	{
	case WM_CREATE:
		x = 0;
		y = 0;
		break;

	case WM_LBUTTONDOWN:
	{
		if (win) //둘 중 하나가 승리하면 종료
			break;

		int mx = LOWORD(lParam);
		int my = HIWORD(lParam);

		int col = mx / SQUARE;
		int row = my / SQUARE;

		if((SetStone(row, col, board,color)==false))
			break;

		win = WhoISWin(row, col, board);
	
		InvalidateRgn(hWnd, NULL, true);

		if (win == BLACKWIN)
		{
			MessageBox(hWnd, L"흑돌 승리!", L"결과", MB_OK);
			break;
		}
		else if (win == WHITEWIN)
		{
			MessageBox(hWnd, L"백돌 승리!", L"결과", MB_OK);
			break;
		}
	}
		break;

	case WM_PAINT:	
	{
		PAINTSTRUCT ps;

		hdc = BeginPaint(hWnd, &ps);

		hWBrush = CreateSolidBrush(RGB(255, 255, 255));  // white
		hBBrush = CreateSolidBrush(RGB(0, 0, 0));   // black
		
		DrawTiles(hdc, x, y, hWBrush);
		DrawStones(hdc, board, x, y, hWBrush, hBBrush);
		
		DeleteObject(hBBrush);
		DeleteObject(hWBrush);

		EndPaint(hWnd, &ps);
	}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}



