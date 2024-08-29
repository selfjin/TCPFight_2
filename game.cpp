#include "game.h"
#include "PacketDefine.h"


std::pair<int, int> moveDir[8] =		// x, y
{
    { -3, 0 },
    { -3, -2 },
    { 0, -2 },
    { 3, -2 },
    { 3, 0 },
    { 3, 2 },
    { 0, 2},
    { -3, 2}
};

//-------------------------------------------------------------
// 범위 판정
//-------------------------------------------------------------

//             공격 방향,   공격자, 공격범위,    위      아래
bool isWithRange(int dir, int x, int x2, int y1, int y2, int enemy_X, int enemy_Y)
{

    if (dir == dfPACKET_MOVE_DIR_LL)  // 왼쪽 방향
    {
        // 적 Y의 좌표는 y1과 y2의 사이, enemy_X는 x와 x2 사이에 있어야 함 (x2는 x보다 작음)
        return (enemy_X <= x && enemy_X >= x2 && enemy_Y >= y1 && enemy_Y <= y2);
    }
    else if (dir == dfPACKET_MOVE_DIR_RR)  // 오른쪽 방향
    {
        // 적 Y의 좌표는 y1과 y2 사이, enemy_X는 x2보다 크고 x보다 작아야 함 (x2는 x보다 큼)
        return (enemy_X >= x && enemy_X <= x2 && enemy_Y >= y1 && enemy_Y <= y2);
    }
}


void Update()
{
    auto movePlayer = playerList.begin();

    while (movePlayer != playerList.end())
    {
        if ((*movePlayer)->Alive != false)
        {
            if ((*movePlayer)->move != 0)
            {
                auto move = moveDir[(*movePlayer)->dir];

                (*movePlayer)->x += move.first;
                (*movePlayer)->y += move.second;

                if ((*movePlayer)->x <= 50 && (*movePlayer)->x >= 470)
                {
                    (*movePlayer)->move = 0;
                    (*movePlayer)->move_Sub = 0;
                }

                if ((*movePlayer)->y <= 10 && (*movePlayer)->y >= 630)
                {
                    (*movePlayer)->move = 0;
                    (*movePlayer)->move_Sub = 0;
                }
            }
            else
            {
                if ((*movePlayer)->move_Sub == true)  // 한 프레임에 이동과 멈춤이 같은 Recv에 있음
                {
                    auto move = moveDir[(*movePlayer)->dir];

                    (*movePlayer)->x += move.first;
                    (*movePlayer)->y += move.second;

                    (*movePlayer)->move_Sub = 0;
                }
            }
        }

        ++movePlayer;  // 이터레이터 증가를 루프 마지막에 한 번만 수행
    }


    for (auto hpPlayer : playerList)
    {
        if ((*hpPlayer).hp <= 0)
        {
            (*hpPlayer).Alive = false;
        }
    }
    
}


void netSession()
{
    for (auto it : playerList)
    {
        if ((*it).Alive == false)
        {
            PACKET_HEADER header;
            NET_PACKET_MP_HEADER(&header, dfPACKET_SC_DELETE_CHARACTER);
            PACKET_SC_DELETE_CHARACTER delete_Packet;
            NET_PACKET_MP_Delete(&delete_Packet, (*it).ID);

            
            NETWORK_BROADCAST((char*)&delete_Packet, it, &header);
        }
    }

    auto deletePlayer = playerList.begin();

    while (deletePlayer != playerList.end())
    {
        if ((*deletePlayer)->Alive == false)
        {
            closesocket((*deletePlayer)->Sock);
            delete (*deletePlayer);

            deletePlayer = playerList.erase(deletePlayer);
        }
        else
        {
            ++deletePlayer;
        }
    }
}