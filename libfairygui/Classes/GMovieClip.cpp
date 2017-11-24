#include "GMovieClip.h"
#include "utils/ToolSet.h"
#include "display/FUISprite.h"

NS_FGUI_BEGIN
USING_NS_CC;

GMovieClip::GMovieClip() :
    _playAction(nullptr),
    _content(nullptr),
    _playing(true)
{
    _sizeImplType = 1;
    _touchDisabled = true;
}

GMovieClip::~GMovieClip()
{
    CC_SAFE_RELEASE(_playAction);
}

void GMovieClip::handleInit()
{
    _content = FUISprite::create();
    _content->retain();

    _playAction = ActionMovieClip::create(nullptr);
    _playAction->retain();

    _displayObject = _content;
}

void GMovieClip::setPlaySettings(int start, int end, int times, int endAt, std::function<void()> completeCallback)
{
    _playAction->setPlaySettings(start, end, times, endAt, completeCallback);
}

void GMovieClip::setPlaying(bool value)
{
    if (_playing != value)
    {
        _playing = value;
        if (_playing)
            _content->runAction(_playAction);
        else
            _content->stopAction(_playAction);
    }
}

void GMovieClip::setCurrentFrame(int value)
{
    _playAction->setCurrentFrame(value);
}

FlipType GMovieClip::getFlip() const
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

void GMovieClip::setFlip(FlipType value)
{
    _content->setFlippedX(value == FlipType::HORIZONTAL || value == FlipType::BOTH);
    _content->setFlippedY(value == FlipType::VERTICAL || value == FlipType::BOTH);
}

void GMovieClip::setColor(const cocos2d::Color3B & value)
{
    _content->setColor(value);
}

cocos2d::Color4B GMovieClip::cg_getColor() const
{
    return (Color4B)_content->getColor();
}

void GMovieClip::cg_setColor(const cocos2d::Color4B& value)
{
    _content->setColor((Color3B)value);
}

void GMovieClip::handleGrayedChanged()
{
    GObject::handleGrayedChanged();

    ((FUISprite*)_content)->setGrayed(_finalGrayed);
}

void GMovieClip::constructFromResource()
{
    sourceSize.width = _packageItem->width;
    sourceSize.height = _packageItem->height;
    initSize = sourceSize;

    _playAction->setAnimation(_packageItem->animation, _packageItem->repeatDelay);
    _content->runAction(_playAction);

    setSize(sourceSize.width, sourceSize.height);
}

void GMovieClip::setup_BeforeAdd(TXMLElement * xml)
{
    GObject::setup_BeforeAdd(xml);

    const char *p;

    p = xml->Attribute("frame");
    if (p)
        setCurrentFrame(atoi(p));

    p = xml->Attribute("playing");
    if (p)
        setPlaying(strcmp(p, "false") != 0);

    p = xml->Attribute("flip");
    if (p)
        setFlip(ToolSet::parseFlipType(p));

    p = xml->Attribute("color");
    if (p)
        setColor((Color3B)ToolSet::convertFromHtmlColor(p));
}

NS_FGUI_END