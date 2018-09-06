#ifndef __BYTEBUFFER_H_
#define __BYTEBUFFER_H_

#include "cocos2d.h"
#include "FairyGUIMacros.h"

NS_FGUI_BEGIN

class ByteBuffer
{
public:
    ByteBuffer(char* buffer, int offset, int len, bool transferOwnerShip);
    ~ByteBuffer();

    const char* getBuffer() const { return _buffer; }

    bool isLittleEndian() const { return _littleEndian; }
    void setLittleEndian(bool value) { _littleEndian = value; }

    int getBytesAvailable() const;
    int getLength() const { return _length; }

    void Skip(int count) { position += count; }

    char ReadByte();
    unsigned char ReadUbyte();
    bool ReadBool();
    short ReadShort();
    unsigned short ReadUshort();
    int ReadInt();
    unsigned int ReadUint();
    float ReadFloat();
    std::string ReadString();
    std::string ReadString(int len);
    const std::string& ReadS();
    bool ReadS(std::string& result);
    const std::string* ReadSP();
    void WriteS(const std::string& value);
    cocos2d::Color4B ReadColor();
    ByteBuffer* ReadBuffer();
    bool Seek(int indexTablePos, int blockIndex);

    std::vector<std::string>* stringTable;
    int version;
    int position;

private:
    char* _buffer;
    int _offset;
    int _length;
    bool _littleEndian;
    bool _ownsBuffer;
};

NS_FGUI_END

#endif
