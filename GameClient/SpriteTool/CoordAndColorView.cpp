#include "CoordAndColorView.h"
#include <tchar.h>

CCoordAndColorView::CCoordAndColorView()
	: m_x(0), m_y(0), m_pixelcolor(false), m_hasKey(false), m_color(0), m_keycolor(0),
	m_spriteCount(0), m_hasCount(false)
{
}

CCoordAndColorView::~CCoordAndColorView()
{
}

void CCoordAndColorView::SetPixelInfo(int x, int y, UINT32 pbgra, bool pixelcolor)
{
	m_x = x;
	m_y = y;
	m_color = pbgra;
	m_pixelcolor = pixelcolor;
}

void CCoordAndColorView::SetKeyColor(UINT32 bgra)
{
	m_keycolor = bgra;
	m_hasKey = true;
}

void CCoordAndColorView::DrawPlate(ID2D1RenderTarget* pRT, ID2D1SolidColorBrush* pBrush, IDWriteTextFormat* pFormat, int row, PCWSTR text, int len)
{
	D2D1_SIZE_F size = pRT->GetSize();
	const float margin = 10.0f, width = 320.0f, height = 30.0f, gap = 5.0f;

	float bottom = size.height - margin - row * (height + gap);   
	D2D1_RECT_F plate = D2D1::RectF(size.width - width - margin, bottom - height,
		size.width - margin, bottom);

	pBrush->SetColor(D2D1::ColorF(0, 0, 0, 0.6f));
	pRT->FillRectangle(plate, pBrush);
	pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
	pRT->DrawText(text, len, pFormat, plate, pBrush);
}

void CCoordAndColorView::Draw(ID2D1RenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush, IDWriteTextFormat* pFormat)
{
	wchar_t buf[128];
	int row = 0;
	int len = swprintf_s(buf, L"(%d, %d) ARGB 0x%08X", m_x, m_y, m_color);
	DrawPlate(pRenderTarget, pBrush, pFormat, row++, buf, len);

	if (m_hasKey)
	{
		len = swprintf_s(buf, L"Key       ARGB 0x%08X", m_keycolor);
		DrawPlate(pRenderTarget, pBrush, pFormat, row++, buf, len);
	}

	if (m_hasCount)
	{
		len = swprintf_s(buf, L"Sprites   %d", m_spriteCount);
		DrawPlate(pRenderTarget, pBrush, pFormat, row++, buf, len);
	}
}

void CCoordAndColorView::SetSpriteCount(int count)
{
	m_spriteCount = count;
	m_hasCount = true;
}
