#include <iostream>
#include "Game.h"
#include "Console.h"

using namespace std;


int main()
{
	SetConsoleSize(WIDTH, HEIGHT);
	SetCursor(FALSE);
	srand(time(nullptr));

	//game에서 벽을 그리고 뱀을 놓고 사과를 뿌리기 위해 초기화
	Game game;

	while (true)
	{
		Clear();
		game.KeyCheck();
		game.Update();
		if (game.IsOver() == true)
			break;
		game.Render();
		Sleep(200);
	}

	return 0;
}
