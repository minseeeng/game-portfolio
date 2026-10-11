#pragma once
#include <d2d1.h>
#include <wincodec.h>
#include <vector>

#pragma comment(lib, "d2d1.lib")

class CGraphicsCore;
struct FillContext
{
	BYTE* pData;
	UINT stride;
	int width;
	int height;
	RECT rect;
	std::vector<uint8_t>* visited;
};

class CImage
{
public:
	CImage();
	~CImage();

	HRESULT Load(CGraphicsCore& core, PCWSTR fileName);
	HRESULT Save(CGraphicsCore& core, PCWSTR fileName);
	void Release();

	HRESULT GetImageSize(UINT& width, UINT& height);
	HRESULT GetPixelColor(int x, int y, UINT32& pbgra);
	HRESULT RemoveKeyColor(UINT32 keycolor);
	HRESULT CreateD2DBitmap(ID2D1RenderTarget* pRenderTarget, ID2D1Bitmap** ppBitmap);

	HRESULT ShrinkRect(RECT& rc);
	HRESULT AutoSlice(std::vector<RECT>& outRects);
	PCWSTR GetFileName() { return m_fileName; }

private:
	static bool ShrinkRectLocked(BYTE* pData, UINT stride, RECT& rc);
	static void FloodFill(FillContext& context, int x, int y);

private:
	IWICBitmap* m_pWICBitmap;
	wchar_t m_fileName[MAX_PATH];
};

