#include "RingBuffer.h"



RingBuffer::RingBuffer(int capacity)
    : _begin_index(0)
    , _end_index(0)
    , _size(0)
    , _capacity(capacity)
{
    buffer = new char[capacity];
}

RingBuffer::~RingBuffer()
{
    delete[] buffer;
}

int RingBuffer::Enqueue(const char* data, int bytes)
{
    if (bytes == 0) return 0;

    int capacity = _capacity;
    int bytes_to_write = min(bytes, capacity - _size); // Free Size�� ��

    // ����Ⱑ �߻��ϴ��� Ȯ��
    if (_size + bytes_to_write > capacity) {
        // ����Ⱑ �߻��� ���ɼ��� �ִ� ���, ���⸦ ���� ( ���� �׽�Ʈ�� ���� �Ϻθ� ���� )
        bytes_to_write = capacity - _size;
    }

    // ���� ������ �ϴ� ����Ʈ��, ( Free Size ) ���� �۰ų� ���ٸ�
    if (bytes_to_write <= capacity - _end_index)
    {
        memcpy(buffer + _end_index, data, bytes_to_write);
        _end_index += bytes_to_write;
        if (_end_index == capacity) _end_index = 0;
    }
    // �Ѿ����� ....
    else
    {
        int Use_Size = capacity - _end_index;  // Use Size
        memcpy(buffer + _end_index, data, Use_Size);  // ��� ������ ��ŭ ����

        int Remaining_Byte = bytes_to_write - Use_Size;  // �ϴ� �� ����Ʈ ����, ���� ����Ʈ ������
        memcpy(buffer, data + Use_Size, Remaining_Byte); // �տ��ٰ� ��

        _end_index = Remaining_Byte;
    }

    _size += bytes_to_write;
    return bytes_to_write;
}

int RingBuffer::Dequeue(char* data, int bytes)
{
    if (bytes == 0) return 0;

    int capacity = _capacity;
    int bytes_to_read = min(bytes, _size);

    // Read in a single step
    if (bytes_to_read <= capacity - _begin_index)
    {
        memcpy(data, buffer + _begin_index, bytes_to_read);
        _begin_index += bytes_to_read;
        if (_begin_index == capacity) _begin_index = 0;
    }
    // Read in two steps
    else
    {
        int size_1 = capacity - _begin_index;
        memcpy(data, buffer + _begin_index, size_1);
        int size_2 = bytes_to_read - size_1;
        memcpy(data + size_1, buffer, size_2);
        _begin_index = size_2;
    }

    _size -= bytes_to_read;
    return bytes_to_read;
}


int RingBuffer::peek(char* data, int bytes)
{
    if (bytes == 0) return 0;

    int capacity = _capacity;
    int bytes_to_read = min(bytes, _size);

    // Read in a single step
    if (bytes_to_read <= capacity - _begin_index)
    {
        memcpy(data, buffer + _begin_index, bytes_to_read);
    }
    // Read in two steps
    else
    {
        int size_1 = capacity - _begin_index;
        memcpy(data, buffer + _begin_index, size_1);
        int size_2 = bytes_to_read - size_1;
        memcpy(data + size_1, buffer, size_2);
    }

    return bytes_to_read;
}

void RingBuffer::moveBegin(int value)
{

    _begin_index = (_begin_index + value) % _capacity;

    _size -= value;
}


