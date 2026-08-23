#pragma once
#include "List.h"
#include "Config.h"


class Apple;

enum class MoveResult
{
	Moved,
	Ate,
	Died,
};

class Snake 
{
public:
	Snake();
	~Snake();
	MoveResult Move(const Apple& apple);
	void Render();
	bool IsBodyHit(int x, int y);
	int	NextHeadX()	{ return m_list.GetHeadX() + m_dx[m_dir]; }
	int NextHeadY() { return m_list.GetHeadY() + m_dy[m_dir]; }
	void ChangeDir(int dir) { m_dir = dir; }

private:
	DLinkedList m_list;
	int			m_dir;
	int			m_dx[4] = { -1, 0, 1, 0 };
	int			m_dy[4] = { 0, 1, 0, -1 };
};

