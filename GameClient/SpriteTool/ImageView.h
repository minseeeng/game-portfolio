#pragma once
#include <vector>
#include "GraphicsCore.h"
#include "BaseWindow.h"
#include "Image.h"
#include "CoordAndColorView.h"
#include "Sprite.h"

class CSpriteView;

class CImageView : public CBaseWindow
{
public:
	CImageView(CGraphicsCore& core, CImage& image);
	~CImageView();
	void SetSpriteView(CSpriteView* sprite) { m_pSpriteView = sprite; }

private:
	void OnLButtonDown(int x, int y);
	void OnMouseMove(int x, int y);
	void OnLButtonUp(int x, int y);
	void OnRButtonDown(int x, int y);

	RECT MakeRect(int left, int top, int right, int bottom);

	void RemoveBackGround();
	void OnAutoSlice();
	bool OpenFile();
	void OnSaveSprite();
	void OnSavePng();
	void OnOpenSprite();

	void OnRender();
	void Release();
	void ReloadBitmap();

protected:
	
	virtual LRESULT CALLBACK MainProc(UINT, WPARAM, LPARAM) override;

private:
	ID2D1HwndRenderTarget* m_pRenderTarget;
	ID2D1Bitmap* m_bitmap;
	ID2D1SolidColorBrush* m_pBrush;
	CGraphicsCore& m_core;
	CImage& m_image;

	bool m_dragging;
	int m_startX;
	int m_startY;
	int m_endX;
	int m_endY;
	int m_mouseX;
	int m_mouseY;

	UINT32 m_keyColor;
	bool m_hashColor;
	CSpriteView* m_pSpriteView;
	CCoordAndColorView m_coordAndColor;
	IDWriteTextFormat* m_pTextFormat;
	std::vector<RECT> m_rcSelects;
};

