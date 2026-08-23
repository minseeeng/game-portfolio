#pragma once
#include <vector>
#include <queue>
#include <cmath>

struct Node
{
    int x, y;
    Node* Parent;
    float G, H, F;
    Node(int _x, int _y)
        : x(_x), y(_y), G(0), H(0), F(0), Parent(nullptr) {}
};

struct CompareNode
{
    bool operator()(Node* a, Node* b)
    {
        return a->F > b->F;
    }
};


class PathFinder
{
public:
    PathFinder(char* pGrid, int gridWidth, int gridHeight, int* pSearchIDMarker);
    ~PathFinder();

    void Initialize(const Node* start, const Node* end);
    bool ExecuteNextStep(std::vector<Node*>* outPath);
    std::vector<Node*> JPS_PathFind(const Node* start, const Node* end);

    std::priority_queue<Node*, std::vector<Node*>, CompareNode>& GetOpenList() { return m_OpenList; }
    const std::vector<Node*>& GetClosedList() const { return m_AllNodes; }
    std::vector<Node*> ReleaseClosedList() { return std::move(m_AllNodes); }

private:
    Node* JumpStraight(int x, int y, int dirX, int dirY, int colorID);
    Node* JumpDiagonal(int x, int y, int dirX, int dirY, int colorID);
    std::vector < Node*> FindSuccessors(Node* currentNode);
    void AddToOpenList(Node* successorNode, Node* parentNode);

    bool isWalkable(int x, int y) const;
    int Manhatten(int x1, int y1, int x2, int y2) const;
    Node* createNode(int x, int y);
    Node** m_pNodeGrid;

private:
    char* m_pGrid;
    int m_GridWidth;
    int m_GridHeight;

    const Node* m_pEndNode; 
    std::priority_queue<Node*, std::vector<Node*>, CompareNode> m_OpenList;
    bool* m_pClosedMarker;
    std::vector<Node*> m_AllNodes;

    int* m_pSearchIDMarker;
    int m_colorIDCounter;
};