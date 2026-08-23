#include "JPS.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <windows.h>
#include <string>
#include <cstring>

PathFinder::PathFinder(char* pGrid, int gridWidth, int gridHeight, int* pSearchIDMarker)
    : m_pGrid(pGrid), m_GridWidth(gridWidth), m_GridHeight(gridHeight),
    m_pEndNode(nullptr), m_pSearchIDMarker(pSearchIDMarker), m_colorIDCounter(1)
{
    m_pClosedMarker = new bool[m_GridWidth * m_GridHeight];
    int gridSize = m_GridWidth * m_GridHeight;
    m_pNodeGrid = new Node * [gridSize];
    memset(m_pNodeGrid, 0, sizeof(Node*) * gridSize);
}


PathFinder::~PathFinder()
{
    delete[] m_pClosedMarker;
    for (Node* node : m_AllNodes)
    {
        delete node;
    }
    m_AllNodes.clear();
    delete[] m_pNodeGrid;

}

Node* PathFinder::createNode(int x, int y)
{
    int index = y * m_GridWidth + x;

    // 해당 좌표에 노드가 이미 있는지 확인
    if (m_pNodeGrid[index])
    {
        return m_pNodeGrid[index];
    }
    Node* newNode = new Node(x, y);
    newNode->Parent = nullptr;
    newNode->G = 1e9f;   // 큰 값 (INF)
    newNode->H = 0.0f;
    newNode->F = 1e9f;
    m_AllNodes.push_back(newNode);
    return newNode;
}

void PathFinder::Initialize(const Node* start, const Node* end)
{
    m_OpenList = {};
    memset(m_pClosedMarker, false, sizeof(bool) * m_GridHeight * m_GridWidth);

    m_pEndNode = end;
    Node* startNode = createNode(start->x, start->y);
    startNode->G = 0.0f;
    startNode->H = (float)Manhatten(start->x, start->y, m_pEndNode->x, m_pEndNode->y);
    startNode->F = startNode->G + startNode->H;
    startNode->Parent = nullptr;

    m_OpenList.push(startNode);
}

int PathFinder::Manhatten(int x1, int y1, int x2, int y2) const
{
    return (std::abs(x1 - x2) + std::abs(y1 - y2));
}

bool PathFinder::isWalkable(int x, int y) const
{
    if (y < 0 || y >= m_GridHeight || x < 0 || x >= m_GridWidth)
        return false;

    return m_pGrid[y * m_GridWidth + x] == 0;
}


bool PathFinder::ExecuteNextStep(std::vector<Node*>* outPath)
{
    if (m_OpenList.empty())
    {
        outPath->clear();
        return true;
    }

    Node* currentNode = m_OpenList.top();
    m_OpenList.pop();

    int markerIndex = currentNode->y * m_GridWidth + currentNode->x;
    if (m_pClosedMarker[markerIndex])
    {
        return false;
    }

    m_pClosedMarker[markerIndex] = true;

    if (currentNode->x == m_pEndNode->x && currentNode->y == m_pEndNode->y)
    {
        std::vector<Node*> path;
        Node* temp = currentNode;
        while (temp != nullptr)
        {
            path.push_back(temp);
            temp = temp->Parent;
        }

        std::reverse(path.begin(), path.end());
        *outPath = path;
        return true;
    }
    FindSuccessors(currentNode);
    return false;
}

std::vector<Node*> PathFinder::JPS_PathFind(const Node* start, const Node* end)  //탐색한 노드 반환용
{
    Initialize(start, end);
    std::vector<Node*> path;
    while (!m_OpenList.empty())
    {
        if (ExecuteNextStep(&path))
        {
            return path;
        }
    }
    return {};
}

void PathFinder::AddToOpenList(Node* successorNode, Node* parentNode)
{
    if (successorNode == nullptr)
        return;

    int markerIndex = successorNode->y * m_GridWidth + successorNode->x;
    if (m_pClosedMarker[markerIndex])
        return;

    int dx = std::abs(successorNode->x - parentNode->x);
    int dy = std::abs(successorNode->y - parentNode->y);
    int diagonal_moves = (std::min)(dx, dy);
    int straight_moves = (std::max)(dx, dy) - diagonal_moves;
    float moveCost = (diagonal_moves * 1.414f) + straight_moves;
    float newG = parentNode->G + moveCost;

    if (newG >= successorNode->G - 1e-6f)
    {
        return;
    }

    successorNode->G = parentNode->G + moveCost;
    successorNode->H = (float)Manhatten(successorNode->x, successorNode->y, m_pEndNode->x, m_pEndNode->y);
    successorNode->F = successorNode->G + successorNode->H;
    successorNode->Parent = parentNode;

    m_OpenList.push(successorNode);
}

Node* PathFinder::JumpStraight(int x, int y, int dirX, int dirY, int colorID)
{
    int nextX = x + dirX;
    int nextY = y + dirY;

    if (!isWalkable(nextX, nextY))
        return nullptr;

    if (m_pSearchIDMarker)
        m_pSearchIDMarker[nextY * m_GridWidth + nextX] = colorID;

    if (m_pEndNode != nullptr && nextX == m_pEndNode->x && nextY == m_pEndNode->y)
    {
        return createNode(nextX, nextY);
    }

    if (dirX != 0) // 수평 이동
    {
        if ((!isWalkable(nextX, nextY + 1) && isWalkable(nextX + dirX, nextY + 1)) ||
            (!isWalkable(nextX, nextY - 1) && isWalkable(nextX + dirX, nextY - 1)))
        {
            return createNode(nextX, nextY);
        }
    }
    else //수직 이동
    {
        if ((!isWalkable(nextX + 1, nextY) && isWalkable(nextX + 1, nextY + dirY)) ||
            (!isWalkable(nextX - 1, nextY) && isWalkable(nextX - 1, nextY + dirY)))
        {
            return createNode(nextX, nextY);
        }
    }

    return JumpStraight(nextX, nextY, dirX, dirY, colorID);

}

Node* PathFinder::JumpDiagonal(int x, int y, int dirX, int dirY, int colorID)
{

    int nextX = x + dirX;
    int nextY = y + dirY;

    if (!isWalkable(nextX, nextY))
        return nullptr;


    if (!isWalkable(nextX - dirX, nextY) && !isWalkable(nextX, nextY - dirY))
        return nullptr;


    if (m_pSearchIDMarker)
        m_pSearchIDMarker[nextY * m_GridWidth + nextX] = colorID;

    if (m_pEndNode != nullptr && nextX == m_pEndNode->x && nextY == m_pEndNode->y)
    {
        return createNode(nextX, nextY);
    }

    if (!isWalkable(nextX - dirX, nextY) && isWalkable(nextX - dirX, nextY + dirY))
    {
        return createNode(nextX, nextY);
    }

    // 수직 방향이 막혀있고, 그 방향 대각선으로 갈 수 있는 경우
    if (!isWalkable(nextX, nextY - dirY) && isWalkable(nextX + dirX, nextY - dirY))
    {
        return createNode(nextX, nextY);
    }

    // 대각선 이동 중 수평/수직 방향에서 점프 포인트가 발견되면 현재 위치가 점프 포인트
    if (JumpStraight(nextX, nextY, dirX, 0, colorID) != nullptr ||
        JumpStraight(nextX, nextY, 0, dirY, colorID) != nullptr)
    {
        return createNode(nextX, nextY);
    }

    return JumpDiagonal(nextX, nextY, dirX, dirY, colorID);

}

std::vector<Node*>  PathFinder::FindSuccessors(Node* pNode) 
{
    m_colorIDCounter++;
    const int searchColorID = m_colorIDCounter;

    std::vector<Node*> successors;
    int x = pNode->x;
    int y = pNode->y;

    if (pNode->Parent == nullptr)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            for (int dy = -1; dy <= 1; ++dy)
            {
                if (dx == 0 && dy == 0) continue;

                Node* jump = nullptr;
                if (dx != 0 && dy != 0)
                {
                    // 대각선 방향
                    jump = JumpDiagonal(x, y, dx, dy, m_colorIDCounter);
                }
                else
                {
                    // 직선 방향
                    jump = JumpStraight(x, y, dx, dy, m_colorIDCounter);
                }

                if (jump)
                {
                    successors.push_back(jump);
                    AddToOpenList(jump, pNode);
                }
            }
        }
    }
    else
    {
        // 부모로부터 온 방향 계산 (정규화)
        int dx = (x > pNode->Parent->x) ? 1 : (x < pNode->Parent->x) ? -1 : 0;
        int dy = (y > pNode->Parent->y) ? 1 : (y < pNode->Parent->y) ? -1 : 0;

        if (dx != 0 && dy != 0)
        {

            // 대각선 이동인 경우
            Node* jp = JumpDiagonal(x, y, dx, dy, m_colorIDCounter);
            if (jp)
            {
                successors.push_back(jp);
                AddToOpenList(jp, pNode);
            }

            jp = JumpStraight(x, y, dx, 0, m_colorIDCounter);
            if (jp)
            {
                successors.push_back(jp);
                AddToOpenList(jp, pNode);
            }

            jp = JumpStraight(x, y, 0, dy, m_colorIDCounter);
            if (jp)
            {
                successors.push_back(jp);
                AddToOpenList(jp, pNode);
            }

            // Forced neighbors 체크
            // 수평 방향이 막혀있고, 대각선으로 갈 수 있는 경우
            if (!isWalkable(x - dx, y) && isWalkable(x - dx, y + dy))
            {
                jp = JumpDiagonal(x, y, -dx, dy, m_colorIDCounter);
                if (jp)
                {
                    successors.push_back(jp);
                    AddToOpenList(jp, pNode);
                }
            }
            // 수직 방향이 막혀있고, 대각선으로 갈 수 있는 경우
            if (!isWalkable(x, y - dy) && isWalkable(x + dx, y - dy))
            {
                jp = JumpDiagonal(x, y, dx, -dy, m_colorIDCounter);
                if (jp)
                {
                    successors.push_back(jp);
                    AddToOpenList(jp, pNode);
                }
            }
        }
        else if (dx != 0)
        {
            // 수평 이동인 경우
            Node* jp = JumpStraight(x, y, dx, 0, m_colorIDCounter);
            if (jp)
            {
                successors.push_back(jp);
                AddToOpenList(jp, pNode);
            }
            // 2. Forced neighbors: 위아래가 막혀있을 때
            // 위쪽이 막혀있고 대각선으로 갈 수 있는 경우
            if (!isWalkable(x, y - 1) && isWalkable(x + dx, y - 1))
            {
                jp = JumpDiagonal(x, y, dx, -1, m_colorIDCounter);
                if (jp)
                {
                    successors.push_back(jp);
                    AddToOpenList(jp, pNode);
                }
            }

            // 아래쪽이 막혀있고 대각선으로 갈 수 있는 경우
            if (!isWalkable(x, y + 1) && isWalkable(x + dx, y + 1))
            {
                jp = JumpDiagonal(x, y, dx, 1, m_colorIDCounter);
                if (jp)
                {
                    successors.push_back(jp);
                    AddToOpenList(jp, pNode);
                }
            }
        }
        else if (dy != 0)
        {
            // 수직 이동인 경우
            Node* jp = JumpStraight(x, y, 0, dy, m_colorIDCounter);
            if (jp)
            {
                successors.push_back(jp);
                AddToOpenList(jp, pNode);
            }
            // 2. Forced neighbors: 좌우가 막혀있을 때
            // 왼쪽이 막혀있고 대각선으로 갈 수 있는 경우
            if (!isWalkable(x - 1, y) && isWalkable(x - 1, y + dy))
            {
                jp = JumpDiagonal(x, y, -1, dy, m_colorIDCounter);
                if (jp)
                {
                    successors.push_back(jp);
                    AddToOpenList(jp, pNode);
                }
            }

            // 오른쪽이 막혀있고 대각선으로 갈 수 있는 경우
            if (!isWalkable(x + 1, y) && isWalkable(x + 1, y + dy))
            {
                jp = JumpDiagonal(x, y, 1, dy, m_colorIDCounter);
                if (jp)
                {
                    successors.push_back(jp);
                    AddToOpenList(jp, pNode);
                }
            }
        }
    }
    return successors;
}