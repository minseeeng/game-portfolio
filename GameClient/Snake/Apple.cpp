#include "Apple.h"
#include "Console.h"
#include <stdio.h>
#include "Snake.h"

Apple::Apple()
{
}

Apple::~Apple()
{
}

void Apple::MakeCoord(Snake& snake)
{
	do
	{
		m_appleX = 1 + rand() % (WIDTH - 2);
		m_appleY = 1 + rand() % (HEIGHT - 2);
	} while (snake.IsBodyHit(m_appleX, m_appleY));
}

void Apple::Render()
{
	GotoXY(m_appleX, m_appleY);
	printf("@");
}
