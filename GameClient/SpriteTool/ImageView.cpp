#include "ImageView.h"
#include "SpriteView.h"
#include <tchar.h>
#include "resource.h"

CImageView::CImageView(CGraphicsCore& core, CImage& image)
	:m_pRenderTarget(nullptr),m_bitmap(nullptr),m_pBrush(nullptr), m_core(core), m_image(image),
	m_dragging(false), m_startX(0), m_startY(0), m_endX(0), m_endY(0),
	m_mouseX(0), m_mouseY(0), m_keyColor(0), m_hashColor(false),
	m_pSpriteView(nullptr), m_pTextFormat(nullptr)
{
}

CImageView::~CImageView()
{
	Release();
}

void CImageView::OnLButtonDown(int x, int y)
{
	m_dragging = true;
	m_startX = x;
	m_startY = y;
	m_endX = x;
	m_endY = y;
}

void CImageView::OnMouseMove(int x, int y)
{
	m_mouseX = x;
	m_mouseY = y;

	UINT32 color = 0;
	bool pixelColor = SUCCEEDED(m_image.GetPixelColor(x, y, color));
	m_coordAndColor.SetPixelInfo(x, y, color, pixelColor);

	if (m_dragging == false)
		return;

	m_endX = x;
	m_endY = y;
}

void CImageView::OnLButtonUp(int x, int y)
{
	m_dragging = false;
	m_endX = x;
	m_endY = y;

	RECT rect = MakeRect(m_startX, m_startY, m_endX, m_endY);

	if (m_image.ShrinkRect(rect) != S_OK)
		return;

	m_rcSelects.push_back(rect);

	if (m_pSpriteView)
		m_pSpriteView->SetSprite(rect);

	m_coordAndColor.SetSpriteCount((int)m_rcSelects.size());
	InvalidateRect(m_hWnd, nullptr, FALSE);
}

void CImageView::OnRButtonDown(int x, int y)
{
	UINT32 color = 0;
	if (FAILED(m_image.GetPixelColor(x, y, color)))
		return;
	else
	{
		m_keyColor = color;
		m_hashColor = true;
		m_coordAndColor.SetKeyColor(color);
	}
}

RECT CImageView::MakeRect(int left, int top, int right, int bottom)
{
	return { min(left, right), min(top, bottom), max(left, right), max(top, bottom) };
}

void CImageView::RemoveBackGround()
{
	if (!m_hashColor)
		return;
	
	if (FAILED(m_image.RemoveKeyColor(m_keyColor)))
		return;

	ReloadBitmap();
	if (m_pSpriteView)
		m_pSpriteView->ReloadBitmap();
}

void CImageView::OnAutoSlice()
{
	std::vector<RECT> rects;

	if (FAILED(m_image.AutoSlice(rects)))
		return;

	m_rcSelects = rects;

	if (m_pSpriteView)
	{
		m_pSpriteView->ClearSprite();
		for (int i = 0; i < m_rcSelects.size(); i++)
			m_pSpriteView->SetSprite(m_rcSelects[i]);
	}

	m_coordAndColor.SetSpriteCount((int)m_rcSelects.size());
	InvalidateRect(m_hWnd, nullptr, FALSE);
}

bool CImageView::OpenFile()
{
	OPENFILENAME ofn;
	TCHAR lpstrFile[MAX_PATH] = _T("");
	TCHAR filter[] = _T("Every File(*.*)\0*.*\0Text File\0*.png;*.bmp;*.jpg\0");
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = lpstrFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrInitialDir = _T(".");

	if (GetOpenFileName(&ofn) != 0)
	{
		if (FAILED(m_image.Load(m_core, ofn.lpstrFile)))
			return false;
	}
	else
		return false;

	ReloadBitmap();
	m_rcSelects.clear();
	if (m_pSpriteView)
	{
		m_pSpriteView->ClearSprite();
		m_pSpriteView->ReloadBitmap();
	}

	m_coordAndColor.SetSpriteCount(0);
	InvalidateRect(m_hWnd, nullptr, FALSE);
	return true;
}

void CImageView::OnSaveSprite()
{
	OPENFILENAME ofn = {};
	TCHAR lpstrFile[MAX_PATH] = _T("");
	TCHAR filter[] = _T("Sprite File(*.spr)\0*.spr\0");
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = lpstrFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrInitialDir = _T(".");
	ofn.lpstrDefExt = _T("spr");

	if (GetSaveFileName(&ofn) == 0)
		return;

	CSprite::SaveSpriteFile(ofn.lpstrFile, m_image.GetFileName(), m_rcSelects);
	InvalidateRect(m_hWnd, nullptr, FALSE);
}

void CImageView::OnSavePng()
{
	OPENFILENAME ofn = {};
	TCHAR lpstrFile[MAX_PATH] = _T("");
	TCHAR filter[] = _T("PNG File(*.png)\0*.png\0");
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = lpstrFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrInitialDir = _T(".");
	ofn.lpstrDefExt = _T("png");
	ofn.Flags = OFN_OVERWRITEPROMPT;

	if (GetSaveFileName(&ofn) == 0)
		return;

	m_image.Save(m_core, ofn.lpstrFile);
}

void CImageView::OnOpenSprite()
{
	OPENFILENAME ofn = {};
	TCHAR lpstrFile[MAX_PATH] = _T("");
	TCHAR filter[] = _T("Sprite File(*.spr)\0*.spr\0");
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = lpstrFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrInitialDir = _T(".");

	if (GetOpenFileName(&ofn) == 0)
		return;

	wchar_t imageFile[MAX_PATH] = L"";
	std::vector<RECT> rects;

	if(!CSprite::LoadSpriteFile(ofn.lpstrFile, imageFile, MAX_PATH, rects))
		return;

	if (FAILED(m_image.Load(m_core, imageFile)))     
		return;

	m_rcSelects = rects;

	ReloadBitmap();
	if (m_pSpriteView)
	{
		m_pSpriteView->ReloadBitmap();
		m_pSpriteView->ClearSprite();
		for (int i = 0; i < m_rcSelects.size(); i++)
			m_pSpriteView->SetSprite(m_rcSelects[i]);
	}
	m_coordAndColor.SetSpriteCount((int)m_rcSelects.size());
	InvalidateRect(m_hWnd, nullptr, FALSE);
}

void CImageView::OnRender()
{
	PAINTSTRUCT ps;
	BeginPaint(m_hWnd, &ps);
	m_pRenderTarget->BeginDraw();

	m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
	m_pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Red));
	if (m_bitmap == NULL)
	{
		m_pRenderTarget->EndDraw();
		EndPaint(m_hWnd, &ps);
		return;
	}

	D2D1_SIZE_F size = m_bitmap->GetSize();
	m_pRenderTarget->DrawBitmap(m_bitmap,
		D2D1::RectF(0, 0, size.width, size.height));

	for (int i = 0; i < m_rcSelects.size(); i++)
	{
		m_pRenderTarget->DrawRectangle(
			D2D1::RectF((float)m_rcSelects[i].left, (float)m_rcSelects[i].top,
				(float)m_rcSelects[i].right, (float)m_rcSelects[i].bottom),
			m_pBrush,
			1.0f);
	}

	if (m_dragging)
	{
		RECT rc = MakeRect(m_startX, m_startY, m_endX, m_endY);
		m_pRenderTarget->DrawRectangle(
			D2D1::RectF((float)rc.left, (float)rc.top,
				(float)rc.right, (float)rc.bottom),
			m_pBrush,
			1.0f);
	}

	m_coordAndColor.Draw(m_pRenderTarget, m_pBrush, m_pTextFormat);
	m_pRenderTarget->EndDraw();
	EndPaint(m_hWnd, &ps);
}

void CImageView::Release()
{
	if (m_pRenderTarget) { m_pRenderTarget->Release(); m_pRenderTarget = nullptr; }
	if (m_bitmap) { m_bitmap->Release(); m_bitmap = nullptr; }
	if (m_pBrush) { m_pBrush->Release(); m_pBrush = nullptr; }
}

void CImageView::ReloadBitmap()
{
	if (m_bitmap)
	{
		m_bitmap->Release();
		m_bitmap = nullptr;
	}
	m_image.CreateD2DBitmap(m_pRenderTarget, &m_bitmap);
	InvalidateRect(m_hWnd, nullptr, FALSE);
}

LRESULT CImageView::MainProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		m_core.CreateRenderTarget(m_hWnd, &m_pRenderTarget);
		m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &m_pBrush);
		m_core.CreateTextFormat(&m_pTextFormat);
		break;
	case WM_SIZE:
		m_pRenderTarget->Resize(D2D1::SizeU(LOWORD(lParam), HIWORD(lParam)));
		break;
	case WM_PAINT:
		OnRender();
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_40001:
			OpenFile();
			break;
		case ID_40003:
			OnAutoSlice();
			break;
		case ID_40004:
			RemoveBackGround();
			break;
		case ID_40005:
			OnSaveSprite();
			break;
		case ID_40006:
			OnOpenSprite();
			break;
		case ID_40007:
			OnSavePng();
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		OnLButtonDown(LOWORD(lParam), HIWORD(lParam));
		InvalidateRect(m_hWnd, nullptr, FALSE);
		break;
	case WM_MOUSEMOVE:
		OnMouseMove(LOWORD(lParam), HIWORD(lParam));
		InvalidateRect(m_hWnd, nullptr, FALSE);
		break;
	case WM_LBUTTONUP:
		OnLButtonUp(LOWORD(lParam), HIWORD(lParam));
		InvalidateRect(m_hWnd, nullptr, FALSE);
		break;
	case WM_RBUTTONDOWN:
		OnRButtonDown(LOWORD(lParam), HIWORD(lParam));
		InvalidateRect(m_hWnd, nullptr, FALSE);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return CBaseWindow::MainProc(message, wParam, lParam);
}
