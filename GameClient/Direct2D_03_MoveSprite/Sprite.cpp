#include "Sprite.h"

Sprite::Sprite(ID2D1Bitmap* bitmap, const std::vector<D2D1_RECT_F>& rects)
	:m_bitmap(bitmap), m_srcRects(rects), m_index(0)
{
}

Sprite::~Sprite()
{
}

void Sprite::Render(ID2D1HwndRenderTarget* rt, float bx, float by)
{
	float width = (m_srcRects[m_index].right - m_srcRects[m_index].left);
	float height = (m_srcRects[m_index].bottom - m_srcRects[m_index].top);
	float top = by - height;
	float bottom = by;

	rt->DrawBitmap(m_bitmap, D2D1::RectF(bx, top, width + bx, bottom),
		1.0f,
		D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		m_srcRects[m_index]);
}

void Sprite::Update()
{
	m_index++;
	m_index = m_index % m_srcRects.size();
}

D2D1_RECT_F Sprite::SelectRect(float x, float y, float w, float h)
{
	float right = x + w;
	float bottom = y + h;
	D2D1_RECT_F rect = D2D1::RectF(x, y, right, bottom);

	return rect;
}
