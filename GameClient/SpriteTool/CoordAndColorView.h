#pragma once
#include <d2d1.h>
#include <dwrite.h>
#pragma comment (lib,"dwrite.lib")

class CCoordAndColorView
{
public:
	CCoordAndColorView();
	~CCoordAndColorView();

	void SetPixelInfo(int x, int y, UINT32 pbgra, bool pixelcolor);
	void SetKeyColor(UINT32 bgra);

	void DrawPlate(ID2D1RenderTarget* pRT, ID2D1SolidColorBrush* pBrush, IDWriteTextFormat* pFormat, int row, PCWSTR text, int len);
	void Draw(ID2D1RenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush, IDWriteTextFormat* pFormat);

	void SetSpriteCount(int count);

private:
	int m_x;
	int m_y;
	bool m_pixelcolor;
	bool m_hasKey;

	int m_spriteCount;
	bool m_hasCount;

	UINT32 m_color;
	UINT32 m_keycolor;
};

