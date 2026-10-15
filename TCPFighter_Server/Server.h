#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <list>

#pragma comment(lib,"ws2_32.lib")

class Session;

//접속을 받고 세션들을 select 로 감시하며 게임 프레임을 돌린다
class Server
{
public:
	Server();
	~Server();

	bool CreateSocket();
	void SetAddress(unsigned short port);
	bool Bind();
	bool Listen();
	void Run();
	void CleanUp();
	void BroadCast(Session* from, char* packet, int size, bool sendToSender);

private:
	void Network();
	void Accept();
	void Update();
	void Attack(Session* attacker);

private:
	SOCKET m_listenSocket;
	SOCKADDR_IN m_serverAddr;
	std::list<Session*> m_sessions;
	DWORD m_sessionIDCounter;
	bool m_shutdown;
};
