#include "SpriteView.h"
#include "Image.h"

CSpriteView::CSpriteView(CGraphicsCore& core, CImage& image)
	: m_pRenderTarget(nullptr), m_bitmap(nullptr), m_pBrush(nullptr), m_core(core), m_image(image)
{
}

CSpriteView::~CSpriteView()
{
	Release();
}

void CSpriteView::SetSprite(RECT& rc)
{
	m_rcSprites.push_back(rc);
	InvalidateRect(m_hWnd, nullptr, FALSE);
}

void CSpriteView::ClearSprite()
{
	m_rcSprites.clear();
}

void CSpriteView::OnRender()
{
	PAINTSTRUCT ps;
	BeginPaint(m_hWnd, &ps);
	m_pRenderTarget->BeginDraw();

	m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	if (m_rcSprites.empty())
	{
		m_pRenderTarget->EndDraw();
		EndPaint(m_hWnd, &ps);
		return;
	}

	for (int i = 0; i < m_rcSprites.size(); i++)
	{
		D2D1_RECT_F src = D2D1::RectF( (float)m_rcSprites[i].left, (float)m_rcSprites[i].top, (float)m_rcSprites[i].right, (float)m_rcSprites[i].bottom);
		float w = (float)(m_rcSprites[i].right - m_rcSprites[i].left);
		float h = (float)(m_rcSprites[i].bottom - m_rcSprites[i].top);

		m_pRenderTarget->DrawBitmap(
			m_bitmap,
			D2D1::RectF(0, 0, w, h),
			1.0f,
			D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			&src);
	}

	m_pRenderTarget->EndDraw();
	EndPaint(m_hWnd, &ps);
}

void CSpriteView::Release()
{
	if (m_pRenderTarget) { m_pRenderTarget->Release(); m_pRenderTarget = nullptr; }
	if (m_bitmap) { m_bitmap->Release(); m_bitmap = nullptr; }
	if (m_pBrush) { m_pBrush->Release(); m_pBrush = nullptr; }
}

void CSpriteView::ReloadBitmap()
{
	if (m_bitmap)
	{
		m_bitmap->Release();
		m_bitmap = nullptr;
	}
	m_image.CreateD2DBitmap(m_pRenderTarget, &m_bitmap);
	InvalidateRect(m_hWnd, nullptr, FALSE);
}

LRESULT CSpriteView::MainProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		m_core.CreateRenderTarget(m_hWnd, &m_pRenderTarget);
		m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &m_pBrush);
		break;
	case WM_SIZE:
		m_pRenderTarget->Resize(D2D1::SizeU(LOWORD(lParam), HIWORD(lParam)));
		break;
	case WM_PAINT:
		OnRender();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return CBaseWindow::MainProc(message, wParam, lParam);
}
