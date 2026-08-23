#include "CDib.h"

CDib::CDib()
	: hBitmap(NULL), bmFileHeader(), pBitmapInfo(NULL), lpDIBits(NULL)
{
}

CDib::~CDib()
{
	delete [] (BYTE*)pBitmapInfo;
	DeleteObject(hBitmap);
}

void CDib::Load(LPCWSTR _filename, HDC hdc)
{
	HANDLE hFile;

	hFile = CreateFile(_filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)	
		return;

	ReadHeader(hFile);
	ReadInfoHeader(hFile);
	ReadPixeldata(hdc, hFile);
	CloseHandle(hFile);
}

void CDib::ReadHeader(HANDLE hFile)
{
	DWORD dwReadBytes = 0;
	ReadFile(hFile, &bmFileHeader, sizeof(BITMAPFILEHEADER), &dwReadBytes, NULL);
}

void CDib::ReadInfoHeader(HANDLE hFile)
{
	DWORD dwReadBytes = 0;
	int iBitmapInfoSize = bmFileHeader.bfOffBits - sizeof(BITMAPFILEHEADER);
	pBitmapInfo = (BITMAPINFO*)new BYTE[iBitmapInfoSize];
	ReadFile(hFile, (LPVOID)pBitmapInfo, iBitmapInfoSize, &dwReadBytes, NULL);
	//pBitmapInfo->bmiHeader.biBitCount = 32;
}

void CDib::ReadPixeldata(HDC hdc, HANDLE hFile)
{
	hBitmap = CreateDIBSection(hdc, pBitmapInfo, DIB_RGB_COLORS, &lpDIBits, NULL, 0);
	BITMAP bitmap;
	GetObject(hBitmap, sizeof(BITMAP), (LPVOID)&bitmap);

	BYTE* dst = (BYTE*)lpDIBits;

	DWORD dwReadBytes = 0;
	int pixelSize = pBitmapInfo->bmiHeader.biWidth * pBitmapInfo->bmiHeader.biHeight * 3;
	BYTE* pBitmap = new BYTE[pixelSize];
	ReadFile(hFile, pBitmap, pixelSize, &dwReadBytes, NULL);

	for (int y = 0; y < pBitmapInfo->bmiHeader.biHeight; y++)
	{
		for (int x = 0; x < pBitmapInfo->bmiHeader.biWidth; x++)
		{
			int srcIndex = (y * pBitmapInfo->bmiHeader.biWidth + x) * 3;
			int dstIndex = (y * pBitmapInfo->bmiHeader.biWidth + x) * 4;

			BYTE Blue = pBitmap[srcIndex + 0];
			BYTE Green = pBitmap[srcIndex + 1];
			BYTE Red = pBitmap[srcIndex + 2];
			BYTE Alpha = 255;

			dst[dstIndex + 0] = Blue;
			dst[dstIndex + 1] = Green;
			dst[dstIndex + 2] = Red;
			dst[dstIndex + 3] = 255;
		}
	}
	
	delete[] pBitmap;
}
