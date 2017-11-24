#include "GImage.h"
#include "display/FUISprite.h"
#include "utils/ToolSet.h"

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

void GImage::setColor(const cocos2d::Color3B & value)
{
    _content->setColor(value);
}

cocos2d::Color4B GImage::cg_getColor() const
{
    return (Color4B)_content->getColor();
}

void GImage::cg_setColor(const cocos2d::Color4B& value)
{
    _content->setColor((Color3B)value);
}

void GImage::constructFromResource()
{
    sourceSize.width = _packageItem->width;
    sourceSize.height = _packageItem->height;
    initSize = sourceSize;

    _content->setSpriteFrame(_packageItem->spriteFrame);
    if (_packageItem->scale9Grid)
        ((FUISprite*)_content)->setScale9Grid(_packageItem->scale9Grid);

    setSize(sourceSize.width, sourceSize.height);
}

void GImage::setup_BeforeAdd(TXMLElement * xml)
{
    GObject::setup_BeforeAdd(xml);

    const char *p;

    p = xml->Attribute("flip");
    if (p)
        setFlip(ToolSet::parseFlipType(p));

    p = xml->Attribute("color");
    if (p)
        setColor((Color3B)ToolSet::convertFromHtmlColor(p));
}

NS_FGUI_END