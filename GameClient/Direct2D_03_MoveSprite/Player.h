#pragma once

class Player
{
public:
	Player();
	~Player();

	enum class State { Idle, Move };
	void Move(float dx);
	float PlayerX() { return m_x; }
	float PlayerY() { return m_y; }
	void SetState(State state);
	State GetState() { return m_state; }
private:
	float m_x;
	float m_y;
	State m_state;
};

extern Player* g_player;