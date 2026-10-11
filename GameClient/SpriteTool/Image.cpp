#include "Image.h"
#include "GraphicsCore.h"

CImage::CImage()
	: m_pWICBitmap(nullptr)
{
	m_fileName[0] = L'\0';
}

CImage::~CImage()
{
	Release();
}

HRESULT CImage::Load(CGraphicsCore& core, PCWSTR fileName)
{
	IWICBitmap* pNewBitmap = nullptr;

	HRESULT hr = core.LoadImageFile(fileName, &pNewBitmap);
	if (FAILED(hr))
		return E_FAIL;

	Release();
	m_pWICBitmap = pNewBitmap;
	wcscpy_s(m_fileName, fileName);

	return hr;
}

HRESULT CImage::Save(CGraphicsCore& core, PCWSTR fileName)
{
	if (m_pWICBitmap == nullptr)
		return E_FAIL;

	HRESULT hr = core.SaveImageFile(fileName, m_pWICBitmap);
	if (FAILED(hr))
		return hr;

	wcscpy_s(m_fileName, fileName);

	return hr;
}

void CImage::Release()
{
	if (m_pWICBitmap) { m_pWICBitmap->Release(); m_pWICBitmap = nullptr; }
	m_fileName[0] = L'\0';
}

HRESULT CImage::GetImageSize(UINT& width, UINT& height)
{
	if (m_pWICBitmap == nullptr)
		return E_FAIL;
	else
		return m_pWICBitmap->GetSize(&width, &height);
}

HRESULT CImage::GetPixelColor(int x, int y, UINT32& pbgra)
{
	if (m_pWICBitmap == nullptr)
		return E_FAIL;

	UINT width = 0, height = 0;
	if (FAILED(GetImageSize(width, height)))
		return E_FAIL;

	WICRect rc = { x, y, 1, 1 };
	if (x < 0 || y < 0 || x >= (int)width || y >= (int)height)
		return E_INVALIDARG;

	IWICBitmapLock* pLock = NULL;
	HRESULT hr = m_pWICBitmap->Lock(&rc, WICBitmapLockRead, &pLock);
	if (FAILED(hr)) return hr;

	UINT cbBufferSize = 0;
	BYTE* pPixelData = NULL;
	pLock->GetDataPointer(&cbBufferSize, &pPixelData);

	BYTE b = pPixelData[0], g = pPixelData[1], r = pPixelData[2], a = pPixelData[3];
	pbgra = (a << 24) | (r << 16) | (g << 8) | b;

	pLock->Release();

	return hr;
}

HRESULT CImage::RemoveKeyColor(UINT32 keycolor)
{
	if (m_pWICBitmap == nullptr)
		return E_FAIL;

	UINT width = 0, height = 0;
	if (FAILED(GetImageSize(width, height)))
		return E_FAIL;

	WICRect rcLock = { 0, 0, (INT)width, (INT)height };
	int b = (int)(keycolor & 0xFF);
	int g = (int)((keycolor >> 8) & 0xFF);
	int r = (int)((keycolor >> 16) & 0xFF);
	int a = (int)((keycolor >> 24) & 0xFF);

	IWICBitmapLock* pLock = nullptr;
	HRESULT hr = m_pWICBitmap->Lock(&rcLock, WICBitmapLockWrite, &pLock);
	if (FAILED(hr))
		return hr;

	UINT cbStride = 0;
	UINT cbBufferSize = 0;
	BYTE* pPixelData = nullptr;
	pLock->GetStride(&cbStride);
	pLock->GetDataPointer(&cbBufferSize, &pPixelData);

	BYTE* pRow = pPixelData;
	for (UINT y = 0; y < height; y++)
	{
		BYTE* pPixel = pRow;
		for (UINT x = 0; x < width; x++)
		{
			if (pPixel[0] == b && pPixel[1] == g && pPixel[2] == r && pPixel[3] == a)
			{
				pPixel[0] = 0;
				pPixel[1] = 0;
				pPixel[2] = 0;
				pPixel[3] = 0;
			}
			pPixel += 4;
		}
		pRow += cbStride;
	}

	pLock->Release();
	return S_OK;
}

HRESULT CImage::CreateD2DBitmap(ID2D1RenderTarget* pRenderTarget, ID2D1Bitmap** ppBitmap)
{
	if (m_pWICBitmap == nullptr)
		return E_FAIL;

	HRESULT hr = pRenderTarget->CreateBitmapFromWicBitmap(m_pWICBitmap, NULL, ppBitmap);
	if (FAILED(hr)) return hr;

	return hr;
}

HRESULT CImage::ShrinkRect(RECT& rc)
{
	UINT width = 0, height = 0;
	HRESULT hr = GetImageSize(width, height);
	if (FAILED(hr))
		return E_FAIL;

	if (rc.left < 0) rc.left = 0;
	if (rc.top < 0) rc.top = 0;
	if (rc.right > width) rc.right = width;
	if (rc.bottom > height) rc.bottom = height;
	if (rc.left >= rc.right || rc.top >= rc.bottom)
		return E_FAIL;

	WICRect rcLock = { 0, 0, (INT)width, (INT)height };
	IWICBitmapLock* pLock = nullptr;
	hr = m_pWICBitmap->Lock(&rcLock, WICBitmapLockRead, &pLock);
	if (FAILED(hr))
		return E_FAIL;

	UINT cbStride = 0;
	UINT cbBufferSize = 0;
	BYTE* pPixelData = nullptr;
	pLock->GetStride(&cbStride);
	pLock->GetDataPointer(&cbBufferSize, &pPixelData);

	bool found = ShrinkRectLocked(pPixelData, cbStride, rc);
	pLock->Release();
	return found ? S_OK : S_FALSE;
}

HRESULT CImage::AutoSlice(std::vector<RECT>& outRects)
{
	if (m_pWICBitmap == nullptr)
		return E_FAIL;

	outRects.clear();

	UINT width = 0, height = 0;
	HRESULT hr = GetImageSize(width, height);
	if (FAILED(hr))
		return E_FAIL;

	WICRect rcLock = { 0, 0, (INT)width, (INT)height };
	IWICBitmapLock* pLock = nullptr;
	hr = m_pWICBitmap->Lock(&rcLock, WICBitmapLockRead, &pLock);
	if (FAILED(hr))
		return E_FAIL;

	UINT cbStride = 0;
	UINT cbBufferSize = 0;
	BYTE* pPixelData = nullptr;
	pLock->GetStride(&cbStride);
	pLock->GetDataPointer(&cbBufferSize, &pPixelData);

	std::vector<uint8_t> visited(width * height, 0);

	FillContext context;
	context.pData = pPixelData;
	context.stride = cbStride;
	context.width = width;
	context.height = height;
	context.visited = &visited;

	for (int y = 0; y < (int)height; y++)
	{
		for (int x = 0; x < (int)width; x++)
		{
			context.rect = { x, y, x, y };
			int pixel = y * (int)width + x;
			if (visited[pixel] || pPixelData[y * cbStride + x * 4 + 3] == 0)
				continue;
			FloodFill(context, x, y);
			outRects.push_back({ context.rect.left, context.rect.top, context.rect.right + 1, context.rect.bottom + 1 });
		}
	}

	pLock->Release();
	return S_OK;
}

bool CImage::ShrinkRectLocked(BYTE* pData, UINT stride, RECT& rc)
{
	int minX = rc.right;
	int minY = rc.bottom;
	int maxX = rc.left - 1;
	int maxY = rc.top - 1;

	UINT rcWidth = rc.right - rc.left;
	UINT rowSkip = stride - rcWidth * 4;
	UINT pixelIndex = (rc.top * stride) + (rc.left * 4);
	for (int y = rc.top; y < rc.bottom; y++)
	{
		for (int x = rc.left; x < rc.right; x++)
		{
			if (pData[pixelIndex + 3] != 0)
			{
				if (x < minX) minX = x;
				if (y < minY) minY = y;
				if (x > maxX) maxX = x;
				if (y > maxY) maxY = y;
			}
			pixelIndex += 4;
		}
		pixelIndex += rowSkip;
	}

	if (maxX < minX)
		return false;

	rc = { minX, minY, maxX + 1, maxY + 1 };

	return true;
}

struct XY
{
	int x; 
	int y;
};

void CImage::FloodFill(FillContext& context, int x, int y)
{
	int dx[4] = { 0,1,0,-1 };
	int dy[4] = { -1,0,1,0 };

	std::vector<XY> stackXY;
	stackXY.push_back({ x, y });


	std::vector<std::pair<int, int>> stack;
	stack.push_back({ x, y });

	while (!stack.empty())
	{
		std::pair<int, int> top = stack.back();
		stack.pop_back();

		int tx = top.first;
		int ty = top.second;

		if (tx < 0 || ty < 0 || tx >= context.width || ty >= context.height)
			continue;

		int pixel = ty * context.width + tx;
		if ((*context.visited)[pixel])
			continue;

		if (context.pData[ty * context.stride + tx * 4 + 3] == 0)
			continue;

		(*context.visited)[pixel] = 1;

		context.rect.left = min(context.rect.left, tx);
		context.rect.top = min(context.rect.top, ty);
		context.rect.right = max(context.rect.right, tx);
		context.rect.bottom = max(context.rect.bottom, ty);

		for (int i = 0; i < 4; i++)
		{
			int nx = tx + dx[i];
			int ny = ty + dy[i];

			//int pixel = ty * context.width + tx;
			//if (!(*context.visited)[pixel])
				stack.push_back({ nx,ny });
		}
	}
}
