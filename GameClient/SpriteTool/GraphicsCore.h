#pragma once
#include <d2d1.h>
#include <wincodec.h>
#include <dwrite.h>

#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2d1.lib")

class CGraphicsCore
{
public:
	CGraphicsCore();
	~CGraphicsCore();

	HRESULT Create();
	void Release();

	HRESULT CreateRenderTarget(HWND hWnd, ID2D1HwndRenderTarget** ppRT);
	HRESULT LoadImageFile(PCWSTR _wcFileName, IWICBitmap** ppWicBitmap);
	HRESULT SaveImageFile(PCWSTR _wcFileName, IWICBitmap* pWicBitmap);
	HRESULT CreateTextFormat(IDWriteTextFormat** ppFormat);

private:
	ID2D1Factory* m_pD2DFactory;
	IWICImagingFactory* m_cWICFactory;
	IDWriteFactory* m_pDWriteFactory;
};

