#include "Render.h"
#include <windowsx.h>
#include <vector>
#include <cmath>
#include <debugapi.h> // 디버그 출력을 위한 헤더


std::vector<Point> Bresenham(Point start, Point end)
{
    std::vector<Point> coordinates;

    int x1 = start.x;
    int y1 = start.y;
    int x2 = end.x;
    int y2 = end.y;

    const int dx = std::abs(x2 - x1);
    const int dy = std::abs(y2 - y1);
    const int sx = (x1 < x2) ? 1 : -1;
    const int sy = (y1 < y2) ? 1 : -1;

    int currentX = x1;
    int currentY = y1;

    if (dx >= dy)
    {
        int err = dx / 2;
        while (currentX != x2) 
        {
            coordinates.push_back({ currentX, currentY });
            err -= dy;
            if (err < 0) 
            {
                currentY += sy;
                err += dx;
            }
            currentX += sx;
        }
    }
    else {
        int err = dy / 2;
        while (currentY != y2) {
            coordinates.push_back({ currentX, currentY });
            err -= dx;
            if (err < 0) {
                currentX += sx;
                err += dy;
            }
            currentY += sy;
        }
    }
    coordinates.push_back({ currentX, currentY });

    return coordinates;
}


bool IsPathClear(Point start, Point end)
{
    std::vector<Point> path = Bresenham(start, end);

    for (const auto& p : path)
    {
        if (p.x < 0 || p.x >= GRID_WIDTH || p.y < 0 || p.y >= GRID_HEIGHT)
        {
            return false; 
        }
        if (g_Tile[p.y][p.x] == 1)
        {
            return false;
        }
    }


    return true;
}


void RenderGrid(HDC hdc)
{
    int scaledGridSize = GetScaledGridSize();
    int iX = 0 + g_iOffsetX;
    int iY = 0 + g_iOffsetY;

    HPEN hOldPen = (HPEN)SelectObject(hdc, g_hGridPen);

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

/*
void DrawNodeText(HDC hdc, Node* node, COLORREF textColor)
{
    if (g_fZoomLevel < 2.0f)
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
        sprintf_s(gText, "G:%.1f", node->G);
        sprintf_s(hText, "H:%.0f", node->H);
        sprintf_s(fText, "F:%.1f", node->F);

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
*/


void RenderJPSResult(HDC hdc)
{
    int scaledGridSize = GetScaledGridSize();

    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, g_hTileBrush);
    SelectObject(hdc, GetStockObject(NULL_PEN));

    if (g_bStepMode && !g_OpenList.empty())
    {
        HBRUSH hOpenBrush = CreateSolidBrush(RGB(0, 255, 255));
        SelectObject(hdc, hOpenBrush);

        for (Node* node : g_OpenList)
        {
            int screenX, screenY;
            GridToScreen(node->x, node->y, screenX, screenY);
            Rectangle(hdc, screenX, screenY, screenX + scaledGridSize, screenY + scaledGridSize);
            //DrawNodeText(hdc, node, RGB(0, 0, 128));
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
        //DrawNodeText(hdc, node, RGB(0, 0, 255));
    }

    // 경로를 선으로 연결해서 그리기
    if (g_Path.size() > 1) //경로상의 점이 2개이상이면 코드 실행
    {
        int penWidth = max(2, (int)(3 * g_fZoomLevel)); //선의 두께 확대할수록 두껍게
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


           // DrawNodeText(hdc, node, RGB(0, 0, 0)); // 검은색 텍스트
        }

        SelectObject(hdc, hOldPen);
        DeleteObject(hPathNodePen);
    }

    // 시작점 그리기 (가장 위에)
    if (g_pStartNode)
    {
        SelectObject(hdc, g_hStartBrush);
        int screenX, screenY;
        GridToScreen(g_pStartNode->x, g_pStartNode->y, screenX, screenY);
        Rectangle(hdc, screenX, screenY, screenX + scaledGridSize, screenY + scaledGridSize);
    }
    // 도착점 그리기
    if (g_pEndNode)
    {
        SelectObject(hdc, g_hEndBrush);
        int screenX, screenY;
        GridToScreen(g_pEndNode->x, g_pEndNode->y, screenX, screenY);
        Rectangle(hdc, screenX, screenY, screenX + scaledGridSize, screenY + scaledGridSize);
    }

    SelectObject(hdc, hOldBrush);
}

COLORREF GetRandomColor()
{

    static std::random_device rd;  
    static std::mt19937 gen(rd()); 
    static std::uniform_int_distribution<int> dis(50, 200); 

    // R, G, B 값을 각각 무작위로 추출
    int r = dis(gen);
    int g = dis(gen);
    int b = dis(gen);

    return RGB(r, g, b); 
}

void RenderSearchedArea(HDC hdc)
{
    int scaledGridSize = GetScaledGridSize();
    SelectObject(hdc, GetStockObject(NULL_PEN));
  
    for (int y = 0; y < GRID_HEIGHT; ++y)
    {
        for (int x = 0; x < GRID_WIDTH; ++x)
        {
            if (g_SearchIDMarker[y][x] > 0)
            {
                int searchID = g_SearchIDMarker[y][x];
                HBRUSH hCurrentBrush = NULL;

                if (g_SearchIDBrushMap.find(searchID) == g_SearchIDBrushMap.end())
                {
                    COLORREF randomColor = GetRandomColor();
                    hCurrentBrush = CreateSolidBrush(randomColor);
                    g_SearchIDBrushMap[searchID] = hCurrentBrush;
                }
                else
                {
                    hCurrentBrush = g_SearchIDBrushMap[searchID];
                }

                HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hCurrentBrush);
                int screenX, screenY;
                GridToScreen(x, y, screenX, screenY);
                Rectangle(hdc, screenX, screenY, screenX + scaledGridSize, screenY + scaledGridSize);
                SelectObject(hdc, hOldBrush);
            }
        }
    }

  
}


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


std::vector<Node*> SmoothPath(const std::vector<Node*>& originalPath)
{

    if (originalPath.size() <= 2)
    {
        return originalPath;
    }

    std::vector<Node*> smoothedPath;
    smoothedPath.push_back(originalPath[0]); 

    int currentIndex = 0;
    while (currentIndex < originalPath.size() - 1)
    {
        int lastVisibleIndex = currentIndex + 1;
        for (int i = currentIndex + 2; i < originalPath.size(); i++)
        {
            Point startPoint = { originalPath[currentIndex]->x, originalPath[currentIndex]->y };
            Point endPoint = { originalPath[i]->x, originalPath[i]->y };

            if (IsPathClear(startPoint, endPoint))
            {
                lastVisibleIndex = i;
            }
            else
            {
                break;
            }
        }

        smoothedPath.push_back(originalPath[lastVisibleIndex]);
        currentIndex = lastVisibleIndex;
    }

    return smoothedPath;
}


void RenderSmoothedPath(HDC hdc)
{
    // 다듬은 경로(g_SmoothedPath)를 핫핑크색 선으로 연결해서 그리기
    if (g_SmoothedPath.size() > 1)
    {
        int scaledGridSize = GetScaledGridSize();
        int penWidth = max(2, (int)(3 * g_fZoomLevel));
        // 펜 색상을 핫핑크로 변경
        HPEN hPathPen = CreatePen(PS_SOLID, penWidth, RGB(255, 105, 180));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPathPen);

        for (size_t i = 1; i < g_SmoothedPath.size(); ++i)
        {
            int x1, y1, x2, y2;
            GridToScreen(g_SmoothedPath[i - 1]->x, g_SmoothedPath[i - 1]->y, x1, y1);
            GridToScreen(g_SmoothedPath[i]->x, g_SmoothedPath[i]->y, x2, y2);

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
}
