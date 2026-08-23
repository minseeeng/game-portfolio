#pragma once

// 벽을 "#" 형태로 생성
// 정해진 범위로 사각형 형태로 만든다
// #이 아니면 벽으로 간주하지 않음
class Wall
{
public:
	Wall();
	~Wall();

	void Render();
};

