#pragma once
#include <d2d1.h>
#include "CSprite.h"

class CDevice;

class CRenderer
{
public:
	CRenderer();
	~CRenderer();
	
	HRESULT Init(CDevice* cdevice);
	void Render();
	void Update();
	void Move();
	
private:
	CSprite* m_csprite;
	ID2D1Bitmap* m_pBitmap;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	int m_x = 0;
};

extern CRenderer* g_crenderer;
