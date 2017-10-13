#include "GImage.h"

NS_FGUI_BEGIN
USING_NS_CC;

GImage::GImage() :_content(nullptr)
{
}

GImage::~GImage()
{
}

void GImage::constructFromResource()
{
    sourceWidth = packageItem->width;
    sourceHeight = packageItem->height;
    initWidth = sourceWidth;
    initHeight = sourceHeight;

    _content->initWithSpriteFrame(packageItem->spriteFrame);
    if (packageItem->scale9Grid)
    {
        _content->setRenderingType(ui::Scale9Sprite::RenderingType::SLICE);
        _content->setCapInsets(*packageItem->scale9Grid);
    }
    else
        _content->setRenderingType(ui::Scale9Sprite::RenderingType::SIMPLE);
    _content->setAnchorPoint(Vec2(0, 1));

    setSize(sourceWidth, sourceHeight);
}

bool GImage::init()
{
    _content = ui::Scale9Sprite::create();
    _content->retain();
    _displayObject = _content;

    return true;
}

NS_FGUI_END