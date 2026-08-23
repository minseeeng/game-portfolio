#include "Wall.h"
#include "Config.h"
#include "Console.h"
#include <stdio.h>

Wall::Wall()
{
}

Wall::~Wall()
{
}

void Wall::Render()
{
	for (int x = 0; x < WIDTH; x++)
	{
		GotoXY(x, 0);
		printf("#");
		GotoXY(x, HEIGHT - 1);
		printf("#");
	}
	for (int y = 0; y < HEIGHT; y++)
	{
		GotoXY(0, y);
		printf("#");
		GotoXY(WIDTH - 1, y);
		printf("#");
	}
}