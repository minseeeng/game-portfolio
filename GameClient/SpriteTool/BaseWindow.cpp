#include "BaseWindow.h"
#include "resource.h"

CBaseWindow::CBaseWindow()
	:m_hWnd(nullptr), m_hInstance(nullptr)
{
}

CBaseWindow::~CBaseWindow()
{
}

bool CBaseWindow::Create(const wchar_t* className, const wchar_t* windowName, DWORD style, int x, int y, int width, int height, HWND hParent, HINSTANCE hInstance, int menuID)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = CBaseWindow::SetUpProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = (menuID != 0) ? MAKEINTRESOURCE(menuID) : nullptr;
	wcex.lpszClassName = className;
	wcex.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
		return false;

	m_hInstance = hInstance;

	m_hWnd = CreateWindow(
		className,
		windowName,
		style,
		x, y, width, height,
		hParent, nullptr, GetModuleHandle(nullptr),
		this);

	if (!m_hWnd)
		return false;

	return true;
}

HRESULT CBaseWindow::Show(int nCmdShow)
{
	ShowWindow(m_hWnd, nCmdShow);
	UpdateWindow(m_hWnd);
	return S_OK;
}

LRESULT CBaseWindow::MainProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(m_hWnd, message, wParam, lParam);
}

LRESULT CBaseWindow::SetUpProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_NCCREATE)
	{
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		CBaseWindow* pWindow = reinterpret_cast<CBaseWindow*>(pCreate->lpCreateParams);

		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
		pWindow->m_hWnd = hWnd;
	}

	CBaseWindow* pWindow = reinterpret_cast<CBaseWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	if (pWindow)
	{
		return pWindow->MainProc(message, wParam, lParam);
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
