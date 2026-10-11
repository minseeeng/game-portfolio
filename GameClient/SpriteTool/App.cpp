#include "App.h"
#include "resource.h"

CApp::CApp()
	:m_spriteview(m_graphicscore, m_image), m_imageview(m_graphicscore, m_image)
{
}

CApp::~CApp()
{
}

bool CApp::Init(HINSTANCE hInstance)
{
	if (FAILED(m_graphicscore.Create()))
		return false;

	if ((m_spriteview.Create(L"SpriteView", L"Sprite", WS_OVERLAPPEDWINDOW,
		0, 100, 900, 900, nullptr, hInstance) == false))
		return false;

	if ((m_imageview.Create(L"ImageView", L"Image", WS_OVERLAPPEDWINDOW,
		950, 100, 900, 900, nullptr, hInstance, IDR_MENU1) == false))
		return false;

	m_imageview.SetSpriteView(&m_spriteview);

	return true;
}

void CApp::CleanUp()
{
}

void CApp::Show(int nCmdShow)
{
	m_imageview.Show(nCmdShow);
	m_spriteview.Show(nCmdShow);
}

int CApp::Run()
{
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}
