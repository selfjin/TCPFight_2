#pragma once
#include <list>
#include "PacketHeader.h"
#include "RingBuffer.h"
#include "SerializationBuffer.h"

//-------------------------------------------------------------
//  NETWORK
//-------------------------------------------------------------

struct Session;
extern SOCKET Listen_Socket;
extern std::list<Session*> playerList;

#define SERVER_PORT 5000
#define MESSAGE_BUFFER_SIZE 5000

///////////////////////////////////////////////////////////////


struct Session
{
	//-------------------------------------------------------------
	// Session 생성자 및 소멸자
	//-------------------------------------------------------------
	Session(int bufferSize)
	{
		recvBuffer = new RingBuffer(bufferSize);
		sendBuffer = new RingBuffer(bufferSize);
	}

	~Session()
	{
		delete recvBuffer;
		delete sendBuffer;
	}

	//-------------------------------------------------------------
	// Network
	//-------------------------------------------------------------
	SOCKET Sock;
	WCHAR Ip[16];
	int Port;

	RingBuffer* recvBuffer;
	RingBuffer* sendBuffer;

	//-------------------------------------------------------------
	// Game
	//-------------------------------------------------------------

	bool Alive;
	int ID;
	char hp;
	short x;
	short y;
	char dir;
	char move = 0;
	char move_Sub = 0;

};


void SOCK_ERROR_PRINT(const char* string);

bool WSASet();

///////////////////////////////////////////////////////////////

void NETWORK_PROC_ACCEPT(SOCKET listenSocket);

void NETWORK_BROADCAST(char* packet, Session* exclude_Session, PACKET_HEADER* header);

void NETWORK_UNICAST(char* packet, Session* user, PACKET_HEADER* header);


///////////////////////////////////////////////////////////////
void netIOProcess_RECV();

void netIOProcess_SEND();

void netSession();

///////////////////////////////////////////////////////////////

//-------------------------------------------------------------
//  NET_PACKET_MP_PROC  // Make Packet
//-------------------------------------------------------------

void NET_PACKET_MP_HEADER(PACKET_HEADER* packet, int defineNum);

void NET_PACKET_MP_MoveStart(CPacket* MakePacket, PACKET_CS_MOVE_START* payloadPacket, int ID);

void NET_PACKET_MP_MoveStop(CPacket* MakePacket, PACKET_CS_MOVE_STOP* payloadPacket, int ID);

void NET_PACKET_MP_Delete(CPacket* MakePacket, int ID);

void NET_PACKET_MP_ATTACK1(CPacket* MakePacket, PACKET_CS_ATTACK1* payloadPacket, int ID);

void NET_PACKET_MP_ATTACK2(CPacket* MakePacket, PACKET_CS_ATTACK2* payloadPacket, int ID);

void NET_PACKET_MP_ATTACK3(CPacket* MakePacket, PACKET_CS_ATTACK3* payloadPacket, int ID);

void NET_PACKET_MP_Damage(CPacket* MakePacket, int attackerID, int damageID, char dagameHP);

//-------------------------------------------------------------
//  NET_PACKET_PROC  // Packet Procedure
//-------------------------------------------------------------
void NET_PACKET_PROC(Session* session, char* packet, int packetType);