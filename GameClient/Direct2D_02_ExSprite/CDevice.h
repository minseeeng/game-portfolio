#pragma once
#include <d2d1.h>
#include "CWIC.h"

class CDevice
{
public:
	CDevice();
	~CDevice();

	HRESULT Create(HWND _hWnd);
	void Release();
	ID2D1HwndRenderTarget* GetRenderTarget() { return m_pRenderTarget; }
	ID2D1Bitmap* GetBitmap() { return m_pBitmap; }
	
private:
	ID2D1Factory* m_pD2DFactory;
	ID2D1Bitmap* m_pBitmap;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	CWIC* m_cwic;
};

extern CDevice* g_cdevice;