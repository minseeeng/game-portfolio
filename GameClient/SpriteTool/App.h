#pragma once
#include "GraphicsCore.h"
#include "SpriteView.h"
#include "ImageView.h"
#include "Image.h"

class CApp
{
public:
	CApp();
	~CApp();

	bool Init(HINSTANCE hInstance);
	void CleanUp();
	void Show(int nCmdShow);
	int Run();

private:
	CGraphicsCore m_graphicscore;
	CImage m_image;
	CSpriteView m_spriteview;
	CImageView m_imageview;
};

