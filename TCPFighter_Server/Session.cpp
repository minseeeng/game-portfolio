#include "Session.h"
#include <stdio.h>
#include <stdlib.h>
#include "Server.h"
#include "GameDefine.h"
#include "PacketStruct.h"
#include "_PacketDefine.h"
#include "MakePacket.h"

Session::Session(SOCKET sock, DWORD sessionID, short x, short y, Server* server)
	: m_socket(sock), m_sessionID(sessionID), m_server(server), m_isAlive(true),
	m_action(dfACTION_IDLE), m_direction(dfPACKET_MOVE_DIR_RR), m_x(x), m_y(y), m_hp(100)
{
}

Session::~Session()
{
	closesocket(m_socket);
}

bool Session::Recv()
{
	int directEnqueueSize = m_recvQ.DirectEnqueueSize();
	if (directEnqueueSize <= 0)
	{
		printf("ID: %d, RecvQ is full!\n", m_sessionID);
		return false;
	}

	char* pWritePtr = m_recvQ.GetRearBufferPtr();
	int recvResult = recv(m_socket, pWritePtr, directEnqueueSize, 0);

	// 클라이언트가 정상적으로 접속을 종료함
	if (recvResult == 0)
		return false;

	if (recvResult == SOCKET_ERROR)
	{
		// 논블로킹 소켓에서 아직 받을 데이터가 없다는 의미, 에러가 아님
		if (WSAGetLastError() == WSAEWOULDBLOCK)
			return true;

		printf("ID: %d, recv() 에러: %d\n", m_sessionID, WSAGetLastError());
		return false;
	}

	m_recvQ.MoveRear(recvResult);

	while (true)
	{
		int useSize = m_recvQ.GetUseSize();
		if (useSize < sizeof(Header))
			break;

		Header header;
		m_recvQ.Peek((char*)&header, sizeof(Header));
		if (header.byCode != 0x89)
		{
			printf("ID: %d, 잘못된 패킷 코드 수신: 0x%X\n", m_sessionID, header.byCode);
			return false;
		}

		int requiredSize = sizeof(Header) + header.bySize;
		if (useSize < requiredSize)
			break;

		char packetBuffer[4096];
		m_recvQ.Dequeue(packetBuffer, requiredSize);
		if (ProcessPacket(header.byType, packetBuffer + sizeof(Header)) == false)
			return false;
	}

	return true;
}

bool Session::Send()
{
	while (m_sendQ.GetUseSize() > 0)
	{
		int directDequeueSize = m_sendQ.DirectDequeueSize();
		if (directDequeueSize <= 0)
			break;

		char* pReadPtr = m_sendQ.GetFrontBufferPtr();
		int sendResult = send(m_socket, pReadPtr, directDequeueSize, 0);

		if (sendResult == SOCKET_ERROR)
		{
			// 송신 버퍼가 가득 참, 다음 select 에서 다시 보낸다
			if (WSAGetLastError() == WSAEWOULDBLOCK)
				break;

			printf("ID: %d, send() 에러: %d\n", m_sessionID, WSAGetLastError());
			return false;
		}

		m_sendQ.MoveFront(sendResult);
	}

	return true;
}

void Session::SendPacket(char* packet, int size)
{
	m_sendQ.Enqueue(packet, size);
}

//실제 삭제는 Server::Network() 맨 위에서 일괄로 한다 (지연 삭제)
void Session::Disconnect()
{
	if (m_isAlive == false)
		return;

	printf("Disconnect # SessionID:%d\n", m_sessionID);
	m_isAlive = false;

	char deleteBuffer[sizeof(SC_DeleteCharacter)];
	int packetSize = MakePacket_SC_DeleteCharacter(deleteBuffer, m_sessionID);
	m_server->BroadCast(this, deleteBuffer, packetSize, false);
}

void Session::Move()
{
	static const char* actionNames[] = { "LL", "LU", "UU", "RU", "RR", "RD", "DD", "LD" };

	switch (m_action)
	{
	case dfPACKET_MOVE_DIR_LL: m_x -= MOVE_SPEED_X; break;
	case dfPACKET_MOVE_DIR_LU: m_x -= MOVE_SPEED_X; m_y -= MOVE_SPEED_Y; break;
	case dfPACKET_MOVE_DIR_UU: m_y -= MOVE_SPEED_Y; break;
	case dfPACKET_MOVE_DIR_RU: m_x += MOVE_SPEED_X; m_y -= MOVE_SPEED_Y; break;
	case dfPACKET_MOVE_DIR_RR: m_x += MOVE_SPEED_X; break;
	case dfPACKET_MOVE_DIR_RD: m_x += MOVE_SPEED_X; m_y += MOVE_SPEED_Y; break;
	case dfPACKET_MOVE_DIR_DD: m_y += MOVE_SPEED_Y; break;
	case dfPACKET_MOVE_DIR_LD: m_x -= MOVE_SPEED_X; m_y += MOVE_SPEED_Y; break;
	default:
		return;
	}

	// 화면 경계에 닿으면 멈춘다
	if (m_x < dfRANGE_MOVE_LEFT)
	{
		m_x = dfRANGE_MOVE_LEFT;
		m_action = dfACTION_IDLE;
	}
	if (m_x > dfRANGE_MOVE_RIGHT)
	{
		m_x = dfRANGE_MOVE_RIGHT;
		m_action = dfACTION_IDLE;
	}
	if (m_y < dfRANGE_MOVE_TOP)
	{
		m_y = dfRANGE_MOVE_TOP;
		m_action = dfACTION_IDLE;
	}
	if (m_y > dfRANGE_MOVE_BOTTOM)
	{
		m_y = dfRANGE_MOVE_BOTTOM;
		m_action = dfACTION_IDLE;
	}

	if (m_action != dfACTION_IDLE)
	{
		printf("# gameRun:%s # SessionID:%d / X:%d / Y:%d\n",
			actionNames[m_action], m_sessionID, m_x, m_y);
	}
}

void Session::Damage(char damage)
{
	m_hp -= damage;
	if (m_hp < 0)
		m_hp = 0;
}

bool Session::ProcessPacket(BYTE type, char* payload)
{
	printf("Packet Received # SessionID:%d / PacketType:%d\n", m_sessionID, type);

	switch (type)
	{
	case dfPACKET_CS_MOVE_START:
		return PacketProc_MoveStart(payload);
	case dfPACKET_CS_MOVE_STOP:
		return PacketProc_MoveStop(payload);
	case dfPACKET_CS_ATTACK1:
		return PacketProc_Attack1(payload);
	case dfPACKET_CS_ATTACK2:
		return PacketProc_Attack2(payload);
	case dfPACKET_CS_ATTACK3:
		return PacketProc_Attack3(payload);
	default:
		return false;
	}
}

bool Session::PacketProc_MoveStart(char* payload)
{
	CS_MoveStart* pRecvPacket = (CS_MoveStart*)payload;

	printf("# PACKET_MOVESTART # SessionID:%d / Direction:%d / X:%d / Y:%d\n",
		m_sessionID, pRecvPacket->byDirection, pRecvPacket->x, pRecvPacket->y);

	UpdatePosition(pRecvPacket->x, pRecvPacket->y);

	m_action = pRecvPacket->byDirection;

	// 캐릭터가 바라보는 방향은 좌/우 두 가지
	switch (pRecvPacket->byDirection)
	{
	case dfPACKET_MOVE_DIR_RR:
	case dfPACKET_MOVE_DIR_RU:
	case dfPACKET_MOVE_DIR_RD:
		m_direction = dfPACKET_MOVE_DIR_RR;
		break;
	case dfPACKET_MOVE_DIR_LL:
	case dfPACKET_MOVE_DIR_LU:
	case dfPACKET_MOVE_DIR_LD:
		m_direction = dfPACKET_MOVE_DIR_LL;
		break;
	}

	char sendBuffer[sizeof(SC_MoveStart)];
	int packetSize = MakePacket_SC_MoveStart(sendBuffer, m_sessionID, pRecvPacket->byDirection, m_x, m_y);
	m_server->BroadCast(this, sendBuffer, packetSize, false);

	return true;
}

bool Session::PacketProc_MoveStop(char* payload)
{
	CS_MoveStop* pRecvPacket = (CS_MoveStop*)payload;

	printf("# PACKET_MOVESTOP # SessionID:%d / Direction:%d / X:%d / Y:%d\n",
		m_sessionID, pRecvPacket->byDirection, pRecvPacket->x, pRecvPacket->y);

	UpdatePosition(pRecvPacket->x, pRecvPacket->y);

	m_action = dfACTION_IDLE;
	m_direction = pRecvPacket->byDirection;

	char sendBuffer[sizeof(SC_MoveStop)];
	int packetSize = MakePacket_SC_MoveStop(sendBuffer, m_sessionID, pRecvPacket->byDirection, m_x, m_y);
	m_server->BroadCast(this, sendBuffer, packetSize, false);

	return true;
}

bool Session::PacketProc_Attack1(char* payload)
{
	CS_Attack_1* pRecvPacket = (CS_Attack_1*)payload;

	printf("# PACKET_ATTACK1 # SessionID:%d / Direction:%d / X:%d / Y:%d\n",
		m_sessionID, pRecvPacket->byDirection, pRecvPacket->x, pRecvPacket->y);

	m_action = dfPACKET_CS_ATTACK1;
	m_direction = pRecvPacket->byDirection;
	UpdatePosition(pRecvPacket->x, pRecvPacket->y);

	char sendBuffer[sizeof(SC_Attack_1)];
	int packetSize = MakePacket_SC_Attack_1(sendBuffer, m_sessionID, pRecvPacket->byDirection, m_x, m_y);
	m_server->BroadCast(this, sendBuffer, packetSize, false);

	return true;
}

bool Session::PacketProc_Attack2(char* payload)
{
	CS_Attack_2* pRecvPacket = (CS_Attack_2*)payload;

	printf("# PACKET_ATTACK2 # SessionID:%d / Direction:%d / X:%d / Y:%d\n",
		m_sessionID, pRecvPacket->byDirection, pRecvPacket->x, pRecvPacket->y);

	m_action = dfPACKET_CS_ATTACK2;
	m_direction = pRecvPacket->byDirection;
	UpdatePosition(pRecvPacket->x, pRecvPacket->y);

	char sendBuffer[sizeof(SC_Attack_2)];
	int packetSize = MakePacket_SC_Attack_2(sendBuffer, m_sessionID, pRecvPacket->byDirection, m_x, m_y);
	m_server->BroadCast(this, sendBuffer, packetSize, false);

	return true;
}

bool Session::PacketProc_Attack3(char* payload)
{
	CS_Attack_3* pRecvPacket = (CS_Attack_3*)payload;

	printf("# PACKET_ATTACK3 # SessionID:%d / Direction:%d / X:%d / Y:%d\n",
		m_sessionID, pRecvPacket->byDirection, pRecvPacket->x, pRecvPacket->y);

	m_action = dfPACKET_CS_ATTACK3;
	m_direction = pRecvPacket->byDirection;
	UpdatePosition(pRecvPacket->x, pRecvPacket->y);

	char sendBuffer[sizeof(SC_Attack_3)];
	int packetSize = MakePacket_SC_Attack_3(sendBuffer, m_sessionID, pRecvPacket->byDirection, m_x, m_y);
	m_server->BroadCast(this, sendBuffer, packetSize, false);

	return true;
}

//클라 좌표가 서버 좌표와 오차 범위 안이면 받아들이고, 벗어나면 서버 좌표를 유지한다
void Session::UpdatePosition(short x, short y)
{
	if (abs(m_x - x) > dfERROR_RANGE || abs(m_y - y) > dfERROR_RANGE)
	{
		printf("# PositionError # SessionID:%d / Server X:%d Y:%d / Client X:%d Y:%d\n",
			m_sessionID, m_x, m_y, x, y);
		return;
	}

	m_x = x;
	m_y = y;
}
