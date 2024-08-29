#pragma once
#include <Windows.h>
#include <list>
#include "PacketHeader.h"
#include "RingBuffer.h"

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

void NET_PACKET_MP_MoveStart(PACKET_SC_MOVE_START* MakePacket, PACKET_CS_MOVE_START* payloadPacket, int ID);

void NET_PACKET_MP_MoveStop(PACKET_SC_MOVE_STOP* MakePacket, PACKET_CS_MOVE_STOP* payloadPacket, int ID);

void NET_PACKET_MP_Delete(PACKET_SC_DELETE_CHARACTER* MakePacket, int ID);

//-------------------------------------------------------------
//  NET_PACKET_PROC  // Packet Procedure
//-------------------------------------------------------------
void NET_PACKET_PROC(Session* session, char* packet, int packetType);