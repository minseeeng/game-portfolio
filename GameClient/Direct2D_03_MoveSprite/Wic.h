#pragma once
#include <wincodec.h>
#include <d2d1.h>

class Wic
{
public:
	Wic();
	~Wic();

	HRESULT Create();
	void Release();
	HRESULT Load(PCWSTR _wcFileName, ID2D1Bitmap** _ppBitmap, ID2D1HwndRenderTarget* _pRenderTarget);

private:
	IWICImagingFactory* m_cWICFactory;
};

