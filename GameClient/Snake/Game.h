#pragma once
#include "Snake.h"
#include "Apple.h"
#include "Wall.h"
#include <conio.h>
#include "Config.h"



class Game
{
public:
	Game();

	void KeyCheck();
	void Update();
	void Render();
	bool IsOver() { return m_dead; }
private:
	Snake m_snake;
	Apple m_apple;
	Wall m_wall;
	bool m_dead = false;
};