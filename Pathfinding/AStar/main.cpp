// Astar_Algorithm.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#include<iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include "framework.h"
#include "Astar_Algorithm.h"
#include <windowsx.h>


#define MAX_LOADSTRING 100
#define GRID_SIZE 16    // 각 그리드 셀의 픽셀 크기
#define GRID_WIDTH 100
#define GRID_HEIGHT 50

struct Node
{
    int y, x;
    Node* Parent;
    int G, H, F;
    Node(int _y, int _x)
        : y(_y), x(_x), G(0), H(0), F(0), Parent(nullptr) {
    }

    void Calculate_F()
    {
        F = G + H;
    }
};


HBRUSH g_hTileBrush;    //장애물 칠하는 브러시
HBRUSH g_hStartBrush;   //시작점 칠하는 브러시
HBRUSH g_hEndBrush; //도착점 칠하는 브러시
HBRUSH g_hPathBrush;    //경로 칠하는 브러시
HBRUSH g_hClosedBrush;  //탐색된 노드 칠하는 브러시
HPEN g_hGridPen;    //그리드 선 그리는 펜

char g_Tile[GRID_HEIGHT][GRID_WIDTH];   //0 장애물 없음 / 1 장애물 있음
bool g_bErase = false;  //지우기 모드지 확인
bool g_bDrag = false;   //마우스 드래그 중인지 확인


std::vector<Node*> g_OpenList;  //탐색 대기 노드들 (연한 초록색)
bool g_bStepMode = false;       // 단계별 모드 여부
Node* g_StepStartNode = nullptr;    //단계별이 뭐지. 단계별 모드용 시작노드
Node* g_StepEndNode = nullptr;  // 단계별 모드용 끝 노드

// A* 상태 관리 전역 변수
Node* g_pStartNode = nullptr;   //사용자가 설정한 시작노드
Node* g_pEndNode = nullptr; //사용자가 설정한 도착 노드
std::vector<Node*> g_Path;  //최종 경로
std::vector<Node*> g_ClosedList;    //이미 탐색한 노드들


// 향상된 시각화를 위한 추가 전역 변수들
float g_fZoomLevel = 1.0f;      // 줌 레벨
int g_iOffsetX = 0;             // 화면 오프셋 X
int g_iOffsetY = 0;             // 화면 오프셋 Y
bool g_bShowValues = false;     // 가중치 값 표시 여부
HFONT g_hFont = nullptr;        // 텍스트 출력용 폰트


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



int Manhatten(int y1, int x1, int y2, int x2)
{
    // 10을 곱하는 이유: 직선 이동 비용이 10이므로 일관성 유지
    return (std::abs(y1 - y2) + std::abs(x1 - x2)) * 10;
}


std::vector<Node*> AstarPathFind(
    char(*map)[GRID_WIDTH], //맵 데이터
    Node* start,
    Node* end,
    std::vector<Node*>* outClosedList)  //탐색한 노드 반환용
{

    std::vector<Node*> OpenList;
    std::vector<Node*> ClosedList;
    std::vector<Node*> path;

    OpenList.push_back(start);

    while (!OpenList.empty())
    {
        //openlist에서 f값이 가장 작는 노드를 찾는다
        Node* currentNode = OpenList[0];
        int currentIndex = 0;
        for (int i = 1; i < OpenList.size(); ++i)
        {
            if (OpenList[i]->F < currentNode->F)
            {
                currentNode = OpenList[i];
                currentIndex = i;
            }
        }

        // 찾은 노드를 OpenList에서 빼고 Close 리스트에 넣는다.
        OpenList.erase(OpenList.begin() + currentIndex);
        ClosedList.push_back(currentNode);

        //목적지를 만나면 종료
        if (currentNode->y == end->y && currentNode->x == end->x)
        {
            Node* temp = currentNode;
            while (temp != nullptr) //출발지 노드의 부모는 노드이기에
            {
                path.push_back(temp);
                temp = temp->Parent;
            }
            //목적지 노드-> 목적지 바로 전노드 -> ... -> 출발지노드
            std::reverse(path.begin(), path.end()); // 경로를 시작점으로부터로 뒤집기

            // OpenList에 남은 노드 메모리 해제
            for (Node* node : OpenList)
                delete node;

            delete end;

            // 최종 클로즈 리스트를 외부로 복사
            *outClosedList = ClosedList;

            return path;
        }

        //주변 여덟방향 노드 확인
        for (int dy = -1; dy <= 1; ++dy)
        {
            for (int dx = -1; dx <= 1; ++dx)
            {
                if (dy == 0 && dx == 0)
                    continue;

                int nextY = currentNode->y + dy;
                int nextX = currentNode->x + dx;

                // 맵 범위를 벗어나는지 확인
                if (nextY < 0 || nextY >= GRID_HEIGHT || nextX < 0 || nextX >= GRID_WIDTH)
                    continue;

                //벽인지 확인
                if (map[nextY][nextX] == 1)
                {
                    continue;
                }

                //대각선 통과 막기
                bool isDiagonal = (dx != 0 && dy != 0); //둘다 0이 아니라면 대각선 이동을 의미
                if (isDiagonal)
                {
                    // 대각선 이동 시 인접한 두 칸이 모두 비어있어야 함
                    // 예: (0,0) -> (1,1)로 이동할 때 (0,1)과 (1,0) 모두 비어있어야 함
                    int checkX1 = currentNode->x + dx;  // 가로 방향 체크
                    int checkX2 = currentNode->x;       // 현재 X  
                    int checkY2 = currentNode->y + dy;  // 세로 방향 체크
                    int checkY1 = currentNode->y;       // 현재 Y



                    if (map[checkY1][checkX1] == 1 || map[checkY2][checkX2] == 1)
                        continue;
                }

                //이동 비용 계산
                int moveCost = (dx != 0 && dy != 0) ? 14 : 10;

                Node* neighborNode = new Node(nextY, nextX);
                neighborNode->Parent = currentNode;
                neighborNode->G = currentNode->G + moveCost;
                neighborNode->H = Manhatten(nextY, nextX, end->y, end->x);
                neighborNode->Calculate_F();

                //ClosedList에 있는지 확인
                bool inClosed = false;
                for (Node* node : ClosedList)
                {
                    if (node->y == nextY && node->x == nextX)
                    {
                        inClosed = true;
                        break;
                    }
                }
                if (inClosed)
                {
                    delete neighborNode;
                    continue;
                }
                //OpenList에 있는지 확인하고 있따면 g값이 더 작은 경우에만 업데이트
                bool inOpen = false;
                for (Node* node : OpenList)
                {
                    if (node->y == nextY && node->x == nextX)
                    {
                        inOpen = true;
                        //더 좋은 길인지 탐색
                        if (neighborNode->G < node->G)
                        {
                            node->G = neighborNode->G;
                            node->Parent = currentNode;
                            node->Calculate_F();
                        }
                        break;
                    }
                }
                if (!inOpen)
                {
                    OpenList.push_back(neighborNode);
                }
                else
                {
                    delete neighborNode; //업데이트가 필요 없는 중복 노드는 삭제
                }
            }
        }

    }
    // 경로 못찾았을 때 메모리 정리
    for (Node* node : OpenList)
        delete node;

    delete end;

    *outClosedList = ClosedList;

    return path;
}


//확대된 그리드 사이즈 계산
int GetScaledGridSize()
{
    return (int)(GRID_SIZE * g_fZoomLevel);
}


// 화면 좌표를 그리드 좌표로 변환
void ScreenToGrid(int screenX, int screenY, int& gridX, int& gridY)
{
    int scaledGridSize = GetScaledGridSize();
    gridX = (screenX - g_iOffsetX) / scaledGridSize;
    gridY = (screenY - g_iOffsetY) / scaledGridSize;
}


// 그리드 좌표를 화면 좌표로 변환
void GridToScreen(int gridX, int gridY, int& screenX, int& screenY)
{
    int scaledGridSize = GetScaledGridSize();
    screenX = gridX * scaledGridSize + g_iOffsetX;
    screenY = gridY * scaledGridSize + g_iOffsetY;
}


void RenderGrid(HDC hdc)
{
    int scaledGridSize = GetScaledGridSize();
    int iX = 0 + g_iOffsetX;
    int iY = 0 + g_iOffsetY;

    HPEN hOldPen = (HPEN)SelectObject(hdc,g_hGridPen);

    RECT clientRect;
    GetClientRect(WindowFromDC(hdc), &clientRect);

    // 세로선 그리기
    for (int iCntW = 0; iCntW <= GRID_WIDTH; iCntW++)
    {
        MoveToEx(hdc, iX, g_iOffsetY, NULL);
        LineTo(hdc, iX, GRID_HEIGHT * scaledGridSize + g_iOffsetY);
        iX += scaledGridSize;
    }

    // 가로선 그리기
    iY = 0 + g_iOffsetY;
    for (int iCntH = 0; iCntH <= GRID_HEIGHT; iCntH++)
    {
        MoveToEx(hdc, g_iOffsetX, iY, NULL);
        LineTo(hdc, GRID_WIDTH * scaledGridSize + g_iOffsetX, iY);
        iY += scaledGridSize;
    }

    SelectObject(hdc, hOldPen);
}


void RenderObstacle(HDC hdc)
{

    int scaledGridSize = GetScaledGridSize();
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, g_hTileBrush);
    SelectObject(hdc, GetStockObject(NULL_PEN));
    // 사각형의 테두리를 안보이도록 하기 위해 NULL_PEN 지정
    // CreatePen으로 NULL_PEN도 가능하지만 GetStockObject를 사용하여 시스템에 만들어져 있는
    // GDI Object 사용
    // GetStockObjectsms 시스템의 고정적인 범용 GDI Object로서 삭제가 필요 없음
    // 시스템 전역적인 GDI Object를 얻어서 사용한다는 개념

    for (int iCntW = 0; iCntW < GRID_WIDTH; iCntW++)
    {
        for (int iCntH = 0; iCntH < GRID_HEIGHT; iCntH++)
        {
            if (g_Tile[iCntH][iCntW])
            {
                int iX = iCntW * scaledGridSize + g_iOffsetX;
                int iY = iCntH * scaledGridSize + g_iOffsetY;
                //테두리 크기 때문에 +2 포함
                Rectangle(hdc, iX, iY, iX + scaledGridSize + 2, iY + scaledGridSize + 2);
            }
        }
    }

    SelectObject(hdc, hOldBrush);
}


void DrawNodeText(HDC hdc, Node* node, COLORREF textColor)
{
    if ( g_fZoomLevel < 2.0f) 
        return; // 충분히 확대됐을 때만 표시

    int scaledGridSize = GetScaledGridSize();
    int screenX, screenY;
    GridToScreen(node->x, node->y, screenX, screenY);

    if (g_hFont)
    {
        HFONT hOldFont = (HFONT)SelectObject(hdc, g_hFont);
        SetTextColor(hdc, textColor);
        SetBkMode(hdc, TRANSPARENT);

        // 각 값을 개별 문자열로 준비
        char gText[20], hText[20], fText[20];
        sprintf_s(gText, "G:%d", node->G);
        sprintf_s(hText, "H:%d", node->H);
        sprintf_s(fText, "F:%d", node->F);

        // 그리드를 3등분하여 각각의 영역 계산
        int thirdHeight = scaledGridSize / 3;

        // 상단 영역 (G값)
        RECT topRect;
        topRect.left = screenX + 2;
        topRect.top = screenY + 2;
        topRect.right = screenX + scaledGridSize - 2;
        topRect.bottom = screenY + thirdHeight;
        DrawTextA(hdc, gText, -1, &topRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        // 중단 영역 (H값)
        RECT middleRect;
        middleRect.left = screenX + 2;
        middleRect.top = screenY + thirdHeight;
        middleRect.right = screenX + scaledGridSize - 2;
        middleRect.bottom = screenY + thirdHeight * 2;
        DrawTextA(hdc, hText, -1, &middleRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        // 하단 영역 (F값)
        RECT bottomRect;
        bottomRect.left = screenX + 2;
        bottomRect.top = screenY + thirdHeight * 2;
        bottomRect.right = screenX + scaledGridSize - 2;
        bottomRect.bottom = screenY + scaledGridSize - 2;
        DrawTextA(hdc, fText, -1, &bottomRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        SelectObject(hdc, hOldFont);
    }
}



void RenderAstarResult(HDC hdc)
{
    int scaledGridSize = GetScaledGridSize();
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, g_hTileBrush);
    SelectObject(hdc, GetStockObject(NULL_PEN));

    // OpenList 그리기 (연한 녹색)
    if (g_bStepMode && !g_OpenList.empty())
    {
        HBRUSH hOpenBrush = CreateSolidBrush(RGB(144, 238, 144));
        SelectObject(hdc, hOpenBrush);

        for (Node* node : g_OpenList)
        {
            int screenX, screenY;
            GridToScreen(node->x, node->y, screenX, screenY);
            Rectangle(hdc, screenX, screenY, screenX + scaledGridSize, screenY + scaledGridSize);
            DrawNodeText(hdc, node, RGB(0, 100, 0));
        }
        DeleteObject(hOpenBrush);
    }
    // 클로즈 리스트 그리기 (탐색된 노드)
    SelectObject(hdc, g_hClosedBrush);

    for (Node* node : g_ClosedList)
    {
        int screenX, screenY;
        GridToScreen(node->x, node->y, screenX, screenY);
        Rectangle(hdc, screenX, screenY, screenX + scaledGridSize, screenY + scaledGridSize);
        DrawNodeText(hdc, node, RGB(255, 255, 255));
    }

    // 2. 경로를 선으로 연결해서 그리기
    if (g_Path.size() > 1)
    {
        int penWidth = max(2, (int)(3 * g_fZoomLevel));
        HPEN hPathPen = CreatePen(PS_SOLID, penWidth, RGB(255, 255, 0)); // 노란색 선
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPathPen);

        for (size_t i = 1; i < g_Path.size(); ++i)
        {
            int x1, y1, x2, y2;
            GridToScreen(g_Path[i - 1]->x, g_Path[i - 1]->y, x1, y1);
            GridToScreen(g_Path[i]->x, g_Path[i]->y, x2, y2);

            // 그리드 중앙에서 중앙으로 선 그리기
            x1 += scaledGridSize / 2;
            y1 += scaledGridSize / 2;
            x2 += scaledGridSize / 2;
            y2 += scaledGridSize / 2;

            MoveToEx(hdc, x1, y1, NULL);
            LineTo(hdc, x2, y2);
        }

        SelectObject(hdc, hOldPen);
        DeleteObject(hPathPen);
    }
    if (g_Path.size() > 0)
    {
        SelectObject(hdc, g_hPathBrush);
        HPEN hPathNodePen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPathNodePen);

        for (Node* node : g_Path)
        {
            int screenX, screenY;
            GridToScreen(node->x, node->y, screenX, screenY);
            int centerX = screenX + scaledGridSize / 2;
            int centerY = screenY + scaledGridSize / 2;
            int radius = max(3, scaledGridSize / 6);

            Ellipse(hdc, centerX - radius, centerY - radius, centerX + radius, centerY + radius);

            // 확대 시 가중치 값 표시
            DrawNodeText(hdc, node, RGB(0, 0, 0)); // 검은색 텍스트
        }

        SelectObject(hdc, hOldPen);
        DeleteObject(hPathNodePen);
    }

    // 4. 시작점 그리기 (가장 위에)
    if (g_pStartNode)
    {
        SelectObject(hdc, g_hStartBrush);
        int screenX, screenY;
        GridToScreen(g_pStartNode->x, g_pStartNode->y, screenX, screenY);
        Rectangle(hdc, screenX, screenY, screenX + scaledGridSize, screenY + scaledGridSize);

        // 'S' 문자 표시
        if (g_fZoomLevel >= 1.5f)
        {
            SetTextColor(hdc, RGB(255, 255, 255));
            SetBkMode(hdc, TRANSPARENT);
            RECT textRect = { screenX, screenY, screenX + scaledGridSize, screenY + scaledGridSize };
            DrawTextA(hdc, "S", -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
    }
    // 도착점 그리기
    if (g_pEndNode)
    {
        SelectObject(hdc, g_hEndBrush);
        int screenX, screenY;
        GridToScreen(g_pEndNode->x, g_pEndNode->y, screenX, screenY);
        Rectangle(hdc, screenX, screenY, screenX + scaledGridSize, screenY + scaledGridSize);

        // 'E' 문자 표시
        if (g_fZoomLevel >= 1.5f)
        {
            SetTextColor(hdc, RGB(255, 255, 255));
            SetBkMode(hdc, TRANSPARENT);
            RECT textRect = { screenX, screenY, screenX + scaledGridSize, screenY + scaledGridSize };
            DrawTextA(hdc, "E", -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
    }

    SelectObject(hdc, hOldBrush);
}


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
    LoadStringW(hInstance, IDC_ASTARALGORITHM, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ASTARALGORITHM));

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

    return (int) msg.wParam;
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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ASTARALGORITHM));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ASTARALGORITHM);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
        int iTileX;// = xPos / GRID_SIZE;
        int iTileY;// = yPos / GRID_SIZE;
        ScreenToGrid(xPos, yPos, iTileX, iTileY);

        if (iTileX >= 0 && iTileX < GRID_WIDTH && iTileY >= 0 && iTileY < GRID_HEIGHT)
        {
            if (g_Tile[iTileY][iTileX] == 0) // 장애물이 아닌 곳에만 지정 가능
            {

                // 새로운 시작점/도착점을 설정할 때마다 단계별 모드 데이터 초기화
                g_bStepMode = false;

                // 기존 단계별 데이터 정리
                for (Node* node : g_OpenList)
                    delete node;
                g_OpenList.clear();

                for (Node* node : g_ClosedList)
                    delete node;
                g_ClosedList.clear();

                if (g_StepEndNode)
                {
                    delete g_StepEndNode;
                    g_StepEndNode = nullptr;
                }

                g_Path.clear(); // 기존 경로도 초기화

                if (g_pStartNode == nullptr)
                {
                    g_pStartNode = new Node(iTileY, iTileX);
                }
                else if (g_pEndNode == nullptr)
                {
                    g_pEndNode = new Node(iTileY, iTileX);
                }
                else // 둘 다 있으면 초기화
                {
                    delete g_pStartNode;
                    delete g_pEndNode;
                    g_pStartNode = new Node(iTileY, iTileX);
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
        g_hStartBrush = CreateSolidBrush(RGB(0, 255, 0)); // 시작점 (초록)
        g_hEndBrush = CreateSolidBrush(RGB(255, 0, 0));   // 도착점 (빨강)
        g_hPathBrush = CreateSolidBrush(RGB(255, 255, 0)); // 경로 (노랑)
        g_hClosedBrush = CreateSolidBrush(RGB(0, 128, 255)); // 탐색된 곳 (파랑)

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
            RenderAstarResult(g_hMemDC);
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
        if (wParam == VK_SPACE) // 스페이스바: 한 스텝씩 실행
        {
            if (g_pStartNode != nullptr && g_pEndNode != nullptr)
            {
                if (!g_Path.empty() && !g_bStepMode)
                {
                    break; // 경로 찾기 완료 상태에서는 아무것도 하지 않음
                }
                // 첫 실행이면 초기화
                if (!g_bStepMode)
                {
                    // 기존 데이터 정리
                    g_Path.clear();
                    for (Node* node : g_ClosedList)
                        delete node;
                    g_ClosedList.clear();

                    for (Node* node : g_OpenList)
                        delete node;
                    g_OpenList.clear();

                    // 시작 노드를 OpenList에 추가
                    g_StepStartNode = new Node(g_pStartNode->y, g_pStartNode->x);
                    g_StepEndNode = new Node(g_pEndNode->y, g_pEndNode->x);

                    g_OpenList.push_back(g_StepStartNode);
                    g_bStepMode = true;

                    InvalidateRect(hWnd, NULL, false);
                }
                // 한 스텝 실행 (초기화 직후든 아니든 항상 실행)
                if (g_bStepMode && !g_OpenList.empty() && g_Path.empty())
                {
                    // OpenList에서 F값이 가장 작은 노드 찾기
                    Node* currentNode = g_OpenList[0];
                    int currentIndex = 0;
                    for (int i = 1; i < g_OpenList.size(); ++i)
                    {
                        if (g_OpenList[i]->F < currentNode->F)
                        {
                            currentNode = g_OpenList[i];
                            currentIndex = i;
                        }
                    }

                    // 찾은 노드를 OpenList에서 빼고 ClosedList에 넣기
                    g_OpenList.erase(g_OpenList.begin() + currentIndex);
                    g_ClosedList.push_back(currentNode);

                    // 목적지 도달 확인
                    if (currentNode->y == g_StepEndNode->y && currentNode->x == g_StepEndNode->x)
                    {
                        // 경로 구성
                        Node* temp = currentNode;
                        while (temp != nullptr)
                        {
                            g_Path.push_back(temp);
                            temp = temp->Parent;
                        }
                        std::reverse(g_Path.begin(), g_Path.end());

                        // OpenList 정리
                        for (Node* node : g_OpenList)
                            delete node;
                        g_OpenList.clear();

                        delete g_StepEndNode;
                        g_StepEndNode = nullptr;
                        g_bStepMode = false;
                    }
                    else
                    {
                        // 주변 8방향 노드 확인
                        for (int dy = -1; dy <= 1; ++dy)
                        {
                            for (int dx = -1; dx <= 1; ++dx)
                            {
                                if (dy == 0 && dx == 0)
                                    continue;

                                int nextY = currentNode->y + dy;
                                int nextX = currentNode->x + dx;

                                // 맵 범위 확인
                                if (nextY < 0 || nextY >= GRID_HEIGHT || nextX < 0 || nextX >= GRID_WIDTH)
                                    continue;

                                // 벽 확인
                                if (g_Tile[nextY][nextX] == 1)
                                    continue;

                                // 대각선 이동 시 인접 칸 확인
                                bool isDiagonal = (dx != 0 && dy != 0);
                                if (isDiagonal)
                                {
                                    int checkX1 = currentNode->x + dx;
                                    int checkY1 = currentNode->y;
                                    int checkX2 = currentNode->x;
                                    int checkY2 = currentNode->y + dy;

                                    if (g_Tile[checkY1][checkX1] == 1 || g_Tile[checkY2][checkX2] == 1)
                                        continue;
                                }

                                int moveCost = isDiagonal ? 14 : 10;

                                Node* neighborNode = new Node(nextY, nextX);
                                neighborNode->Parent = currentNode;
                                neighborNode->G = currentNode->G + moveCost;
                                neighborNode->H = Manhatten(nextY, nextX, g_StepEndNode->y, g_StepEndNode->x);
                                neighborNode->Calculate_F();

                                // ClosedList에 있는지 확인
                                bool inClosed = false;
                                for (Node* node : g_ClosedList)
                                {
                                    if (node->y == nextY && node->x == nextX)
                                    {
                                        inClosed = true;
                                        break;
                                    }
                                }

                                if (inClosed)
                                {
                                    delete neighborNode;
                                    continue;
                                }

                                // OpenList에 있는지 확인
                                bool inOpen = false;
                                for (Node* node : g_OpenList)
                                {
                                    if (node->y == nextY && node->x == nextX)
                                    {
                                        inOpen = true;
                                        if (neighborNode->G < node->G)
                                        {
                                            node->G = neighborNode->G;
                                            node->Parent = currentNode;
                                            node->Calculate_F();
                                        }
                                        break;
                                    }
                                }

                                if (!inOpen)
                                {
                                    g_OpenList.push_back(neighborNode);
                                }
                                else
                                {
                                    delete neighborNode;
                                }
                            }
                        }
                    }
                }
                else if (g_bStepMode && g_OpenList.empty() && g_Path.empty())
                {
                    // OpenList가 비어있으면 경로 없음
                    g_bStepMode = false;
                    if (g_StepEndNode)
                    {
                        delete g_StepEndNode;
                        g_StepEndNode = nullptr;
                    }
                }

                InvalidateRect(hWnd, NULL, false);
            }
        }
        if (wParam == 'A') // 스페이스 바를 누르면 길찾기 시작
        {
            if (g_pStartNode != nullptr && g_pEndNode != nullptr)
            {
                g_Path.clear();
                for (Node* node : g_ClosedList) 
                    delete node;
                g_ClosedList.clear();
                if (g_StepEndNode)
                {
                    delete g_StepEndNode;
                    g_StepEndNode = nullptr;
                }
                g_bStepMode = false;
                for (Node* node : g_OpenList)
                    delete node;
                g_OpenList.clear();
                // A* 실행 - 새로운 노드 생성해서 전달
                Node* startCopy = new Node(g_pStartNode->y,g_pStartNode->x);
                Node* endCopy = new Node(g_pEndNode->y,g_pEndNode->x);

                g_Path = AstarPathFind(g_Tile, startCopy, endCopy, &g_ClosedList);

               
                InvalidateRect(hWnd, NULL, false); // 화면 갱신
            }
            
        }

        else if (wParam == 'C') 
        {
            delete g_pStartNode;
            delete g_pEndNode;
            g_pStartNode = nullptr;
            g_pEndNode = nullptr;

            g_Path.clear(); // g_Path는 g_ClosedList의 일부이므로 별도 해제 불필요
            for (Node* node : g_ClosedList) 
                delete node;
            g_ClosedList.clear();
          
            // 단계별 모드 관련 데이터 초기화 (연한 초록색 부분)
            g_bStepMode = false;
            for (Node* node : g_OpenList)
                delete node;
            g_OpenList.clear();

            if (g_StepEndNode)
            {
                delete g_StepEndNode;
                g_StepEndNode = nullptr;
            }
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
        if (g_hFont)
            DeleteObject(g_hFont);

        SelectObject(g_hMemDC, g_hMemDCBitmap_Old);
        DeleteObject(g_hMemDC);
        DeleteObject(g_hMemDCBitmap);
   
        // 동적 할당된 노드들 해제
        delete g_pStartNode;
        delete g_pEndNode;
        for (Node* node : g_ClosedList)
            delete node;

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
