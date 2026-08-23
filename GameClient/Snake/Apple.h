#pragma once
#include "Config.h"

class Snake;


class Apple
{
public:
	Apple();
	~Apple();
	int AppleX() const { return m_appleX; }
	int AppleY() const { return m_appleY; }
	void MakeCoord(Snake& snake);
	void Render();
private:
	int m_appleX;
	int m_appleY;
};

