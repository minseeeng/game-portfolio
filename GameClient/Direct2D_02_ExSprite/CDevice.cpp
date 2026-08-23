#include "CDevice.h"

CDevice::CDevice()
	:m_pD2DFactory(nullptr), m_pBitmap(nullptr), m_cwic(nullptr),
	m_pRenderTarget(nullptr)
{
}

CDevice::~CDevice()
{
	Release();
}

HRESULT CDevice::Create(HWND _hWnd)
{
	RECT rc;
	GetClientRect(_hWnd, &rc);

	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
	if (FAILED(hr)) return hr;

	hr = m_pD2DFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(_hWnd,
			D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)),
		&m_pRenderTarget);
	if (FAILED(hr)) return hr;

	m_cwic = new CWIC();

	hr = m_cwic->Create();
	if (FAILED(hr)) return hr;

	hr = m_cwic->Load(L"Aladdin_trans.png", &m_pBitmap, m_pRenderTarget);
	if (FAILED(hr)) return hr;

	return hr;
}

void CDevice::Release()
{
	delete m_cwic;
	if (m_pRenderTarget) m_pRenderTarget->Release();
	if (m_pBitmap)m_pBitmap->Release();
	if (m_pD2DFactory) m_pD2DFactory->Release();
}


