#pragma once
#include <Windows.h>


/// <summary>
/// 하나의 클래스는 하나의 일을 한다.. 
/// 
/// 사람 클래스 - 사람이 하는 일
/// 망치 클래스 - 못을 박는 일
/// </summary>


class CDib
{
public:
	CDib();
	~CDib();

	void Load(LPCWSTR _filename, HDC hdc);
	HBITMAP BitMap() const { return hBitmap; }

private :
	void ReadHeader(HANDLE hFile);
	void ReadInfoHeader(HANDLE hFile);
	void ReadPixeldata(HDC hdc, HANDLE hFile);

private:
	HBITMAP hBitmap;  // m_, _hBitmap, hBitmap_

	BITMAPFILEHEADER bmFileHeader;
	BITMAPINFO* pBitmapInfo;
	LPVOID lpDIBits;
};