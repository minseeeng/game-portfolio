// JPS_.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "JPS_.h"
#include<iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <windowsx.h>
#include <queue>
#include "JPS.h"
#include "Render.h"

#define MAX_LOADSTRING 100
#define GRID_SIZE 16    // 각 그리드 셀의 픽셀 크기
#define GRID_WIDTH 100
#define GRID_HEIGHT 50


HBRUSH g_hTileBrush;    //장애물 칠하는 브러시
HBRUSH g_hStartBrush;   //시작점 칠하는 브러시
HBRUSH g_hEndBrush; //도착점 칠하는 브러시
HBRUSH g_hPathBrush;    //경로 칠하는 브러시
HBRUSH g_hClosedBrush;  //탐색된 노드 칠하는 브러시
HBRUSH g_hLineBrush; //이건 뭐였지
HPEN g_hGridPen;    //그리드 선 그리는 펜


char g_Tile[GRID_HEIGHT][GRID_WIDTH];   //0 장애물 없음 / 1 장애물 있음
bool g_bErase = false;  //지우기 모드지 확인
bool g_bDrag = false;   //마우스 드래그 중인지 확인


//시각화용 변수들
std::vector<Node*> g_OpenList;  //앞으로 탐색할 후보 노드 목록 단계별 실행 시
std::vector<Node*> g_ClosedList;    //이미 탐색한 노드들
std::vector<Node*> g_Path;  //최종 경로
std::vector<Node*> g_SmoothedPath;


// A* 상태 관리 전역 변수
Node* g_pStartNode = nullptr;   //사용자가 설정한 시작노드
Node* g_pEndNode = nullptr; //사용자가 설정한 도착 노드
bool g_bStepMode = false;   // 단계별 모드 여부


// 향상된 시각화를 위한 추가 전역 변수들
float g_fZoomLevel = 1.0f;      // 줌 레벨
int g_iOffsetX = 0;             // 화면 오프셋 X
int g_iOffsetY = 0;             // 화면 오프셋 Y
bool g_bShowValues = false;     // 가중치 값 표시 여부
HFONT g_hFont = nullptr;        // 텍스트 출력용 폰트


int g_SearchIDMarker[GRID_HEIGHT][GRID_WIDTH] = { 0 };
int g_currentSearchID = 0;
std::map<int, HBRUSH> g_SearchIDBrushMap;


//단계별 실행용 변수들
PathFinder* g_pPathFinder = nullptr; // 단계별 실행을 위한 PathFinder 인스턴스
enum SearchState { NOT_STARTED, IN_PROGRESS, FINISHED };
SearchState g_SearchState = NOT_STARTED;


//메모리DC 관련 변수들
HBITMAP g_hMemDCBitmap;
HBITMAP g_hMemDCBitmap_Old;
HDC g_hMemDC;
RECT g_MemDCRect;


// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.


// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_JPS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_JPS));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}


//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_JPS));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_JPS);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //PAINTSTRUCT ps;
    HDC hdc;
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_LBUTTONDOWN:
        g_bDrag = true;
        {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);
            int iTileX;
            int iTileY;
            ScreenToGrid(xPos, yPos, iTileX, iTileY);

            if (iTileX >= 0 && iTileX < GRID_WIDTH &&
                iTileY >= 0 && iTileY < GRID_HEIGHT)
            {
                if (g_Tile[iTileY][iTileX] == 1) // 첫 선택 타일이 장애물이면 지우기 모드 아니면 장애물 넣기모드
                    g_bErase = true;
                else
                    g_bErase = false;
            }
            else
            {
                g_bDrag = false; // 바깥 클릭이면 드래그 모드 시작 안 함
            }
        }
        break;
    case WM_RBUTTONDOWN:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        int iTileX;
        int iTileY;

        ScreenToGrid(xPos, yPos, iTileX, iTileY);

        if (iTileX >= 0 && iTileX < GRID_WIDTH && iTileY >= 0 && iTileY < GRID_HEIGHT)
        {
            memset(g_SearchIDMarker, 0, sizeof(g_SearchIDMarker));
            g_currentSearchID = 0;
            //memset(g_Tile, 0, sizeof(g_Tile)); // 맵 클리어
            if (g_Tile[iTileY][iTileX] == 0)
            {
                g_SearchState = NOT_STARTED;
                g_bStepMode = false; 

                delete g_pPathFinder;
                g_pPathFinder = nullptr;

                g_Path.clear();
                g_OpenList.clear();
                g_ClosedList.clear();
                g_SmoothedPath.clear();

                if (g_pStartNode == nullptr)
                {
                    g_pStartNode = new Node(iTileX, iTileY);
                }
                else if (g_pEndNode == nullptr)
                {
                    g_pEndNode = new Node(iTileX, iTileY);
                }
                else 
                {
                    delete g_pStartNode;
                    delete g_pEndNode;
                    g_pStartNode = new Node(iTileX, iTileY);
                    g_pEndNode = nullptr;
                }

                InvalidateRect(hWnd, NULL, false); // 화면 갱신
            }
        }
    }
    break;
    case WM_LBUTTONUP:
        g_bDrag = false;
        break;
    case WM_MOUSEMOVE:
    {
        if (g_bDrag)
        {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            int iTileX, iTileY;
            ScreenToGrid(xPos, yPos, iTileX, iTileY);

            if (iTileX >= 0 && iTileX < GRID_WIDTH &&
                iTileY >= 0 && iTileY < GRID_HEIGHT)
            {
                if (g_bErase)
                    g_Tile[iTileY][iTileX] = 0;
                else
                    g_Tile[iTileY][iTileX] = 1;
            }
            InvalidateRect(hWnd, NULL, false);
        }
    }
    break;
    // 마우스 휠 줌 처리 추가:
    case WM_MOUSEWHEEL:
    {
        int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        float oldZoom = g_fZoomLevel;

        if (delta > 0)
            g_fZoomLevel = min(8.0f, g_fZoomLevel * 1.2f); // 최대 8배 확대
        else
            g_fZoomLevel = max(0.5f, g_fZoomLevel * 0.8f);  // 최소 0.5배 축소

        // 마우스 위치를 중심으로 줌
        int mouseX = GET_X_LPARAM(lParam);
        int mouseY = GET_Y_LPARAM(lParam);

        float zoomRatio = g_fZoomLevel / oldZoom;
        g_iOffsetX = (int)(mouseX - (mouseX - g_iOffsetX) * zoomRatio);
        g_iOffsetY = (int)(mouseY - (mouseY - g_iOffsetY) * zoomRatio);

        InvalidateRect(hWnd, NULL, FALSE);
    }
    break;
    case WM_CREATE:
    {
        HDC hdc = GetDC(hWnd);
        GetClientRect(hWnd, &g_MemDCRect);
        g_hMemDCBitmap = CreateCompatibleBitmap(hdc, g_MemDCRect.right, g_MemDCRect.bottom);
        g_hMemDC = CreateCompatibleDC(hdc);
        ReleaseDC(hWnd, hdc);
        g_hMemDCBitmap_Old = (HBITMAP)SelectObject(g_hMemDC, g_hMemDCBitmap);
        g_hGridPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
        g_hTileBrush = CreateSolidBrush(RGB(100, 100, 100));

        g_hStartBrush = CreateSolidBrush(RGB(0, 200, 0)); // 시작점 (초록)
        g_hEndBrush = CreateSolidBrush(RGB(200, 0, 0));   // 도착점 (빨강)
        g_hPathBrush = CreateSolidBrush(RGB(255, 200, 0)); // 경로 (노랑)
        g_hClosedBrush = CreateSolidBrush(RGB(0, 255, 255)); // 탐색된 곳 (파랑)
        g_hLineBrush = CreateSolidBrush(RGB(255, 105, 180)); //브렌즈헴 (핫핑크)

        // 폰트 생성
        g_hFont = CreateFont(
            12,                        // nHeight
            0,                         // nWidth
            0,                         // nEscapement
            0,                         // nOrientation
            FW_HEAVY,                 // nWeight
            FALSE,                     // bItalic
            FALSE,                     // bUnderline
            0,                         // cStrikeOut
            ANSI_CHARSET,              // nCharSet
            OUT_DEFAULT_PRECIS,        // nOutPrecision
            CLIP_DEFAULT_PRECIS,       // nClipPrecision
            DEFAULT_QUALITY,           // nQuality
            DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
            L"Arial");
    }
    break;
    case WM_PAINT:
    {
        PatBlt(g_hMemDC, 0, 0, g_MemDCRect.right, g_MemDCRect.bottom, WHITENESS);
        PAINTSTRUCT ps;
        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...

        RenderSearchedArea(g_hMemDC);
        RenderJPSResult(g_hMemDC);
        RenderSmoothedPath(g_hMemDC);

        RenderObstacle(g_hMemDC);
        RenderGrid(g_hMemDC);

        hdc = BeginPaint(hWnd, &ps);
        BitBlt(hdc, 0, 0, g_MemDCRect.right, g_MemDCRect.bottom, g_hMemDC, 0, 0, SRCCOPY);
        EndPaint(hWnd, &ps);
        break;
    }
    break;
    case WM_SIZE:
    {
        SelectObject(g_hMemDC, g_hMemDCBitmap_Old);
        DeleteObject(g_hMemDC);
        DeleteObject(g_hMemDCBitmap);

        HDC hdc = GetDC(hWnd);

        GetClientRect(hWnd, &g_MemDCRect);
        g_hMemDCBitmap = CreateCompatibleBitmap(hdc, g_MemDCRect.right, g_MemDCRect.bottom);
        g_hMemDC = CreateCompatibleDC(hdc);
        ReleaseDC(hWnd, hdc);

        g_hMemDCBitmap_Old = (HBITMAP)SelectObject(g_hMemDC, g_hMemDCBitmap);
    }
    break;

    case WM_KEYDOWN: // 키보드 입력
    {

        if (wParam == 'A') 
        {
            if (g_pStartNode != nullptr && g_pEndNode != nullptr)
            {
                for (Node* node : g_ClosedList)
                {
                    delete node;
                }

                memset(g_SearchIDMarker, 0, sizeof(g_SearchIDMarker));
                g_ClosedList.clear();
                g_Path.clear();

                Point start = { g_pStartNode->x, g_pStartNode->y };
                Point end = { g_pEndNode->x, g_pEndNode->y };
                PathFinder finder((char*)g_Tile, GRID_WIDTH, GRID_HEIGHT, (int*)g_SearchIDMarker);

                g_Path = finder.JPS_PathFind(g_pStartNode, g_pEndNode);
                g_ClosedList = finder.ReleaseClosedList();

                if (!g_Path.empty())
                {
                    g_SmoothedPath = SmoothPath(g_Path);
                }
                

                InvalidateRect(hWnd, NULL, false); // 화면 갱신
            }

        }

        else if (wParam == 'S')
        {
            if (g_pStartNode == nullptr || g_pEndNode == nullptr) 
                break;

  
            if (g_SearchState == NOT_STARTED)
            {
                delete g_pPathFinder; // 이전 탐색이 있다면 정리
                g_pPathFinder = new PathFinder((char*)g_Tile, GRID_WIDTH, GRID_HEIGHT, (int*)g_SearchIDMarker);
                g_pPathFinder->Initialize(g_pStartNode, g_pEndNode);

                g_SearchState = IN_PROGRESS;
                g_bStepMode = true; // OpenList를 그리는 모드 활성화
            }

            if (g_SearchState == IN_PROGRESS)
            {
                bool isFinished = g_pPathFinder->ExecuteNextStep(&g_Path);
                if (isFinished)
                {
                    g_SearchState = FINISHED;
                    g_bStepMode = false; // 탐색이 끝나면 OpenList는 그리지 않음
                }

                // 3. 렌더링을 위해 데이터 복사
                g_ClosedList = g_pPathFinder->GetClosedList();

                g_OpenList.clear(); //이전 프레임에서 그린 흔적을 지우는 용도 WM_PAINT 확인 필요
                auto tempOpenList = g_pPathFinder->GetOpenList(); // 복사본 생성
                while (!tempOpenList.empty())
                {
                    g_OpenList.push_back(tempOpenList.top());
                    tempOpenList.pop();
                }

                if (!g_Path.empty())
                {
                    g_SmoothedPath = SmoothPath(g_Path);
                }

                InvalidateRect(hWnd, NULL, false); // 화면 갱신
            }
            break;
        }
        else if (wParam == 'C')
        {
            delete g_pStartNode;
            delete g_pEndNode;
            g_pStartNode = nullptr;
            g_pEndNode = nullptr;

            delete g_pPathFinder;
            g_pPathFinder = nullptr;

            g_Path.clear(); // g_Path는 g_ClosedList의 일부이므로 별도 해제 불필요
            g_ClosedList.clear();
            g_OpenList.clear();
            g_SmoothedPath.clear();

            g_SearchState = NOT_STARTED;
            g_bStepMode = false;

            memset(g_SearchIDMarker, 0, sizeof(g_SearchIDMarker));
            g_currentSearchID = 0;
            memset(g_Tile, 0, sizeof(g_Tile)); // 맵 클리어

            InvalidateRect(hWnd, NULL, false);
        }
        // 방향키로 팬 이동
        else if (wParam == VK_LEFT)
        {
            g_iOffsetX += 40;
            InvalidateRect(hWnd, NULL, FALSE);
        }
        else if (wParam == VK_RIGHT)
        {
            g_iOffsetX -= 40;
            InvalidateRect(hWnd, NULL, FALSE);
        }
        else if (wParam == VK_UP)
        {
            g_iOffsetY += 40;
            InvalidateRect(hWnd, NULL, FALSE);
        }
        else if (wParam == VK_DOWN)
        {
            g_iOffsetY -= 40;
            InvalidateRect(hWnd, NULL, FALSE);
        }
    }
    break;
    case WM_DESTROY:
    {
        // 생성한 GDI 오브젝트 해제
        DeleteObject(g_hGridPen);
        DeleteObject(g_hTileBrush);
        DeleteObject(g_hStartBrush);
        DeleteObject(g_hEndBrush);
        DeleteObject(g_hPathBrush);
        DeleteObject(g_hClosedBrush);
        DeleteObject(g_hLineBrush);

        if (g_hFont)
            DeleteObject(g_hFont);

        SelectObject(g_hMemDC, g_hMemDCBitmap_Old);
        DeleteObject(g_hMemDC);
        DeleteObject(g_hMemDCBitmap);

        // 동적 할당된 노드들 해제
        delete g_pStartNode;
        delete g_pEndNode;
        delete g_pPathFinder;
        for (auto const& pair : g_SearchIDBrushMap)
        {
            DeleteObject(pair.second);
        }
        g_SearchIDBrushMap.clear();

        PostQuitMessage(0);
    }
    break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}



//누수는 없는지
//카메라 좌표 복기
//필요없는 코드가 있는지
//소멸자 이슈
