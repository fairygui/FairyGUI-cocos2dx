#include "ByteArray.h"

using namespace std;

ByteArray::ByteArray() :
    _buffer(nullptr),
    _pos(0),
    _length(0),
    _endian(ENDIAN_LITTLE),
    _flag(-1)
{
};


ByteArray::~ByteArray()
{
    if (this->_flag == 0 && this->_buffer) {
        delete[] this->_buffer;
        this->_buffer = nullptr;
        this->_flag = -1;
    }
};




ByteArray* ByteArray::create(int len, int endian/* = 0 */)
{
    ByteArray* ba = new ByteArray();
    if (ba)
    {
        ba->_buffer = new char[len];
        ba->_endian = endian;
        ba->_length = len;
        ba->_flag = 0;
    }
    else {
        delete ba;
        ba = nullptr;
        return nullptr;
    }

    return ba;
}


ByteArray* ByteArray::createWithBuffer(char* buffer, int len, int endian/* = 0 */)
{
    ByteArray* ba = new ByteArray();
    if (ba)
    {
        ba->_buffer = buffer;
        ba->_length = len;
        ba->_flag = 1;
        return ba;
    }
    else {
        delete ba;
        ba = NULL;
        return NULL;
    }
    return ba;
}




bool ByteArray::readBool()
{
    bool b;
    char* p = (char*)&b;
    copyMemory(p, this->_buffer + this->_pos, sizeof(b));
    this->_pos += sizeof(b);
    return b;
}


void ByteArray::writeBool(bool value)
{
    char* p = (char*)&value;
    memcpy(this->_buffer + this->_pos, p, sizeof(value));
    this->_pos += sizeof(value);
}


short ByteArray::readShort()
{
    short n = 0;
    char* p = (char*)&n;
    copyMemory(p, this->_buffer + this->_pos, sizeof(n));
    this->_pos += 2;
    return n;
}


void ByteArray::writeShort(short value)
{
    char* p = (char*)&value;
    memcpy(this->_buffer + this->_pos, p, sizeof(value));
    this->_pos += 2;
}


int ByteArray::readInt() {
    int n;
    char* p = (char*)&n;
    copyMemory(p, this->_buffer + this->_pos, sizeof(n));
    this->_pos += 4;
    return n;
}


void ByteArray::writeInt(int value)
{
    char* p = (char*)&value;
    memcpy(this->_buffer + this->_pos, p, sizeof(value));
    this->_pos += 4;
}


unsigned int ByteArray::readUnsignedInt() {
    unsigned int n;
    char* p = (char*)&n;
    copyMemory(p, this->_buffer + this->_pos, sizeof(n));

    this->_pos += 4;
    return n;
}


void ByteArray::writeUnsignedInt(unsigned int value)
{
    char* p = (char*)&value;
    memcpy(this->_buffer + this->_pos, p, sizeof(value));
    this->_pos += 4;
}


unsigned short ByteArray::readUnsignedShort() {
    unsigned short n;
    char* p = (char*)&n;
    copyMemory(p, this->_buffer + this->_pos, sizeof(n));
    this->_pos += 2;
    return n;
}


void ByteArray::writeUnsignedShort(unsigned short value)
{
    char* p = (char*)&value;
    memcpy(this->_buffer + this->_pos, p, sizeof(value));
    this->_pos += 2;
}


char ByteArray::readByte() {
    signed char val = this->_buffer[this->_pos];
    if (val > 127) {
        val = val - 255;
    }
    this->_pos += 1;
    return val;
}


void ByteArray::writeByte(char value)
{
    char* p = (char*)&value;
    memcpy(this->_buffer + this->_pos, p, sizeof(value));
    this->_pos += 1;
}


unsigned char ByteArray::readUnsignedByte() {
    unsigned char val = this->_buffer[this->_pos];
    this->_pos += 1;
    return val;
}


void ByteArray::writeUnsignedChar(unsigned char value)
{
    char* p = (char*)&value;
    memcpy(this->_buffer + this->_pos, p, sizeof(value));
    this->_pos += 1;
}


long long ByteArray::readLongLong()
{
    long long n;
    char* p = (char*)&n;
    copyMemory(p, this->_buffer + this->_pos, sizeof(n));
    this->_pos += sizeof(n);
    return n;
}


void ByteArray::writeLongLong(long long value)
{
    char* p = (char*)&value;
    memcpy(this->_buffer + this->_pos, p, sizeof(value));
    this->_pos += sizeof(value);
}

string ByteArray::readString(int len)
{
    char* value = new char[len + 1];

    value[len] = '\0';
    copyMemory(value, this->_buffer + this->_pos, len);
    this->_pos += len;

    string str(value);
    delete[] value;
    value = nullptr;

    return str;
}


void ByteArray::writeString(const string& value)
{
    auto len = value.length();
    const char* p = value.c_str();
    memcpy(this->_buffer + this->_pos, p, len);
    this->_pos += len;
}




void ByteArray::copyMemory(void* to, const void* from, int len)
{
    char* t = (char*)to;
    char* f = (char*)from;
    if (_endian == ENDIAN_LITTLE)
    {
        memcpy(t, f, len);
    }
    else if (_endian == ENDIAN_BIG)
    {
        for (int i = len - 1; i >= 0; i--)
        {
            t[(len - 1) - i] = f[i];
        }
    }
}


const char* ByteArray::getBuffer()
{
    return this->_buffer;
}


void ByteArray::clear()
{
    memset(this->_buffer, 0, this->_length);
}


//0: little 1:big
int ByteArray::checkCPUEndian() {
    union w
    {
        int a;
        char b;
    } c;
    c.a = 1;
    return  (c.b == 1 ? ENDIAN_LITTLE : ENDIAN_BIG);
}


void ByteArray::setEndian(int n)
{
    this->_endian = n;
}


int ByteArray::getEndian()
{
    return this->_endian;
}


int ByteArray::getPosition()
{
    return this->_pos;
}


void ByteArray::setPosition(int pos)
{
    this->_pos = pos;
}


int ByteArray::getLength()
{
    return this->_length;
}


int ByteArray::getBytesAvailable()
{
    return this->_length - this->_pos;
}

