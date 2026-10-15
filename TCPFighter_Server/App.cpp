#include "App.h"
#include <stdio.h>
#include "GameDefine.h"

#pragma comment(lib,"Winmm.lib")

CApp::CApp()
{
}

CApp::~CApp()
{
}

bool CApp::Init()
{
	timeBeginPeriod(1);

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("WSAStartup() 오류 : %d\n", WSAGetLastError());
		return false;
	}

	printf("WSAStartup #\n");
	return true;
}

bool CApp::Start()
{
	if ((m_server.CreateSocket() == false))
		return false;

	m_server.SetAddress(SERVERPORT);

	if ((m_server.Bind() == false))
		return false;

	if ((m_server.Listen() == false))
		return false;

	return true;
}

void CApp::Run()
{
	m_server.Run();
}

void CApp::End()
{
	m_server.CleanUp();
	WSACleanup();
	timeEndPeriod(1);
}
