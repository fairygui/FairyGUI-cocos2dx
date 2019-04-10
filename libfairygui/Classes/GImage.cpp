#include "GImage.h"
#include "PackageItem.h"
#include "display/FUISprite.h"
#include "utils/ByteBuffer.h"

NS_FGUI_BEGIN
USING_NS_CC;

GImage::GImage() :
    _content(nullptr)
{
    _touchDisabled = true;
}

GImage::~GImage()
{
}

void GImage::handleInit()
{
    _content = FUISprite::create();
    _content->retain();

    _displayObject = _content;
}

FlipType GImage::getFlip() const
{
    if (_content->isFlippedX() && _content->isFlippedY())
        return FlipType::BOTH;
    else if (_content->isFlippedX())
        return FlipType::HORIZONTAL;
    else if (_content->isFlippedY())
        return FlipType::VERTICAL;
    else
        return FlipType::NONE;
}

void GImage::setFlip(FlipType value)
{
    _content->setFlippedX(value == FlipType::HORIZONTAL || value == FlipType::BOTH);
    _content->setFlippedY(value == FlipType::VERTICAL || value == FlipType::BOTH);
}

void GImage::handleSizeChanged()
{
    if (_packageItem->scaleByTile)
        _content->setTextureRect(Rect(Vec2::ZERO, _size));
    else
        _content->setContentSize(_size);
}

void GImage::handleGrayedChanged()
{
    GObject::handleGrayedChanged();

    ((FUISprite*)_content)->setGrayed(_finalGrayed);
}

cocos2d::Color3B GImage::getColor() const
{
    return _content->getColor();
}

void GImage::setColor(const cocos2d::Color3B & value)
{
    _content->setColor(value);
}

FillMethod GImage::getFillMethod() const
{
    return _content->getFillMethod();
}

void GImage::setFillMethod(FillMethod value)
{
    _content->setFillMethod(value);
}

FillOrigin GImage::getFillOrigin() const
{
    return _content->getFillOrigin();
}

void GImage::setFillOrigin(FillOrigin value)
{
    _content->setFillOrigin(value);
}

bool GImage::isFillClockwise() const
{
    return _content->isFillClockwise();
}

void GImage::setFillClockwise(bool value)
{
    _content->setFillClockwise(value);
}

float GImage::getFillAmount() const
{
    return _content->getFillAmount();
}

void GImage::setFillAmount(float value)
{
    _content->setFillAmount(value);
}

void GImage::constructFromResource()
{
    _packageItem->load();

    sourceSize.width = _packageItem->width;
    sourceSize.height = _packageItem->height;
    initSize = sourceSize;

    _content->setSpriteFrame(_packageItem->spriteFrame);
    if (_packageItem->scale9Grid)
        ((FUISprite*)_content)->setScale9Grid(_packageItem->scale9Grid);

    setSize(sourceSize.width, sourceSize.height);
}

void GImage::setup_beforeAdd(ByteBuffer* buffer, int beginPos)
{
    GObject::setup_beforeAdd(buffer, beginPos);

    buffer->Seek(beginPos, 5);

    if (buffer->ReadBool())
        setColor((Color3B)buffer->ReadColor());
    setFlip((FlipType)buffer->ReadByte());
    int fillMethod = buffer->ReadByte();
    if (fillMethod != 0)
    {
        _content->setFillMethod((FillMethod)fillMethod);
        _content->setFillOrigin((FillOrigin)buffer->ReadByte());
        _content->setFillClockwise(buffer->ReadBool());
        _content->setFillAmount(buffer->ReadFloat());
    }
}

NS_FGUI_END