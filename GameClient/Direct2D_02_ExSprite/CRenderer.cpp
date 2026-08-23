#include "CRenderer.h"
#include "CDevice.h"

#include "CWindow.h"

CRenderer::CRenderer()
	:m_csprite(nullptr), m_pBitmap(nullptr), m_pRenderTarget(nullptr)
{
}

CRenderer::~CRenderer()
{
	delete m_csprite;
}

HRESULT CRenderer::Init(CDevice* cdevice)
{
	m_pBitmap = cdevice->GetBitmap();
	m_pRenderTarget = cdevice->GetRenderTarget();

	std::vector<D2D1_RECT_F> rects;		// x     y      w       h
	rects.push_back(CSprite::SelectRect(6.0f, 9.0f, 37.0f, 49.0f)); //58
	rects.push_back(CSprite::SelectRect(54.0f, 12.0f, 41.0f, 46.0f)); //58
	rects.push_back(CSprite::SelectRect(106.0f, 7.0f, 40.0f, 51.0f)); //58
	rects.push_back(CSprite::SelectRect(156.0f, 4.0f, 44.0f, 54.0f)); //58
	rects.push_back(CSprite::SelectRect(205.0f, 8.0f, 41.0f, 50.0f)); //58
	rects.push_back(CSprite::SelectRect(253.0f, 8.0f, 42.0f, 50.0f)); //58
	rects.push_back(CSprite::SelectRect(301.0f, 6.0f, 44.0f, 52.0f)); //58

	m_csprite = new CSprite(m_pBitmap, rects);

	return S_OK;
}

void CRenderer::Render()
{
	m_pRenderTarget->BeginDraw();

	m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::CornflowerBlue));
	//m_csprite->Render(m_pRenderTarget, 200.0f, 300.0f);
	m_csprite->Render(m_pRenderTarget, g_playerX, g_playerY );

	m_pRenderTarget->EndDraw();
}

void CRenderer::Update()
{
	if (m_csprite) m_csprite->Update();
}

void CRenderer::Move()
{
}
