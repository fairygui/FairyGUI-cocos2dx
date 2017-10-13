//
//  ByteArray.hpp
//  GameProject
//
//  Created by jt on 15-09-04.
//
//

#ifndef __JT_BYTEARRAY_H_
#define __JT_BYTEARRAY_H_


#include "cocos2d.h"
USING_NS_CC;

#include <string>
using namespace std;

class ByteArray : public Ref
{
public:
    static ByteArray* create(int len, int endian = ENDIAN_LITTLE);


    static ByteArray* createWithBuffer(char* buffer, int len, int endian = ENDIAN_LITTLE);

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

    string readString(int len);
    void writeString(string value);

    char readByte();
    void writeByte(char value);

    unsigned char readUnsignedByte();
    void writeUnsignedChar(unsigned char value);

    const char* getBuffer();
    void clear();

    int getEndian();
    void setEndian(int n);

    int getPosition();
    void setPosition(int _pos);

    int getBytesAvailable();
    int getLength();

    virtual ~ByteArray();
protected:
    ByteArray();

private:
    void copyMemory(void* to, const void* from, int len);

    char* _buffer;
    int _pos;
    int _length;
    int _endian;
    short _flag;
};

#endif // __JT_BYTEARRAY_H_
