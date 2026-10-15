#include "Server.h"
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include "Session.h"
#include "GameDefine.h"
#include "PacketStruct.h"
#include "_PacketDefine.h"
#include "MakePacket.h"

#pragma comment(lib,"Winmm.lib")

Server::Server()
	: m_listenSocket(INVALID_SOCKET), m_sessionIDCounter(0), m_shutdown(false)
{
}

Server::~Server()
{
	CleanUp();
}

bool Server::CreateSocket()
{
	m_listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_listenSocket == INVALID_SOCKET)
	{
		printf("socket() 오류: %d\n", WSAGetLastError());
		return false;
	}

	// 리슨 소켓 논블로킹 설정
	u_long on = 1;
	if (ioctlsocket(m_listenSocket, FIONBIO, &on) == SOCKET_ERROR)
	{
		printf("리슨 소켓 논블로킹 설정 실패: %d\n", WSAGetLastError());
		return false;
	}

	return true;
}

void Server::SetAddress(unsigned short port)
{
	memset(&m_serverAddr, 0, sizeof(m_serverAddr));
	m_serverAddr.sin_family = AF_INET;
	m_serverAddr.sin_port = htons(port);
	m_serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
}

bool Server::Bind()
{
	if (bind(m_listenSocket, (SOCKADDR*)&m_serverAddr, sizeof(m_serverAddr)) == SOCKET_ERROR)
	{
		printf("바인딩 실패: %d\n", WSAGetLastError());
		return false;
	}

	printf("Bind OK # Port:%d\n", ntohs(m_serverAddr.sin_port));
	return true;
}

bool Server::Listen()
{
	if (listen(m_listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("listen() 오류: %d\n", WSAGetLastError());
		return false;
	}

	printf("Listen OK #\n");
	return true;
}

//네트워크는 매 루프, 게임 로직은 50fps 로 돌린다
void Server::Run()
{
	DWORD dwOldTick = timeGetTime();
	DWORD dwFrameTime = 1000 / GAME_FRAME_PER_SEC;

	while (m_shutdown == false)
	{
		Network();

		DWORD dwCurrentTick = timeGetTime();
		if (dwCurrentTick - dwOldTick >= dwFrameTime)
		{
			Update();
			dwOldTick = dwCurrentTick;
		}
	}
}

void Server::CleanUp()
{
	for (Session* session : m_sessions)
	{
		delete session;
	}
	m_sessions.clear();

	if (m_listenSocket != INVALID_SOCKET)
	{
		closesocket(m_listenSocket);
		m_listenSocket = INVALID_SOCKET;
	}
}

void Server::BroadCast(Session* from, char* packet, int size, bool sendToSender)
{
	for (Session* target : m_sessions)
	{
		if (target->IsAlive() == false)
			continue;

		if (sendToSender == false && target == from)
			continue;

		target->SendPacket(packet, size);
	}
}

void Server::Network()
{
	// 지연 삭제 : 순회가 끝난 지금 끊긴 세션을 한꺼번에 지운다
	for (auto it = m_sessions.begin(); it != m_sessions.end(); )
	{
		if ((*it)->IsAlive() == false)
		{
			delete (*it);
			it = m_sessions.erase(it);
		}
		else
		{
			++it;
		}
	}

	FD_SET readSet;
	FD_SET writeSet;
	FD_ZERO(&readSet);
	FD_ZERO(&writeSet);
	FD_SET(m_listenSocket, &readSet);

	for (Session* session : m_sessions)
	{
		FD_SET(session->GetSocket(), &readSet);
		if (session->HasSendData())
			FD_SET(session->GetSocket(), &writeSet);
	}

	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	int eventCount = select(0, &readSet, &writeSet, NULL, &timeout);

	if (eventCount == SOCKET_ERROR)
	{
		printf("select() 오류: %d\n", WSAGetLastError());
		m_shutdown = true;
		return;
	}

	if (eventCount == 0)
		return;

	if (FD_ISSET(m_listenSocket, &readSet))
	{
		Accept();
		--eventCount;
	}

	for (Session* session : m_sessions)
	{
		if (eventCount <= 0)
			break;

		bool isRead = FD_ISSET(session->GetSocket(), &readSet);
		bool isWrite = FD_ISSET(session->GetSocket(), &writeSet);

		if (isRead)
		{
			--eventCount;
			if (session->Recv() == false)
				session->Disconnect();
		}

		if (isWrite)
		{
			--eventCount;
			if (session->IsAlive() && session->Send() == false)
				session->Disconnect();
		}
	}
}

void Server::Accept()
{
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(clientAddr);
	SOCKET clientSocket = accept(m_listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
	if (clientSocket == INVALID_SOCKET)
	{
		printf("accept 실패: %d\n", WSAGetLastError());
		return;
	}

	char clientIP[INET_ADDRSTRLEN];
	InetNtopA(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);

	// 이동 영역 안의 랜덤 위치에 캐릭터 생성
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<short> distribX(dfRANGE_MOVE_LEFT, dfRANGE_MOVE_RIGHT);
	std::uniform_int_distribution<short> distribY(dfRANGE_MOVE_TOP, dfRANGE_MOVE_BOTTOM);

	Session* newSession = new Session(clientSocket, ++m_sessionIDCounter, distribX(gen), distribY(gen), this);

	printf("Connect # IP:%s / SessionID:%d\n", clientIP, newSession->GetID());
	printf("Create Character # SessionID:%d X:%d Y:%d\n", newSession->GetID(), newSession->GetX(), newSession->GetY());

	// 새 클라이언트에게 자기 캐릭터 정보
	char myCharBuffer[sizeof(SC_CreateMyCharacter)];
	int myCharPacketSize = MakePacket_SC_CreateMyCharacter(myCharBuffer, newSession->GetID(),
		newSession->GetDirection(), newSession->GetX(), newSession->GetY(), newSession->GetHP());
	newSession->SendPacket(myCharBuffer, myCharPacketSize);

	// 새 클라이언트에게 이미 접속해 있던 캐릭터들 정보
	char otherCharBuffer[sizeof(SC_OtherCharacter)];
	for (Session* session : m_sessions)
	{
		int otherCharPacketSize = MakePacket_SC_OtherCharacter(otherCharBuffer, session->GetID(),
			session->GetDirection(), session->GetX(), session->GetY(), session->GetHP());
		newSession->SendPacket(otherCharBuffer, otherCharPacketSize);
	}

	// 기존 클라이언트들에게 새 캐릭터 정보
	char newCharBuffer[sizeof(SC_OtherCharacter)];
	int newCharPacketSize = MakePacket_SC_OtherCharacter(newCharBuffer, newSession->GetID(),
		newSession->GetDirection(), newSession->GetX(), newSession->GetY(), newSession->GetHP());
	BroadCast(newSession, newCharBuffer, newCharPacketSize, false);

	m_sessions.push_back(newSession);
}

void Server::Update()
{
	for (Session* session : m_sessions)
	{
		if (session->IsAlive() == false)
			continue;

		if (session->GetHP() <= 0)
		{
			session->Disconnect();
			continue;
		}

		DWORD action = session->GetAction();
		if (action == dfPACKET_CS_ATTACK1 || action == dfPACKET_CS_ATTACK2 || action == dfPACKET_CS_ATTACK3)
			Attack(session);
		else
			session->Move();
	}
}

//바라보는 방향의 공격 범위 안에서 처음 찾은 한 명에게 데미지
void Server::Attack(Session* attacker)
{
	int rangeX = 0;
	int rangeY = 0;

	switch (attacker->GetAction())
	{
	case dfPACKET_CS_ATTACK1: rangeX = dfATTACK1_RANGE_X; rangeY = dfATTACK1_RANGE_Y; break;
	case dfPACKET_CS_ATTACK2: rangeX = dfATTACK2_RANGE_X; rangeY = dfATTACK2_RANGE_Y; break;
	case dfPACKET_CS_ATTACK3: rangeX = dfATTACK3_RANGE_X; rangeY = dfATTACK3_RANGE_Y; break;
	}

	for (Session* target : m_sessions)
	{
		if (target->IsAlive() == false || target == attacker || target->GetHP() <= 0)
			continue;

		if (abs(attacker->GetY() - target->GetY()) > rangeY)
			continue;

		bool isHit = false;
		if (attacker->GetDirection() == dfPACKET_MOVE_DIR_RR)
		{
			if (target->GetX() > attacker->GetX() && target->GetX() <= attacker->GetX() + rangeX)
				isHit = true;
		}
		else
		{
			if (target->GetX() < attacker->GetX() && target->GetX() >= attacker->GetX() - rangeX)
				isHit = true;
		}

		if (isHit)
		{
			target->Damage(2);

			printf("# AttackHit # AttackerID:%d -> TargetID:%d / TargetHP:%d\n",
				attacker->GetID(), target->GetID(), target->GetHP());

			char damageBuffer[sizeof(SC_Damage)];
			int packetSize = MakePacket_SC_Damage(damageBuffer, attacker->GetID(), target->GetID(), target->GetHP());
			BroadCast(attacker, damageBuffer, packetSize, true);
			break;
		}
	}

	attacker->SetAction(dfACTION_IDLE);
}
