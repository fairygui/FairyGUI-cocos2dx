#include "ByteBuffer.h"

NS_FGUI_BEGIN
using namespace std;

ByteBuffer::ByteBuffer(char* buffer, int offset, int len, bool transferOwnerShip = false) :
    _buffer(buffer),
    position(0),
    _offset(offset),
    _length(len),
    _littleEndian(false),
    _ownsBuffer(transferOwnerShip),
    stringTable(nullptr),
    version(0)
{
}

ByteBuffer::~ByteBuffer()
{
    if (_ownsBuffer && _buffer != nullptr)
        delete _buffer;
}

int ByteBuffer::getBytesAvailable() const
{
    return _length - position;
}

char ByteBuffer::ReadByte()
{
    signed char val = _buffer[_offset + position];
    if (val > 127)
        val = val - 255;
    position += 1;
    return val;
}

unsigned char ByteBuffer::ReadUbyte() {
    unsigned char val = _buffer[_offset + position];
    position += 1;
    return val;
}

bool ByteBuffer::ReadBool()
{
    return ReadByte() == 1;
}

short ByteBuffer::ReadShort()
{
    int startIndex = _offset + position;
    position += 2;
    unsigned char* pbyte = (unsigned char*)(_buffer + startIndex);
    if (_littleEndian)
        return (short)((*pbyte) | (*(pbyte + 1) << 8));
    else
        return (short)((*pbyte << 8) | (*(pbyte + 1)));
}

unsigned short ByteBuffer::ReadUshort()
{
    return (unsigned short)ReadShort();
}

int ByteBuffer::ReadInt()
{
    int startIndex = _offset + position;
    position += 4;
    unsigned char* pbyte = (unsigned char*)(_buffer + startIndex);
    if (_littleEndian)
        return (*pbyte) | (*(pbyte + 1) << 8) | (*(pbyte + 2) << 16) | (*(pbyte + 3) << 24);
    else
        return (*pbyte << 24) | (*(pbyte + 1) << 16) | (*(pbyte + 2) << 8) | (*(pbyte + 3));
}

unsigned int ByteBuffer::ReadUint()
{
    return (unsigned int)ReadInt();
}

float ByteBuffer::ReadFloat()
{
    int val = ReadInt();
    return *(float*)&val;
}

std::string ByteBuffer::ReadString()
{
    int len = ReadUshort();
    return ReadString(len);
}

std::string ByteBuffer::ReadString(int len)
{
    char* value = new char[len + 1];

    value[len] = '\0';
    memcpy(value, _buffer + position, len);
    position += len;

    string str(value);
    delete[] value;
    value = nullptr;

    return str;
}

const std::string& ByteBuffer::ReadS()
{
    int index = ReadUshort();
    if (index == 65534 || index == 65533)
        return cocos2d::STD_STRING_EMPTY;
    else
        return (*stringTable)[index];
}

bool ByteBuffer::ReadS(std::string& result)
{
    int index = ReadUshort();
    if (index == 65534) //null
        return false;
    else if (index == 65533)
    {
        result.clear();
        return true;
    }
    else
    {
        result = (*stringTable)[index];
        return true;
    }
}

const string* ByteBuffer::ReadSP()
{
    int index = ReadUshort();
    if (index == 65534) //null
        return nullptr;
    else if (index == 65533)
        return &cocos2d::STD_STRING_EMPTY;
    else
        return &(*stringTable)[index];
}

void ByteBuffer::WriteS(const std::string& value)
{
    int index = ReadUshort();
    if (index != 65534 && index != 65533)
        (*stringTable)[index] = value;
}

cocos2d::Color4B ByteBuffer::ReadColor()
{
    int startIndex = _offset + position;
    GLubyte r = _buffer[startIndex];
    GLubyte g = _buffer[startIndex + 1];
    GLubyte b = _buffer[startIndex + 2];
    GLubyte a = _buffer[startIndex + 3];
    position += 4;

    return cocos2d::Color4B(r, g, b, a);
}

ByteBuffer* ByteBuffer::ReadBuffer()
{
    int count = ReadInt();
    char* p = (char*)malloc(count);
    memcpy(p, _buffer + position, count);
    ByteBuffer* ba = new ByteBuffer(p, 0, count, true);
    ba->stringTable = stringTable;
    ba->version = version;
    position += count;
    return ba;
}

bool ByteBuffer::Seek(int indexTablePos, int blockIndex)
{
    int tmp = position;
    position = indexTablePos;
    int segCount = _buffer[_offset + position++];
    if (blockIndex < segCount)
    {
        bool useShort = _buffer[_offset + position++] == 1;
        int newPos;
        if (useShort)
        {
            position += 2 * blockIndex;
            newPos = ReadShort();
        }
        else
        {
            position += 4 * blockIndex;
            newPos = ReadInt();
        }

        if (newPos > 0)
        {
            position = indexTablePos + newPos;
            return true;
        }
        else
        {
            position = tmp;
            return false;
        }
    }
    else
    {
        position = tmp;
        return false;
    }
}

NS_FGUI_END
