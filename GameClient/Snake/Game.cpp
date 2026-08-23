#include "Game.h"


Game::Game()
{
	m_apple.MakeCoord(m_snake);
}


void Game::KeyCheck()
{
	if (_kbhit())
	{
		char ch = _getch();
		if (ch == 'w')
			m_snake.ChangeDir(3);
		if (ch == 'a')
			m_snake.ChangeDir(0);
		if (ch == 's')
			m_snake.ChangeDir(1);
		if (ch == 'd')
			m_snake.ChangeDir(2);
	}
}


void Game::Update()
{
	
	MoveResult result = m_snake.Move(m_apple);
	if (result == MoveResult::Died)
		m_dead = true;
	if (result == MoveResult::Ate)
		m_apple.MakeCoord(m_snake);

}



void Game::Render()
{
	m_wall.Render();
	m_apple.Render();
	m_snake.Render();
}


