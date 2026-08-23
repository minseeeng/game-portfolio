#include "Player.h"

Player::Player()
	:m_x(200.0f), m_y(300.0f), m_state(State::Idle)
{
}

Player::~Player()
{
}

void Player::Move(float dx)
{
	m_x += dx;
}

void Player::SetState(State state)
{
	m_state = state;
}
