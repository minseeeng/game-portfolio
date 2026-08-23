#include "CWIC.h"

#pragma comment(lib, "windowscodecs.lib")

CWIC::CWIC()
	:m_cWICFactory(NULL)
{
}

CWIC::~CWIC()
{
	Release();
}

HRESULT CWIC::Create()
{
	CoInitialize(NULL);

	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL,
		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_cWICFactory));
	if (FAILED(hr)) return hr;

	return hr;
}

void CWIC::Release()
{
	if (m_cWICFactory) m_cWICFactory->Release();
	CoUninitialize();
}

HRESULT CWIC::Load(PCWSTR _wcFileName, ID2D1Bitmap** _ppBitmap, ID2D1HwndRenderTarget* _pRenderTarget)
{
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

	hr = _pRenderTarget->CreateBitmapFromWicBitmap(
		pConverter,
		NULL,
		_ppBitmap);
	if (FAILED(hr)) return hr;

	if (pConverter) { pConverter->Release(); pConverter = nullptr; }
	if (pFrame) { pFrame->Release(); pFrame = nullptr; }
	if (pDecoder) { pDecoder->Release(); pDecoder = nullptr; }

	return hr;
}
