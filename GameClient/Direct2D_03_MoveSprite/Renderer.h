#pragma once
#include <d2d1.h>
#include "Sprite.h"

class Device;

class Renderer
{
public:
	Renderer();
	~Renderer();

	HRESULT Init(Device* device);
	void Render(Device* device);
	void Update();
	void Move();

private:
	Sprite* m_sprite;
};

extern Renderer* g_renderer;

