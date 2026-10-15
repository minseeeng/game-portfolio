#pragma once
#include "Server.h"

class CApp
{
public:
	CApp();
	~CApp();

	bool Init();
	bool Start();
	void Run();
	void End();

private:
	Server m_server;
};
