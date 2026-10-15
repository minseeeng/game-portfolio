#pragma once
#include <WinSock2.h>
#include "RingBuffer.h"

class Server;

//클라이언트 하나와 그 캐릭터를 담당한다
class Session
{
public:
	Session(SOCKET sock, DWORD sessionID, short x, short y, Server* server);
	~Session();

	bool Recv();
	bool Send();
	void SendPacket(char* packet, int size);
	void Disconnect();
	void Move();
	void Damage(char damage);

	SOCKET GetSocket() { return m_socket; }
	DWORD GetID() { return m_sessionID; }
	bool IsAlive() { return m_isAlive; }
	bool HasSendData() { return m_sendQ.GetUseSize() > 0; }

	DWORD GetAction() { return m_action; }
	void SetAction(DWORD action) { m_action = action; }
	BYTE GetDirection() { return m_direction; }
	short GetX() { return m_x; }
	short GetY() { return m_y; }
	char GetHP() { return m_hp; }

private:
	bool ProcessPacket(BYTE type, char* payload);
	bool PacketProc_MoveStart(char* payload);
	bool PacketProc_MoveStop(char* payload);
	bool PacketProc_Attack1(char* payload);
	bool PacketProc_Attack2(char* payload);
	bool PacketProc_Attack3(char* payload);
	void UpdatePosition(short x, short y);

private:
	SOCKET m_socket;
	DWORD m_sessionID;
	Server* m_server;
	RingBuffer m_recvQ;
	RingBuffer m_sendQ;

	bool m_isAlive;
	DWORD m_action;
	BYTE m_direction;
	short m_x;
	short m_y;
	char m_hp;
};
