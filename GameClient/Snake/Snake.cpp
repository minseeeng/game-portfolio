#include "Snake.h"
#include "Apple.h"
#include "Console.h"
#include <stdio.h>

Snake::Snake()
{
	m_list.PushFront(12, 10);
	m_list.PushFront(11, 10);
	m_dir = 0;
}

Snake::~Snake()
{
}

MoveResult Snake::Move(const Apple& apple)
{
	int snakeHeadX = NextHeadX();
	int snakeHeadY = NextHeadY();
	int appleX = apple.AppleX();
	int appleY = apple.AppleY();

	if (snakeHeadX == appleX && snakeHeadY == appleY)
	{
		m_list.PushFront(snakeHeadX, snakeHeadY);
		return MoveResult::Ate;
	}

	m_list.PopBack();

	if ((snakeHeadX <= 0 || snakeHeadX >= WIDTH - 1 ||
		snakeHeadY <= 0 || snakeHeadY >= HEIGHT - 1) || IsBodyHit(snakeHeadX, snakeHeadY))
	{
		return MoveResult::Died;
	}
	
	m_list.PushFront(snakeHeadX, snakeHeadY);
	
	return MoveResult::Moved;
}

bool Snake::IsBodyHit(int x, int y)
{
	return m_list.Contains(x, y);
}

void Snake::Render()
{
	const DLinkedList::Node* cur =  m_list.GetHead();
	while (cur != nullptr)
	{
		if (cur == m_list.GetHead())
		{
			GotoXY(cur->x, cur->y);
			printf("O");
		}
		else
		{
			GotoXY(cur->x, cur->y);
			printf("*");
		}
		cur = cur->next;
	}
}