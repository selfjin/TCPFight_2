#include "SerializationBuffer.h"



CPacket::CPacket() :
    _begin_index(0),
    _end_index(0),
    _size(0),
    _capacity(en_PACKET::eBUFFER_DEFAULT)
{
    buffer = new char[_capacity];
}

CPacket::CPacket(int capacity) :
    _begin_index(0),
    _end_index(0),
    _size(0),
    _capacity(capacity)
{
    buffer = new char[_capacity];
}

CPacket::~CPacket()
{
    delete[] buffer;
}

int CPacket::Enqueue(char* data, int bytes)
{

#ifdef DEBUG
    if (bytes > _capacity - _size)
    {
        DebugBreak;
    }
#endif

    memcpy(buffer + _end_index, data, bytes);
    _end_index += bytes;
    _size += bytes;

    return bytes;

}

int CPacket::Dequeue(char* data, int bytes)
{

#ifdef DEBUG
    if (bytes > _size)
    {
        DebugBreak;
    }
#endif

    memcpy(data, buffer + _begin_index, bytes);
    _begin_index += bytes;
    _size -= bytes;

    return bytes;
}

void CPacket::Clear(void)
{
    // 기능 추가 예정
}

int CPacket::moveBegin(int value)
{
    _begin_index += value;

#ifdef DEBUG
    if (_begin_index > _end_index)
    {
        DebugBreak;
    }
#endif

    _size -= value;

    return _begin_index;
}

// _end_index --> Right Move
int CPacket::moveEnd(int value)
{
    _end_index += value;

#ifdef DEBUG
    if (_end_index > _capacity)
    {
        DebugBreak;
    }
#endif

    _size += value;

    return _end_index;
}

// 대입 연산자 오버로딩
CPacket& CPacket::operator<<(unsigned char byValue)
{
    *(reinterpret_cast<unsigned char*>(buffer + _end_index)) = byValue;
    _end_index += sizeof(byValue);
    _size += sizeof(byValue);
    return *this;
}

CPacket& CPacket::operator<<(char chValue)
{
    *(reinterpret_cast<char*>(buffer + _end_index)) = chValue;
    _end_index += sizeof(chValue);
    _size += sizeof(chValue);
    return *this;
}

CPacket& CPacket::operator<<(short shValue)
{
    *(reinterpret_cast<short*>(buffer + _end_index)) = shValue;
    _end_index += sizeof(shValue);
    _size += sizeof(shValue);
    return *this;
}

CPacket& CPacket::operator<<(unsigned short wValue)
{
    *(reinterpret_cast<unsigned short*>(buffer + _end_index)) = wValue;
    _end_index += sizeof(wValue);
    _size += sizeof(wValue);
    return *this;
}

CPacket& CPacket::operator<<(int iValue)
{
    *(reinterpret_cast<int*>(buffer + _end_index)) = iValue;
    _end_index += sizeof(iValue);
    _size += sizeof(iValue);
    return *this;
}

CPacket& CPacket::operator<<(long lValue)
{
    *(reinterpret_cast<long*>(buffer + _end_index)) = lValue;
    _end_index += sizeof(lValue);
    _size += sizeof(lValue);
    return *this;
}

CPacket& CPacket::operator<<(float fValue)
{
    *(reinterpret_cast<float*>(buffer + _end_index)) = fValue;
    _end_index += sizeof(fValue);
    _size += sizeof(fValue);
    return *this;
}

CPacket& CPacket::operator<<(__int64 iValue)
{
    *(reinterpret_cast<__int64*>(buffer + _end_index)) = iValue;
    _end_index += sizeof(iValue);
    _size += sizeof(iValue);
    return *this;
}

CPacket& CPacket::operator<<(double dValue)
{
    *(reinterpret_cast<double*>(buffer + _end_index)) = dValue;
    _end_index += sizeof(dValue);
    _size += sizeof(dValue);
    return *this;
}

// >> 연산자 오버로딩 (데이터 추출)
CPacket& CPacket::operator>>(BYTE& byValue)
{
    byValue = *(reinterpret_cast<BYTE*>(buffer + _begin_index));
    _begin_index += sizeof(byValue);
    _size -= sizeof(byValue);
    return *this;
}

CPacket& CPacket::operator>>(char& chValue)
{
    chValue = *(reinterpret_cast<char*>(buffer + _begin_index));
    _begin_index += sizeof(chValue);
    _size -= sizeof(chValue);
    return *this;
}

CPacket& CPacket::operator>>(short& shValue)
{
    shValue = *(reinterpret_cast<short*>(buffer + _begin_index));
    _begin_index += sizeof(shValue);
    _size -= sizeof(shValue);
    return *this;
}

CPacket& CPacket::operator>>(WORD& wValue)
{
    wValue = *(reinterpret_cast<WORD*>(buffer + _begin_index));
    _begin_index += sizeof(wValue);
    _size -= sizeof(wValue);
    return *this;
}

CPacket& CPacket::operator>>(int& iValue)
{
    iValue = *(reinterpret_cast<int*>(buffer + _begin_index));
    _begin_index += sizeof(iValue);
    _size -= sizeof(iValue);
    return *this;
}

CPacket& CPacket::operator>>(DWORD& dwValue)
{
    dwValue = *(reinterpret_cast<DWORD*>(buffer + _begin_index));
    _begin_index += sizeof(dwValue);
    _size -= sizeof(dwValue);
    return *this;
}

CPacket& CPacket::operator>>(float& fValue)
{
    fValue = *(reinterpret_cast<float*>(buffer + _begin_index));
    _begin_index += sizeof(fValue);
    _size -= sizeof(fValue);
    return *this;
}

CPacket& CPacket::operator>>(__int64& iValue)
{
    iValue = *(reinterpret_cast<__int64*>(buffer + _begin_index));
    _begin_index += sizeof(iValue);
    _size -= sizeof(iValue);
    return *this;
}

CPacket& CPacket::operator>>(double& dValue)
{
    dValue = *(reinterpret_cast<double*>(buffer + _begin_index));
    _begin_index += sizeof(dValue);
    _size -= sizeof(dValue);
    return *this;
}


/*
* 
*   크기를 넘으면 어떻게 할 거냐.
* 
*   1. 예외를 던지는 방법, Catch 해서 받아야 함.
* 
*   2. 다 넣은 다음에 If 문을 통해, 
*/


/*
* 
*  1. 메시지가 생성이 됨.
* 
* 
*  2. d가 안 옴
* 
* 
*  3. 메시지 수신 -> A B C D 인 줄 알았는데 A B C 만 나옴
* 
*     끊어요, 서버의 문제는 아님, 쟤가 잘 못 보낸거임.
* 
*  
*  아이템 거래 같은 코드에서는 -> 거래 개수 ( cnt ) 
*  반복문
* 
*  위험성 - > 
* 
*  생각보다 보안이 허술합니다.
* 
*  네트워크 수신 때는 길이를 확인할 수 있다는 거지.
*  네트워크 수신 때는 판별한다는 건 -> 
* 
*  네트워크용 헤더, 컨텐츠용 헤더
* 
*  컨텐츠에서는 길이도 모르는거야 . 헤더가 안 오는 거임.
* 
*  직렬화 버퍼를 통해 알아내는건 가능하겠죠.
* 
*  우리에게 TCP 헤더를 건들 수는 없잖아요.
* 
*  네트워크 라이브러리 코드와 컨텐츠 코드의 분리
*/



/*
pItem
:

pInventory
:

아이템을 직렬화 버퍼에 바로

아이템 리스트 자체를 직렬화 버퍼에 바로

실수를 줄이고, 트래픽을 높이고

메시지 구조가 객체지향적

*/


/*
* 
* 
*  RPC -> 편의성 증가, 성능 감소
* 
*  캐스팅을 왜 그렇게 하였을까 흑흑
* 
*  RPC가 대체 뭘까
* 
*  만들기 전까지는 모르겠습니다.
* 
* 
*  Engine FrameWork
* 
*  RPC(UNITY, UNREAD)
* 
*  MoveStart -> 
*  
* 
*  외형적인 내용만 보니까 -> 
   
   moveStart() 때리니까 , 서버 측의 moveStart() 실행이 되더라!

   네트워크 송수신 메시지를 블랙 박스화 시키는 것

   라이브러리가 해 줄 수 있는 것


   사용자가 함수에 대한 선언만 하면 

   모든 코드가 자동으로 만들어져야 한다.

   
   int a, int b 

   컨텐츠 개발자가 필요한 코드를 자동으로 만들어지는 뭔가가 있다면 RPC

   1. 송신부

   - > 

   2. 수신부

   - > 

   RPC 객체라는 느낌이라 .


   RPC -> 결국 코드 만드는 템플릿인데... 쓰압




*/