#pragma once
#include "framework.h"
#include "JPS.h"
#include <vector>
#include <map>
#include <random>

struct Point
{
	int x, y;
};

constexpr int GRID_SIZE = 16;
constexpr int GRID_WIDTH = 100;
constexpr int GRID_HEIGHT = 50;


extern HBRUSH g_hTileBrush;
extern HBRUSH g_hStartBrush;
extern HBRUSH g_hEndBrush;
extern HBRUSH g_hPathBrush;
extern HBRUSH g_hClosedBrush;
extern HBRUSH g_hLineBrush;
extern HPEN g_hGridPen;
extern HFONT g_hFont;


// 타일 및 노드 데이터
extern char g_Tile[GRID_HEIGHT][GRID_WIDTH];
extern Node* g_pStartNode;
extern Node* g_pEndNode;
extern std::vector<Node*> g_Path;
extern std::vector<Node*> g_ClosedList;
extern std::vector<Node*> g_OpenList;
extern std::vector<Node*> g_SmoothedPath;

extern bool g_bStepMode;
extern std::map<int, HBRUSH> g_SearchIDBrushMap;


// 시각화 및 뷰포트
extern float g_fZoomLevel;
extern int g_iOffsetX;
extern int g_iOffsetY;
extern int g_SearchIDMarker[GRID_HEIGHT][GRID_WIDTH];


void RenderGrid(HDC hdc);
void RenderObstacle(HDC hdc);
//void DrawNodeText(HDC hdc, Node* node, COLORREF textColor);
void RenderJPSResult(HDC hdc);
void RenderSearchedArea(HDC hdc);
bool IsPathClear(Point start, Point end);
void RenderSmoothedPath(HDC hdc);
std::vector<Point> Bresenham(Point start, Point end);
std::vector<Node*> SmoothPath(const std::vector<Node*>& originalPath);


//확대된 그리드 사이즈 계산
int GetScaledGridSize();
// 화면 좌표를 그리드 좌표로 변환
void ScreenToGrid(int screenX, int screenY, int& gridX, int& gridY);
// 그리드 좌표를 화면 좌표로 변환
void GridToScreen(int gridX, int gridY, int& screenX, int& screenY);
COLORREF GetRandomColor();