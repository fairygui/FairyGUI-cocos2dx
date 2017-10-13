#include "GMovieClip.h"

NS_FGUI_BEGIN
USING_NS_CC;

GMovieClip::GMovieClip() :_playAction(nullptr), _content(nullptr)
{
    _sizeImplType = 1;
}

GMovieClip::~GMovieClip()
{
    CC_SAFE_RELEASE(_playAction);
}

void GMovieClip::constructFromResource()
{
    sourceWidth = packageItem->width;
    sourceHeight = packageItem->height;
    initWidth = sourceWidth;
    initHeight = sourceHeight;

    _content->setAnchorPoint(Vec2(0, 1));
    _playAction = RepeatForever::create(Animate::create(packageItem->animation));
    _playAction->retain();
    _content->runAction(_playAction);

    setSize(sourceWidth, sourceHeight);
}

bool GMovieClip::init()
{
    _content = Sprite::create();
    _content->retain();
    _displayObject = _content;

    return true;
}

NS_FGUI_END