//
//  ByteArray.hpp
//  GameProject
//
//  Created by jt on 15-09-04.
//
//

#ifndef __BYTEARRAY_H_
#define __BYTEARRAY_H_

#include "cocos2d.h"
#include <string>

class ByteArray
{
public:
    static ByteArray* create(size_t len);
    static ByteArray* createWithBuffer(char* buffer, size_t len, bool transferOwnerShip = false);

    static int checkCPUEndian();

    static const int ENDIAN_LITTLE = 0;
    static const int ENDIAN_BIG = 1;

    bool readBool();
    void writeBool(bool value);

    short readShort();
    void writeShort(short value);

    unsigned short readUnsignedShort();
    void writeUnsignedShort(unsigned short value);

    int readInt();
    void writeInt(int value);

    unsigned int readUnsignedInt();
    void writeUnsignedInt(unsigned int value);

    long long readLongLong();
    void writeLongLong(long long value);

    std::string readString();
    std::string readString(size_t len);
    void writeString(const std::string& value);

    char readByte();
    void writeByte(char value);

    unsigned char readUnsignedByte();
    void writeUnsignedChar(unsigned char value);

    const char* getBuffer();
    void clear();

    int getEndian();
    void setEndian(int n);

    size_t getPosition();
    void setPosition(size_t _pos);

    size_t getBytesAvailable();
    size_t getLength();

    virtual ~ByteArray();
protected:
    ByteArray();

private:
    void copyMemory(void* to, const void* from, size_t len);

    char* _buffer;
    ssize_t _pos;
    size_t _length;
    int _endian;
    short _flag;
};

#endif
