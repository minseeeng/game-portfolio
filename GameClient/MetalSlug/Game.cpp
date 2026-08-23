#include "Game.h"

Game::Game()
	: m_hBitmap(NULL), m_pBitmapInfo(NULL), m_lpDIBits(NULL), m_dwReadBytes(0)
{
}

Game::~Game()
{
	delete[](BYTE*)m_pBitmapInfo;
	DeleteObject(m_hBitmap);
}

bool Game::LoadFile(LPCWSTR _filename, HDC hdc)
{
	HANDLE hFile;
	hFile = CreateFile(_filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	BITMAPFILEHEADER bmFileHeader;
	ReadFileHeader(hFile, bmFileHeader);
	ReadInfoHeader(hFile, bmFileHeader);
	ReadPixel(hFile, hdc);

	CloseHandle(hFile);
	return true;
}

void Game::ReadFileHeader(HANDLE hFile, BITMAPFILEHEADER &bmFileHeader)
{
	ReadFile(hFile, &bmFileHeader, sizeof(BITMAPFILEHEADER), &m_dwReadBytes, NULL);
}

void Game::ReadInfoHeader(HANDLE hFile, BITMAPFILEHEADER &bmFileHeader)
{
	int iBitmapInfoSize = bmFileHeader.bfOffBits - sizeof(BITMAPFILEHEADER);
	m_pBitmapInfo = (BITMAPINFO*)new BYTE[iBitmapInfoSize];
	ReadFile(hFile, (LPVOID)m_pBitmapInfo, iBitmapInfoSize, &m_dwReadBytes, NULL);
}

void Game::ReadPixel(HANDLE hFile, HDC hdc)
{
	m_hBitmap = CreateDIBSection(hdc, m_pBitmapInfo, DIB_RGB_COLORS, &m_lpDIBits, NULL, 0);
	ReadFile(hFile, m_lpDIBits, m_pBitmapInfo->bmiHeader.biSizeImage, &m_dwReadBytes, NULL);
}

void Game::MoveMap(HDC hdc, HDC memdc, RECT rect, int dx)
{
	int viewW = CheckWidth(rect);
	StretchBlt(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, memdc, 0 + dx, 0, viewW, MAP_HEIGHT, SRCCOPY);
}

void Game::MoveCharacter(HDC hdc, HDC memdc, int change)
{
	switch (change)
	{
	case CASE1:
	{
		StretchBlt(hdc, 400, 440, 27, 40, memdc, 1406, 1055, 27, 40, SRCCOPY);
	}
	break;
	case CASE2:
	{
		StretchBlt(hdc, 400, 440, 27, 40, memdc, 1436, 1055, 27, 40, SRCCOPY);
	}
	break;
	case CASE3:
	{
		StretchBlt(hdc, 400, 440, 27, 40, memdc, 1466, 1055, 27, 40, SRCCOPY);
	}
	break;
	case CASE4:
	{
		StretchBlt(hdc, 400, 440, 27, 40, memdc, 1501, 1055, 27, 40, SRCCOPY);
	}
	break;
	case CASE5:
		StretchBlt(hdc, 400, 440, 27, 40, memdc, 1534, 1055, 27, 40, SRCCOPY);
		break;
	}
}

void Game::ChangeMotion(int &change)
{
	change++;
	if (change > CASE5)
	{
		change = CASE1;
	}
}

bool Game::CheckMapSize(int &dx, RECT rect)
{
	int viewW = CheckWidth(rect);

	if (dx >= (m_pBitmapInfo->bmiHeader.biWidth- viewW))
		return false;
	else
		dx += 3;

	return true;
}

int Game::CheckWidth(RECT rect) const
{
	int viewW = (rect.right - rect.left);

	return viewW;
}
