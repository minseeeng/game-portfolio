#include "Renderer.h"
#include "Device.h"
#include "CWindow.h"
#include "Player.h"

Renderer::Renderer()
	:m_sprite(nullptr)//, m_pBitmap(nullptr), m_pRenderTarget(nullptr)
{
}

Renderer::~Renderer()
{
	delete m_sprite;
}

HRESULT Renderer::Init(Device* device)
{
	ID2D1Bitmap* pBitmap = device->GetBitmap();
	ID2D1HwndRenderTarget* pRenderTarget = device->GetRenderTarget();

	std::vector<D2D1_RECT_F> rects;		// x     y      w       h
	rects.push_back(Sprite::SelectRect(6.0f, 9.0f, 37.0f, 49.0f)); //58
	rects.push_back(Sprite::SelectRect(54.0f, 12.0f, 41.0f, 46.0f)); //58
	rects.push_back(Sprite::SelectRect(106.0f, 7.0f, 40.0f, 51.0f)); //58
	rects.push_back(Sprite::SelectRect(156.0f, 4.0f, 44.0f, 54.0f)); //58
	rects.push_back(Sprite::SelectRect(205.0f, 8.0f, 41.0f, 50.0f)); //58
	rects.push_back(Sprite::SelectRect(253.0f, 8.0f, 42.0f, 50.0f)); //58
	rects.push_back(Sprite::SelectRect(301.0f, 6.0f, 44.0f, 52.0f)); //58

	m_sprite = new Sprite(pBitmap, rects);

	return S_OK;
}

void Renderer::Render(Device* device)
{
	ID2D1HwndRenderTarget* pRenderTarget = device->GetRenderTarget();

	pRenderTarget->BeginDraw();

	pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::CornflowerBlue));
	//m_csprite->Render(m_pRenderTarget, 200.0f, 300.0f);
	m_sprite->Render(pRenderTarget, g_player->PlayerX(), g_player->PlayerY());

	pRenderTarget->EndDraw();
}

void Renderer::Update()
{
	if (m_sprite) m_sprite->Update();
}

void Renderer::Move()
{
}
