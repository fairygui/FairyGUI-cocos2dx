#include "GLoader.h"
#include "UIPackage.h"
#include "utils/ToolSet.h"
#include "display/FUISprite.h"

NS_FGUI_BEGIN
USING_NS_CC;

GLoader::GLoader() :
    _autoSize(false),
    _align(TextHAlignment::LEFT),
    _verticalAlign(TextVAlignment::TOP),
    _fill(LoaderFillType::NONE),
    _updatingLayout(false),
    _contentItem(nullptr),
    _contentWidth(0),
    _contentHeight(0),
    _contentSourceWidth(0),
    _contentSourceHeight(0),
    _contentStatus(0),
    _content(nullptr),
    _playAction(nullptr),
    _playing(true),
    _frame(0)
{
}

GLoader::~GLoader()
{
    CC_SAFE_RELEASE(_playAction);
    CC_SAFE_RELEASE(_content);
}

void GLoader::handleInit()
{
    _content = FUISprite::create();
    _content->retain();
    _content->setAnchorPoint(Vec2::ZERO);
    _content->setCascadeOpacityEnabled(true);

    _displayObject = Node::create();
    _displayObject->retain();
    _displayObject->addChild(_content);
}

void GLoader::setURL(const std::string & value)
{
    if (_url.compare(value) == 0)
        return;

    _url = value;
    loadContent();
    updateGear(7);
}

void GLoader::setAlign(cocos2d::TextHAlignment value)
{
    if (_align != value)
    {
        _align = value;
        updateLayout();
    }
}

void GLoader::setVerticalAlign(cocos2d::TextVAlignment value)
{
    if (_verticalAlign != value)
    {
        _verticalAlign = value;
        updateLayout();
    }
}

void GLoader::setColor(const cocos2d::Color3B & value)
{
    _content->setColor(value);
}

void GLoader::setPlaying(bool value)
{
    if (_playing != value)
    {
        _playing = value;
        if (_playAction)
        {
            if (_playing)
                _content->runAction(_playAction);
            else
                _content->stopAction(_playAction);
        }
    }
}

void GLoader::setCurrentFrame(int value)
{
    _frame = value;
    if (_playAction)
        _playAction->setCurrentFrame(value);
}

cocos2d::Color4B GLoader::cg_getColor() const
{
    return (Color4B)_content->getColor();
}

void GLoader::cg_setColor(const cocos2d::Color4B& value)
{
    _content->setColor((Color3B)value);
}

void GLoader::loadContent()
{
    clearContent();

    if (_url.length() == 0)
        return;

    if (_url.compare(0, 5, "ui://") == 0)
        loadFromPackage();
    else
    {
        _contentStatus = 3;
        loadExternal();
    }
}

void GLoader::loadFromPackage()
{
    _contentItem = UIPackage::getItemByURL(_url);

    if (_contentItem != nullptr)
    {
        _contentItem->load();

        if (_contentItem->type == PackageItemType::IMAGE)
        {
            _contentStatus = 1;
            _contentSourceWidth = _contentItem->width;
            _contentSourceHeight = _contentItem->height;
            _content->initWithSpriteFrame(_contentItem->spriteFrame);
            if (_contentItem->scale9Grid)
                ((FUISprite*)_content)->setScale9Grid(_contentItem->scale9Grid);
            updateLayout();
        }
        else if (_contentItem->type == PackageItemType::MOVIECLIP)
        {
            _contentStatus = 2;
            _contentSourceWidth = _contentItem->width;
            _contentSourceHeight = _contentItem->height;

            if (_playAction == nullptr)
            {
                _playAction = ActionMovieClip::create(_contentItem->animation, _contentItem->repeatDelay);
                _playAction->retain();
            }
            else
                _playAction->setAnimation(_contentItem->animation, _contentItem->repeatDelay);
            if (_playing)
                _content->runAction(_playAction);
            else
                _playAction->setCurrentFrame(_frame);

            updateLayout();
        }
        else
        {
            if (_autoSize)
                setSize(_contentItem->width, _contentItem->height);

            setErrorState();
        }
    }
    else
        setErrorState();
}

void GLoader::loadExternal()
{

}

void GLoader::freeExternal(cocos2d::SpriteFrame* spriteFrame)
{
    spriteFrame->release();
}

void GLoader::onExternalLoadSuccess(cocos2d::SpriteFrame* spriteFrame)
{
    _contentStatus = 4;
    spriteFrame->retain();
    _content->setSpriteFrame(spriteFrame);
}

void GLoader::onExternalLoadFailed()
{
    setErrorState();
}

void GLoader::clearContent()
{
    clearErrorState();

    if (_contentStatus == 4)
        freeExternal(_content->getSpriteFrame());

    if (_contentStatus == 2)
    {
        _playAction->setAnimation(nullptr);
        _content->stopAction(_playAction);
    }

    ((FUISprite*)_content)->clearContent();
    _contentItem = nullptr;

    _contentStatus = 0;
}

void GLoader::updateLayout()
{
    if (_contentStatus == 0)
    {
        if (_autoSize)
        {
            _updatingLayout = true;
            setSize(50, 30);
            _updatingLayout = false;
        }
        return;
    }

    _contentWidth = _contentSourceWidth;
    _contentHeight = _contentSourceHeight;

    if (_autoSize)
    {
        _updatingLayout = true;
        if (_contentWidth == 0)
            _contentWidth = 50;
        if (_contentHeight == 0)
            _contentHeight = 30;
        setSize(_contentWidth, _contentHeight);
        _updatingLayout = false;

        if (_size.width == _contentWidth && _size.height == _contentHeight)
        {
            _content->setScale(1, 1);
            //if (_content->getTexture() != nullptr)
            //	_content.SetNativeSize();
            return;
        }
        //如果不相等，可能是由于大小限制造成的，要后续处理
    }

    float sx = 1, sy = 1;
    if (_fill != LoaderFillType::NONE)
    {
        sx = _size.width / _contentSourceWidth;
        sy = _size.height / _contentSourceHeight;

        if (sx != 1 || sy != 1)
        {
            if (_fill == LoaderFillType::SCALE_MATCH_HEIGHT)
                sx = sy;
            else if (_fill == LoaderFillType::SCALE_MATCH_WIDTH)
                sy = sx;
            else if (_fill == LoaderFillType::SCALE)
            {
                if (sx > sy)
                    sx = sy;
                else
                    sy = sx;
            }
            _contentWidth = floor(_contentSourceWidth * sx);
            _contentHeight = floor(_contentSourceHeight * sy);
        }
    }

    _content->setContentSize(Size(_contentSourceWidth, _contentSourceHeight));
    _content->setScale(sx, sy);
    _content->setAnchorPoint(Vec2::ZERO);

    float nx;
    float ny;
    if (_align == TextHAlignment::CENTER)
        nx = floor((_size.width - _contentWidth) / 2);
    else if (_align == TextHAlignment::RIGHT)
        nx = floor(_size.width - _contentWidth);
    else
        nx = 0;
    if (_verticalAlign == TextVAlignment::CENTER)
        ny = floor((_size.height - _contentHeight) / 2);
    else if (_verticalAlign == TextVAlignment::BOTTOM)
        ny = 0;
    else
        ny = _size.height - _contentHeight;

    _content->setPosition(nx, ny);
}

void GLoader::setErrorState()
{
}

void GLoader::clearErrorState()
{
}

void GLoader::handleSizeChanged()
{
    GObject::handleSizeChanged();

    if (!_updatingLayout)
        updateLayout();
}

void GLoader::setup_BeforeAdd(tinyxml2::XMLElement * xml)
{
    GObject::setup_BeforeAdd(xml);

    const char*p;

    p = xml->Attribute("url");
    if (p)
        _url = p;

    p = xml->Attribute("align");
    if (p)
        _align = ToolSet::parseAlign(p);

    p = xml->Attribute("vAlign");
    if (p)
        _verticalAlign = ToolSet::parseVerticalAlign(p);

    p = xml->Attribute("fill");
    if (p)
        _fill = ToolSet::parseFillType(p);

    p = xml->Attribute("color");
    if (p)
        setColor((Color3B)ToolSet::convertFromHtmlColor(p));

    p = xml->Attribute("frame");
    if (p)
        _frame = atoi(p);

    p = xml->Attribute("playing");
    if (p)
        _playing = strcmp(p, "false") != 0;

    if (_url.length() > 0)
        loadContent();
}

NS_FGUI_END