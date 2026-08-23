#pragma once
#include <windows.h>

#define MAP_HEIGHT 224
class Game
{
	enum
	{
		CASE1 = 1,
		CASE2,
		CASE3,
		CASE4,
		CASE5,
	};
public:
	Game();
	~Game();

	bool LoadFile(LPCWSTR _filename, HDC hdc);
	void MoveMap(HDC hdc, HDC memdc, RECT rect, int dx);
	void MoveCharacter(HDC hdc, HDC memdc, int change);
	void ChangeMotion(int &change);
	bool CheckMapSize(int &dx, RECT rect);
	HBITMAP BitMap() const { return m_hBitmap; }

private:
	int CheckWidth(RECT rect) const;
	void ReadFileHeader(HANDLE hFile, BITMAPFILEHEADER &bmFileHeader);
	void ReadInfoHeader(HANDLE hFile, BITMAPFILEHEADER &bmFileHeader);
	void ReadPixel(HANDLE hFile, HDC hdc);
private:
	BITMAPINFO* m_pBitmapInfo;
	LPVOID m_lpDIBits;
	HBITMAP m_hBitmap;
	DWORD m_dwReadBytes;
};