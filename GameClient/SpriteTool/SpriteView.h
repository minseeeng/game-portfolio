#pragma once
#include <vector>
#include "BaseWindow.h"
#include "GraphicsCore.h"

class CImage;

class CSpriteView : public CBaseWindow
{
public:
	CSpriteView(CGraphicsCore& core, CImage& image);
	~CSpriteView();

	void SetSprite(RECT& rc);
	void ClearSprite();

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

	std::vector<RECT> m_rcSprites;
};

