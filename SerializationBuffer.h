#pragma once
#include <Windows.h>

class CPacket
{
public:

	/*---------------------------------------------------------------
	Packet Enum.

	----------------------------------------------------------------*/
	enum en_PACKET
	{
		eBUFFER_DEFAULT = 1000		// ��Ŷ�� �⺻ ���� ������.
	};

	CPacket();
	CPacket(int capacity);
	virtual	~CPacket();

	int getCapacity() { return _capacity; }
	int getSize() { return _size; }
	int Enqueue(char* data, int bytes);
	int Dequeue(char* data, int bytes);

	void Clear(void);



	///////////////////////////////////////////////
	int		moveBegin(int value);
	int		moveEnd(int value);

	char* GetBufferPtr(void) { return buffer; }		// Buffer�� �ּҸ� ��ȯ

	///////////////////////////////////////////////

	/* ============================================================================= */
	// ������ �����ε�
	/* ============================================================================= */
	CPacket& operator = (CPacket& clSrcPacket);

	//////////////////////////////////////////////////////////////////////////
	// �ֱ�.	�� ���� Ÿ�Ը��� ��� ����.
	//////////////////////////////////////////////////////////////////////////
	CPacket& operator << (unsigned char byValue);
	CPacket& operator << (char chValue);

	CPacket& operator << (short shValue);
	CPacket& operator << (unsigned short wValue);

	CPacket& operator << (int iValue);
	CPacket& operator << (long lValue);
	CPacket& operator << (float fValue);

	CPacket& operator << (__int64 iValue);
	CPacket& operator << (double dValue);


	//////////////////////////////////////////////////////////////////////////
	// ����.	�� ���� Ÿ�Ը��� ��� ����.
	//////////////////////////////////////////////////////////////////////////
	CPacket& operator >> (BYTE& byValue);
	CPacket& operator >> (char& chValue);

	CPacket& operator >> (short& shValue);
	CPacket& operator >> (WORD& wValue);

	CPacket& operator >> (int& iValue);
	CPacket& operator >> (DWORD& dwValue);
	CPacket& operator >> (float& fValue);

	CPacket& operator >> (__int64& iValue);
	CPacket& operator >> (double& dValue);



protected:
	int _begin_index;
	int _end_index;
	int	_capacity;	// ���� �ִ� ������
	int	_size;		// ��� ���� ������
	char* buffer;
};