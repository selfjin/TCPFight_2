#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "Network.h"
#include "game.h"
#include "PacketDefine.h"

//-------------------------------------------------------------
//  NETWORK
//-------------------------------------------------------------

SOCKET Listen_Socket;
SOCKADDR_IN serverAddr;

std::list<Session*> playerList;

int ID_CREATE_VALUE;

///////////////////////////////////////////////////////////////


void SOCK_ERROR_PRINT(const char* string)
{
    printf("Faild : %s,  ", string);
    int value = WSAGetLastError();
    printf("Error Code : %d\n", value);
}

bool WSASet()
{
    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        SOCK_ERROR_PRINT("WSAStartUp()");
        return 0;
    }

    // 1. SOCKET

    Listen_Socket = socket(AF_INET, SOCK_STREAM, 0);
    if (Listen_Socket == INVALID_SOCKET)
    {
        SOCK_ERROR_PRINT("socket()");
        return 0;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    InetPton(AF_INET, L"0.0.0.0", &serverAddr.sin_addr);
    serverAddr.sin_port = htons(SERVER_PORT);


    // 2. bind

    int bind_Check = bind(Listen_Socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

    if (bind_Check == SOCKET_ERROR)
    {
        SOCK_ERROR_PRINT("bind");
        return 0;
    }


    // 3. Listen


    int listen_Check = listen(Listen_Socket, SOMAXCONN);

    if (listen_Check == SOCKET_ERROR)
    {
        SOCK_ERROR_PRINT("listen");
        return 0;
    }

    // 4. Socket Option


    // 4 - 1. Linger 

    LINGER linger_ON;
    {
        linger_ON.l_linger = 1;
        linger_ON.l_onoff = 0;
    }

    int linger_Check = setsockopt(Listen_Socket, SOL_SOCKET, SO_LINGER, (char*)&linger_ON, sizeof(linger_ON));

    if (linger_Check == SOCKET_ERROR)
    {
        SOCK_ERROR_PRINT("linger");
        return 0;
    }


    // 4- 2. NonBlocking 

    u_long nonBlocking_ON = 1;

    int ioctl_Check = ioctlsocket(Listen_Socket, FIONBIO, &nonBlocking_ON);

    if (ioctl_Check == SOCKET_ERROR)
    {
        SOCK_ERROR_PRINT("ioctl");
        return 0;
    }

    return 1;
}

//-------------------------------------------------------------
//  NETWORK IO
//-------------------------------------------------------------

void NETWORK_PROC_ACCEPT(SOCKET listenSocket)
{
    SOCKET Client_Socket;
    SOCKADDR_IN clientAddr;
    int addrLen = sizeof(clientAddr);
    memset(&clientAddr, 0, sizeof(clientAddr));

    Client_Socket = accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);

    if (Client_Socket == INVALID_SOCKET)
    {
        printf("\n accept - INVALID_SOCKET \n");
        int errorValue = WSAGetLastError();
        printf("ERROR CODE : %d \n", errorValue);
        return;
    }

    Session* CreateUser = new Session(RINBBUFFER_SIZE);
    {
        CreateUser->Alive = TRUE;
        CreateUser->ID = ID_CREATE_VALUE++;
        InetNtop(AF_INET, &clientAddr.sin_addr, CreateUser->Ip, 16);
        CreateUser->Port = ntohs(clientAddr.sin_port);
        CreateUser->Sock = Client_Socket;
        CreateUser->x = (SCREEN_WIDTH / 2) + 1;
        CreateUser->y = (SCREEN_HEIGHT / 2) + 1;
        CreateUser->hp = 100;
        CreateUser->dir = dfPACKET_MOVE_DIR_RR;
        CreateUser->move = 0;
    }

    // 1. 유니캐스트 - 자기 자신 생성

    PAKCET_SC_CREATE_MY_CHARACTER Packet_Self;
    {
        Packet_Self.ID = CreateUser->ID;
        Packet_Self.dir = dfPACKET_MOVE_DIR_RR;
        Packet_Self.x = CreateUser->x;
        Packet_Self.y = CreateUser->y;
        Packet_Self.hp = CreateUser->hp;
    }

    PACKET_HEADER header_My;
    NET_PACKET_MP_HEADER(&header_My, dfPACKET_SC_CREATE_MY_CHARACTER);

    NETWORK_UNICAST((char*)&Packet_Self, CreateUser,
        &header_My);

    playerList.push_back(CreateUser);

    // 2. 유니캐스트 - 다른 플레이어 니 화면에 생성해라

    for (auto it : playerList)
    {
        if (it->Alive == true && it->ID != CreateUser->ID) // 본인은 제외
        {
            PACKET_SC_CREATE_OTHER_CHARACTER packet_Other;
            {
                packet_Other.dir = it->dir;
                packet_Other.hp = it->hp;
                packet_Other.ID = it->ID;
                packet_Other.x = it->x;
                packet_Other.y = it->y;
            }

            PACKET_HEADER header_Other;
            NET_PACKET_MP_HEADER(&header_Other, dfPACKET_SC_CREATE_OTHER_CHARACTER);

            NETWORK_UNICAST((char*)&packet_Other, CreateUser,
                &header_Other);
        }
    }


    // 3. 멀티캐스트 - 만들어진 녀석 생성 ( 자기 자신 제외 )

    PACKET_SC_CREATE_OTHER_CHARACTER packet_New;
    {
        packet_New.dir = CreateUser->dir;
        packet_New.hp = CreateUser->hp;
        packet_New.ID = CreateUser->ID;
        packet_New.x = CreateUser->x;
        packet_New.y = CreateUser->y;
    }
    PACKET_HEADER header_New;
    NET_PACKET_MP_HEADER(&header_New, dfPACKET_SC_CREATE_OTHER_CHARACTER);
    
    NETWORK_BROADCAST((char*)&packet_New, CreateUser,
        &header_New);

}

void NETWORK_BROADCAST(char* packet, Session* exclude_Session, PACKET_HEADER* header)
{
    // 브로드캐스트
    if (exclude_Session == nullptr)
    {
        auto it = playerList.begin();

        while (it != playerList.end())
        {
            if ((*it)->Alive != false)
            {
                (*it)->sendBuffer->Enqueue((char*)header, sizeof(PACKET_HEADER));
                (*it)->sendBuffer->Enqueue(packet, header->Size);
            }
            ++it;
        }
    }
    else
    {
        auto it = playerList.begin();

        while (it != playerList.end())
        {
            if ((*it)->Alive != false && (*it)->ID != exclude_Session->ID)
            {
                (*it)->sendBuffer->Enqueue((char*)header, sizeof(PACKET_HEADER));
                (*it)->sendBuffer->Enqueue(packet, header->Size);
            }
            ++it;
        }
    }
}

void NETWORK_UNICAST(char* packet, Session* user, PACKET_HEADER* header)
{

    if (user->Alive == false)
    {
        return;
    }

    user->sendBuffer->Enqueue((char*)header, sizeof(PACKET_HEADER));
    user->sendBuffer->Enqueue(packet, header->Size);


}


//-------------------------------------------------------------
//  NETWORK Logic
//-------------------------------------------------------------

void netIOProcess_RECV()
{
    FD_SET reads;
    FD_ZERO(&reads);

    timeval select_Non_Blocking;
    select_Non_Blocking.tv_sec = 0;
    select_Non_Blocking.tv_usec = 0;

    //-------------------------------------------------------------
    // SET
    //-------------------------------------------------------------

    // 1. Listen Socket

    FD_SET(Listen_Socket, &reads);

    // 2. Player Socket
    for (auto it : playerList)
    {
        FD_SET((*it).Sock, &reads);
    }
    

    // 3. Select

    int recvSelect = select(0, &reads, NULL, NULL, &select_Non_Blocking);

    if (recvSelect == SOCKET_ERROR)
    {
        SOCK_ERROR_PRINT("select - recv");
        DebugBreak;
    }

    //-------------------------------------------------------------
    // Pooling
    //-------------------------------------------------------------

    if (FD_ISSET(Listen_Socket, &reads))
    {
        NETWORK_PROC_ACCEPT(Listen_Socket);
    }
    
    
    for (auto recvPlayerIter : playerList)
    {
        if (FD_ISSET((*recvPlayerIter).Sock, &reads))
        {
            if ((*recvPlayerIter).Alive == false)
            {
                continue;
            }
            char Message[MESSAGE_BUFFER_SIZE] = { 0, };

            int recvByte = recv((*recvPlayerIter).Sock, Message, MESSAGE_BUFFER_SIZE, 0);

            if (recvByte == SOCKET_ERROR)
            {
                if (WSAGetLastError() == WSAEWOULDBLOCK)
                {
                    continue;
                }
                else
                {
                    (*recvPlayerIter).Alive = false;
                    continue;
                }
            }

            if (recvByte == 0)
            {
                (*recvPlayerIter).Alive = false;
                continue;
            }
            
            (*recvPlayerIter).recvBuffer->Enqueue(Message, recvByte);


            while (1)
            {
                PACKET_HEADER header;
                int headerPeekSize = (*recvPlayerIter).recvBuffer->peek((char*)&header, sizeof(header));

                if (headerPeekSize < sizeof(header))
                {
                    break;
                }

                if (header.Code != 0x89)
                {
                    (*recvPlayerIter).Alive = false;
                    break;
                }
                

                /////////////////////////////////////////////////////////////////////////////////////////////////

                switch (header.Type)
                {
                case dfPACKET_CS_MOVE_START:
                {
                    PACKET_CS_MOVE_START recvPacket;
                    (*recvPlayerIter).recvBuffer->moveBegin(sizeof(header));
                    (*recvPlayerIter).recvBuffer->Dequeue((char*)&recvPacket, sizeof(PACKET_CS_MOVE_START));
                    NET_PACKET_PROC(recvPlayerIter, (char*)&recvPacket, dfPACKET_CS_MOVE_START);
                    break;
                }
                case dfPACKET_CS_MOVE_STOP:
                {
                    PACKET_CS_MOVE_STOP recvPacket;
                    (*recvPlayerIter).recvBuffer->moveBegin(sizeof(header));
                    (*recvPlayerIter).recvBuffer->Dequeue((char*)&recvPacket, sizeof(PACKET_CS_MOVE_STOP));
                    NET_PACKET_PROC(recvPlayerIter, (char*)&recvPacket, dfPACKET_CS_MOVE_STOP);
                    break;
                }
                case dfPACKET_CS_ATTACK1:
                {
                    PACKET_CS_ATTACK1 recvPacket;
                    (*recvPlayerIter).recvBuffer->moveBegin(sizeof(header));
                    (*recvPlayerIter).recvBuffer->Dequeue((char*)&recvPacket, sizeof(PACKET_CS_ATTACK1));
                    NET_PACKET_PROC(recvPlayerIter, (char*)&recvPacket, dfPACKET_CS_ATTACK1);
                    break;
                }
                case dfPACKET_CS_ATTACK2:
                {
                    PACKET_CS_ATTACK2 recvPacket;
                    (*recvPlayerIter).recvBuffer->moveBegin(sizeof(header));
                    (*recvPlayerIter).recvBuffer->Dequeue((char*)&recvPacket, sizeof(PACKET_CS_ATTACK2));
                    NET_PACKET_PROC(recvPlayerIter, (char*)&recvPacket, dfPACKET_CS_ATTACK2);
                    break;
                }
                case dfPACKET_CS_ATTACK3:
                {
                    PACKET_CS_ATTACK3 recvPacket;
                    (*recvPlayerIter).recvBuffer->moveBegin(sizeof(header));
                    (*recvPlayerIter).recvBuffer->Dequeue((char*)&recvPacket, sizeof(PACKET_CS_ATTACK3));
                    NET_PACKET_PROC(recvPlayerIter, (char*)&recvPacket, dfPACKET_CS_ATTACK3);
                    break;
                }

                default:
                    DebugBreak;
                    break;
                }
                
                /////////////////////////////////////


                if ((*recvPlayerIter).recvBuffer->getSize() < sizeof(header) + header.Size)
                {
                    break;
                }
            }
            

        }

    }
    
}

void netIOProcess_SEND()
{
    fd_set writes;
    FD_ZERO(&writes);

    timeval select_Non_Blocking;
    select_Non_Blocking.tv_sec = 0;
    select_Non_Blocking.tv_usec = 0;

    //-------------------------------------------------------------
    // SET
    //-------------------------------------------------------------

    // 1. Player Socket
    for (auto it : playerList)
    {
        if ((*it).sendBuffer->getSize())
        {
            FD_SET((*it).Sock, &writes);
        }
    }


    // 2. Select

    if (writes.fd_count != 0)
    {
        int recvSelect = select(0, NULL, &writes, NULL, &select_Non_Blocking);

        if (recvSelect == SOCKET_ERROR)
        {
            SOCK_ERROR_PRINT("select - recv");
            DebugBreak;
        }
    }

    //-------------------------------------------------------------
    // Pooling
    //-------------------------------------------------------------

    for (auto it : playerList)
    {
        if (FD_ISSET((*it).Sock, &writes))
        {
            if ((*it).Alive == false)
            {
                continue;
            }
            char message[MESSAGE_BUFFER_SIZE] = { 0, };

            int peekSize = (*it).sendBuffer->peek(message, (*it).sendBuffer->getSize());

            int sendByteMessage = send((*it).Sock, message, peekSize, 0);

            if (sendByteMessage == SOCKET_ERROR)
            {
                if (WSAGetLastError() == WSAEWOULDBLOCK)
                {
                    (*it).Alive = false;
                    continue;
                }
                else
                {
                    (*it).Alive = false;
                    continue;
                }
            }

            if (sendByteMessage == 0)
            {
                (*it).Alive == false;
                continue;
            }

            //(*it).sendBuffer->moveBegin(peekSize);
            (*it).sendBuffer->Dequeue(message, peekSize);
        }
    }


}


//-------------------------------------------------------------
//  NET_PACKET_MP_PROC  // Make Packet
//-------------------------------------------------------------

void NET_PACKET_MP_HEADER(PACKET_HEADER* packet, int defineNum)
{
    switch (defineNum)
    {
    case dfPACKET_SC_CREATE_MY_CHARACTER:
    {
        packet->Code = 0x89;
        packet->Size = sizeof(PAKCET_SC_CREATE_MY_CHARACTER);
        packet->Type = dfPACKET_SC_CREATE_MY_CHARACTER;
        break;
    }
    case dfPACKET_SC_CREATE_OTHER_CHARACTER:
    {
        packet->Code = 0x89;
        packet->Size = sizeof(PACKET_SC_CREATE_OTHER_CHARACTER);
        packet->Type = dfPACKET_SC_CREATE_OTHER_CHARACTER;
        
        break;
    }
    case dfPACKET_SC_MOVE_START:
    {
        packet->Code = 0x89;
        packet->Size = sizeof(PACKET_SC_MOVE_START);
        packet->Type = dfPACKET_SC_MOVE_START;
        break;
    }
    case dfPACKET_SC_MOVE_STOP:
    {
        packet->Code = 0x89;
        packet->Size = sizeof(PACKET_SC_MOVE_STOP);
        packet->Type = dfPACKET_SC_MOVE_STOP;
        break;
    }
    case dfPACKET_SC_DELETE_CHARACTER:
    {
        packet->Code = 0x89;
        packet->Size = sizeof(PACKET_SC_DELETE_CHARACTER);
        packet->Type = dfPACKET_SC_DELETE_CHARACTER;
        break;
    }
    case dfPACKET_SC_ATTACK1:
    {
        packet->Code = 0x89;
        packet->Size = sizeof(PACKET_SC_ATTACK1);
        packet->Type = dfPACKET_SC_ATTACK1;
        break;
    }
    case dfPACKET_SC_ATTACK2:
    {
        packet->Code = 0x89;
        packet->Size = sizeof(PACKET_SC_ATTACK2);
        packet->Type = dfPACKET_SC_ATTACK2;
        break;
    }
    case dfPACKET_SC_ATTACK3:
    {
        packet->Code = 0x89;
        packet->Size = sizeof(PACKET_SC_ATTACK3);
        packet->Type = dfPACKET_SC_ATTACK3;
        break;
    }
    case dfPACKET_SC_DAMAGE:
    {
        packet->Code = 0x89;
        packet->Size = sizeof(PACKET_SC_DAMAGE);
        packet->Type = dfPACKET_SC_DAMAGE;
        break;
    }
    default:
        DebugBreak;
        break;
    }
}

void NET_PACKET_MP_MoveStart(CPacket* MakePacket, PACKET_CS_MOVE_START* payloadPacket, int ID)
{
    *MakePacket << ID;
    *MakePacket << payloadPacket->dir;
    *MakePacket << payloadPacket->x;
    *MakePacket << payloadPacket->y;
}

void NET_PACKET_MP_MoveStop(CPacket* MakePacket, PACKET_CS_MOVE_STOP* payloadPacket, int ID)
{
    *MakePacket << ID;
    *MakePacket << payloadPacket->dir;
    *MakePacket << payloadPacket->x;
    *MakePacket << payloadPacket->y;
}

void NET_PACKET_MP_Delete(CPacket* MakePacket, int ID)
{
    *MakePacket << ID;
}

void NET_PACKET_MP_ATTACK1(CPacket* MakePacket, PACKET_CS_ATTACK1* payloadPacket, int ID)
{
    *MakePacket << ID;
    *MakePacket << payloadPacket->dir;
    *MakePacket << payloadPacket->x;
    *MakePacket << payloadPacket->y;
}

void NET_PACKET_MP_ATTACK2(CPacket* MakePacket, PACKET_CS_ATTACK2* payloadPacket, int ID)
{
    *MakePacket << ID;
    *MakePacket << payloadPacket->dir;
    *MakePacket << payloadPacket->x;
    *MakePacket << payloadPacket->y;
}

void NET_PACKET_MP_ATTACK3(CPacket* MakePacket, PACKET_CS_ATTACK3* payloadPacket, int ID)
{
    *MakePacket << ID;
    *MakePacket << payloadPacket->dir;
    *MakePacket << payloadPacket->x;
    *MakePacket << payloadPacket->y;
}

void NET_PACKET_MP_Damage(CPacket* MakePacket, int attackerID, int damageID, char dagameHP)
{
    *MakePacket << attackerID;
    *MakePacket << damageID;
    *MakePacket << dagameHP;
}

//-------------------------------------------------------------
//  NET_PACKET_PROC  // Packet Procedure
//-------------------------------------------------------------

void NET_PACKET_PROC(Session* session, char* packet, int packetType)
{
    switch (packetType)
    {
    case dfPACKET_CS_MOVE_START:
    {
        PACKET_HEADER header;
        NET_PACKET_MP_HEADER(&header, dfPACKET_SC_MOVE_START);
        CPacket makePacket;
        NET_PACKET_MP_MoveStart(&makePacket, (PACKET_CS_MOVE_START*)packet, session->ID);

        ////////////////////////////////////////////////////////////////
                             // 로직, 분리 필요 //

        (*session).move = 1;
        (*session).move_Sub = 1;
        (*session).dir = ((PACKET_CS_MOVE_START*)packet)->dir;
        (*session).x = ((PACKET_CS_MOVE_START*)packet)->x;
        (*session).y = ((PACKET_CS_MOVE_START*)packet)->y;

        ////////////////////////////////////////////////////////////////

        NETWORK_BROADCAST(makePacket.GetBufferPtr(), session, &header);

        break;
    }
    case dfPACKET_CS_MOVE_STOP:
    {
        PACKET_HEADER header;
        NET_PACKET_MP_HEADER(&header, dfPACKET_SC_MOVE_STOP);
        CPacket makePacket;
        NET_PACKET_MP_MoveStop(&makePacket, (PACKET_CS_MOVE_STOP*)packet, session->ID);

        ////////////////////////////////////////////////////////////////
                             // 로직, 분리 필요 //

        (*session).move = 0;

        (*session).dir = ((PACKET_CS_MOVE_STOP*)packet)->dir;
        (*session).x = ((PACKET_CS_MOVE_STOP*)packet)->x;
        (*session).y = ((PACKET_CS_MOVE_STOP*)packet)->y;


        ////////////////////////////////////////////////////////////////

        NETWORK_BROADCAST(makePacket.GetBufferPtr(), session, &header);
        
        break;
    }
    case dfPACKET_CS_ATTACK1:
    {
        PACKET_HEADER header;
        NET_PACKET_MP_HEADER(&header, dfPACKET_SC_ATTACK1);
        CPacket makePacket;
        NET_PACKET_MP_ATTACK1(&makePacket, (PACKET_CS_ATTACK1*)packet, session->ID);

        NETWORK_BROADCAST(makePacket.GetBufferPtr(), session, &header);

        /////////////////////////////////////////////////////////////////
                             // 로직, 분리 필요 //
        
        // 1. 피격 판정과 브로드캐스
        
        attack_1_RangeInPlayer((PACKET_CS_ATTACK1*)packet, session->ID);


        /////////////////////////////////////////////////////////////////
        
        break;
    }
    case dfPACKET_CS_ATTACK2:
    {
        PACKET_HEADER header;
        NET_PACKET_MP_HEADER(&header, dfPACKET_SC_ATTACK2);
        CPacket makePacket;
        NET_PACKET_MP_ATTACK2(&makePacket, (PACKET_CS_ATTACK2*)packet, session->ID);

        NETWORK_BROADCAST(makePacket.GetBufferPtr(), session, &header);

        /////////////////////////////////////////////////////////////////
                             // 로직, 분리 필요 //

        // 1. 피격 판정과 브로드캐스트

        attack_2_RangeInPlayer((PACKET_CS_ATTACK2*)packet, session->ID);


        /////////////////////////////////////////////////////////////////
        break;
    }
    case dfPACKET_CS_ATTACK3:
    {
        PACKET_HEADER header;
        NET_PACKET_MP_HEADER(&header, dfPACKET_SC_ATTACK3);
        CPacket makePacket;
        NET_PACKET_MP_ATTACK3(&makePacket, (PACKET_CS_ATTACK3*)packet, session->ID);

        NETWORK_BROADCAST(makePacket.GetBufferPtr(), session, &header);

        /////////////////////////////////////////////////////////////////
                             // 로직, 분리 필요 //

        // 1. 피격 판정과 브로드캐스트

        attack_3_RangeInPlayer((PACKET_CS_ATTACK3*)packet, session->ID);


        /////////////////////////////////////////////////////////////////
        break;
    }
    default:
        DebugBreak;
        break;
    }
}

void power();