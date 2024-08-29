#pragma once

#pragma pack(push, 1)

struct PACKET_HEADER						// Çì´õ
{
	unsigned char Code;
	unsigned char Size;
	unsigned char Type;
};


struct PAKCET_SC_CREATE_MY_CHARACTER		// 0    S->C  CREATE CHARACTER
{
	int ID;
	char dir;
	short x;
	short y;
	char hp;
};


struct PACKET_SC_CREATE_OTHER_CHARACTER		// 1    
{
	int ID;
	char dir;
	short x;
	short y;
	char hp;
};


struct PACKET_SC_DELETE_CHARACTER			// 2
{
	int ID;
};


struct PACKET_CS_MOVE_START					// 10
{
	char dir;
	short x;
	short y;
};



struct PACKET_SC_MOVE_START					// 11 
{
	int ID;
	char dir;
	short x;
	short y;
};



struct PACKET_CS_MOVE_STOP					// 12
{
	char dir;
	short x;
	short y;
};



struct PACKET_SC_MOVE_STOP					// 13
{
	int ID;
	char dir;
	short x;
	short y;
};



struct PACKET_CS_ATTACK1					// 20
{
	char dir;
	short x;
	short y;
};


struct PACKET_SC_ATTACK1					// 21
{
	int ID;
	char dir;
	short x;
	short y;
};


struct PACKET_CS_ATTACK2					// 22
{
	char dir;
	short x;
	short y;
};


struct PACKET_SC_ATTACK2					// 23
{
	int ID;
	char dir;
	short x;
	short y;
};


struct PACKET_CS_ATTACK3					// 24
{
	char dir;
	short x;
	short y;
};


struct PACKET_SC_ATTACK3					// 25
{
	int ID;
	char dir;
	short x;
	short y;
};



struct PACKET_SC_DAMAGE						// 30
{
	int AttackID;
	int DamageID;
	char DamageHP;
};


#pragma pack(pop)