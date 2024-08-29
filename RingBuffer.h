#pragma once
#include <iostream>
#include <Windows.h>


//-------------------------------------------------------------
// define �� ���� ����
//-------------------------------------------------------------

#define RINBBUFFER_SIZE 10000


// �ش� �� ���۴� ����� �ݵ�� üũ�ؼ�, ���� �� ���� �� �ϼ���.
// ���� ����� Free Size ��ŭ�� ������, ©�󳻰� �ֽ��ϴ�. ��¥. 
// �� �� �� Capacity - Size = Free Size üũ

class RingBuffer
{
public:
    RingBuffer(int capacity);
    ~RingBuffer();

    int getSize() { return _size; }
    int getCapacity() const { return _capacity; }
    int Enqueue(const char* data, int bytes);
    int Dequeue(char* data, int bytes);
    int peek(char* data, int bytes);


    ///////////////////////////////////////////////


    char* GetFrontBufferPtr() { return &buffer[_begin_index]; }
    char* getRearBufferPtr() { return &buffer[_end_index]; }

    void moveBegin(int value);

    ///////////////////////////////////////////////

private:
    int _begin_index;
    int _end_index;
    int _size;
    int _capacity;
    char* buffer;
};

