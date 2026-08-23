#pragma once
#include <d2d1.h>
#include <wincodec.h>
#include <vector>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "windowscodecs.lib")

// 그림의 한조각
class CSprite
{
public:
	CSprite(ID2D1Bitmap* bitmap, const std::vector<D2D1_RECT_F>& rects);
	~CSprite();

	void Render(ID2D1HwndRenderTarget* rt, float bx, float by);
	void Update();

	static D2D1_RECT_F SelectRect(float x, float y, float w, float h);
private:
	ID2D1Bitmap* m_bitmap;
	std::vector<D2D1_RECT_F> m_srcRects;
	int m_index;
};

