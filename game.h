#pragma once
#include <list>
#include "Network.h"

//-------------------------------------------------------------
// MAP define
//-------------------------------------------------------------

#define SCREEN_WIDTH 640		// MAP �ʺ�
#define SCREEN_HEIGHT 380		// MAP ����


#define dfRANGE_MOVE_TOP	50
#define dfRANGE_MOVE_LEFT	10
#define dfRANGE_MOVE_RIGHT	630
#define dfRANGE_MOVE_BOTTOM	470

//-------------------------------------------------------------
// DIR VECTOR / ���� ���� / ���� ���� / ���ݷ�
//-------------------------------------------------------------

#define dfPACKET_MOVE_DIR_LL					0
#define dfPACKET_MOVE_DIR_LU					1
#define dfPACKET_MOVE_DIR_UU					2
#define dfPACKET_MOVE_DIR_RU					3
#define dfPACKET_MOVE_DIR_RR					4
#define dfPACKET_MOVE_DIR_RD					5
#define dfPACKET_MOVE_DIR_DD					6
#define dfPACKET_MOVE_DIR_LD					7

#define dfATTACK1_RANGE_X		80
#define dfATTACK2_RANGE_X		90
#define dfATTACK3_RANGE_X		100
#define dfATTACK1_RANGE_Y		10
#define dfATTACK2_RANGE_Y		10
#define dfATTACK3_RANGE_Y		20



#define dfATTACK1_DAMAGE 8
#define dfATTACK2_DAMAGE 14
#define dfATTACK3_DAMAGE 18

//-------------------------------------------------------------
// extern ���� ����
//-------------------------------------------------------------


//---------------------------------------------------------------------/
//							 Game Rogic
//--------------------------------------------------------------------/

extern std::pair<int, int> moveDir[8];

///////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------
// ���� ����
//-------------------------------------------------------------

void Update();

//-------------------------------------------------------------
// ���� ����
//-------------------------------------------------------------


//               ���� ����, ĳ����, ���ݹ���,    ��,    �Ʒ�,        ���x,       ���y
bool isWithRange(int dir, int x, int x2, int y1, int y2, int enemy_X, int enemy_Y);


void attack_1_RangeInPlayer(PACKET_CS_ATTACK1* payloadPacket, int attackerID);

void attack_2_RangeInPlayer(PACKET_CS_ATTACK2* payloadPacket, int attackerID);

void attack_3_RangeInPlayer(PACKET_CS_ATTACK3* payloadPacket, int attackerID);