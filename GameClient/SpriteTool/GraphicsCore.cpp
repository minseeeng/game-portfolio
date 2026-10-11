#include "GraphicsCore.h"

CGraphicsCore::CGraphicsCore()
	: m_pD2DFactory(nullptr), m_cWICFactory(nullptr), m_pDWriteFactory(nullptr)
{
}

CGraphicsCore::~CGraphicsCore()
{
	Release();
}

HRESULT CGraphicsCore::Create()
{
	CoInitialize(NULL);

	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL,
		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_cWICFactory));
	if (FAILED(hr))
		return hr;

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
	if (FAILED(hr)) return hr;

	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&m_pDWriteFactory));
	if (FAILED(hr)) return hr;

	return S_OK;
}

void CGraphicsCore::Release()
{
	if (m_pD2DFactory) { m_pD2DFactory->Release(); m_pD2DFactory = nullptr; }
	if (m_cWICFactory) { m_cWICFactory->Release(); m_cWICFactory = nullptr; }
	if (m_pDWriteFactory) { m_pDWriteFactory->Release(); m_pDWriteFactory = nullptr; }
	CoUninitialize();
}

HRESULT CGraphicsCore::CreateRenderTarget(HWND hWnd, ID2D1HwndRenderTarget** ppRT)
{
	RECT rc;
	GetClientRect(hWnd, &rc);

	HRESULT hr = m_pD2DFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hWnd,
			D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)),
		ppRT);
	if (FAILED(hr)) return hr;

	return hr;
}

HRESULT CGraphicsCore::LoadImageFile(PCWSTR _wcFileName, IWICBitmap** ppWicBitmap)
{
	IWICBitmap* pWICBitmap = nullptr;
	HRESULT hr = S_OK;
	IWICBitmapDecoder* pDecoder = nullptr;

	hr = m_cWICFactory->CreateDecoderFromFilename(
		_wcFileName,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder);
	if (FAILED(hr)) return hr;

	IWICBitmapFrameDecode* pFrame = nullptr;
	hr = pDecoder->GetFrame(0, &pFrame);
	if (FAILED(hr)) return hr;

	IWICFormatConverter* pConverter = nullptr;
	hr = m_cWICFactory->CreateFormatConverter(&pConverter);
	if (FAILED(hr)) return hr;

	hr = pConverter->Initialize(pFrame,
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		NULL,
		0.0f,
		WICBitmapPaletteTypeCustom);
	if (FAILED(hr)) return hr;


	hr = m_cWICFactory->CreateBitmapFromSource(
		pConverter,
		WICBitmapCacheOnDemand,
		&pWICBitmap);
	if (FAILED(hr)) return hr;

	*ppWicBitmap = pWICBitmap;

	if (pConverter) { pConverter->Release(); pConverter = nullptr; }
	if (pFrame) { pFrame->Release(); pFrame = nullptr; }
	if (pDecoder) { pDecoder->Release(); pDecoder = nullptr; }

	return hr;
}

HRESULT CGraphicsCore::SaveImageFile(PCWSTR _wcFileName, IWICBitmap* pWicBitmap)
{
	if (pWicBitmap == nullptr)
		return E_FAIL;

	IWICStream* pStream = nullptr;
	IWICBitmapEncoder* pEncoder = nullptr;
	IWICBitmapFrameEncode* pFrameEncode = nullptr;

	HRESULT hr = m_cWICFactory->CreateStream(&pStream);

	if (SUCCEEDED(hr))
		hr = pStream->InitializeFromFilename(_wcFileName, GENERIC_WRITE);

	if (SUCCEEDED(hr))
		hr = m_cWICFactory->CreateEncoder(GUID_ContainerFormatPng, nullptr, &pEncoder);

	if (SUCCEEDED(hr))
		hr = pEncoder->Initialize(pStream, WICBitmapEncoderNoCache);

	if (SUCCEEDED(hr))
		hr = pEncoder->CreateNewFrame(&pFrameEncode, nullptr);

	if (SUCCEEDED(hr))
		hr = pFrameEncode->Initialize(nullptr);

	if (SUCCEEDED(hr))
	{
		UINT width = 0, height = 0;
		hr = pWicBitmap->GetSize(&width, &height);

		if (SUCCEEDED(hr))
			hr = pFrameEncode->SetSize(width, height);
	}

	if (SUCCEEDED(hr))
	{
		WICPixelFormatGUID format = GUID_WICPixelFormat32bppBGRA;
		hr = pFrameEncode->SetPixelFormat(&format);
	}

	if (SUCCEEDED(hr))
		hr = pFrameEncode->WriteSource(pWicBitmap, nullptr);

	if (SUCCEEDED(hr))
		hr = pFrameEncode->Commit();

	if (SUCCEEDED(hr))
		hr = pEncoder->Commit();

	if (pFrameEncode) { pFrameEncode->Release(); pFrameEncode = nullptr; }
	if (pEncoder) { pEncoder->Release(); pEncoder = nullptr; }
	if (pStream) { pStream->Release(); pStream = nullptr; }

	return hr;
}

HRESULT CGraphicsCore::CreateTextFormat(IDWriteTextFormat** ppFormat)
{
	if (m_pDWriteFactory == nullptr)
		return E_FAIL;

	WCHAR fontName[] = L"Consolas";
	FLOAT fontsize = 14.0f;

	HRESULT hr = m_pDWriteFactory->CreateTextFormat(
		fontName, NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		fontsize, L"en-us", ppFormat);
	if (FAILED(hr)) return hr;

	return hr;
}
