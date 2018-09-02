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
    _shrinkOnly(false),
    _updatingLayout(false),
    _contentItem(nullptr),
    _contentStatus(0),
    _content(nullptr),
    _content2(nullptr),
    _playAction(nullptr),
    _playing(true),
    _frame(0)
{
}

GLoader::~GLoader()
{
    CC_SAFE_RELEASE(_playAction);
    CC_SAFE_RELEASE(_content);
    CC_SAFE_RELEASE(_content2);
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

void GLoader::setAutoSize(bool value)
{
    if (_autoSize != value)
    {
        _autoSize = value;
        updateLayout();
    }
}

void GLoader::setFill(LoaderFillType value)
{
    if (_fill != value)
    {
        _fill = value;
        updateLayout();
    }
}

void GLoader::setShrinkOnly(bool value)
{
    if (_shrinkOnly != value)
    {
        _shrinkOnly = value;
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
        updateGear(5);
    }
}

int GLoader::getFrame() const
{
    return _playAction->getFrame();
}

void GLoader::setFrame(int value)
{
    if (_frame != value)
    {
        _frame = value;
        if (_playAction)
            _playAction->setFrame(value);
        updateGear(5);
    }
}

float GLoader::getTimeScale() const
{
    if (_playAction)
        return _playAction->getTimeScale();
    else
        return 1;
}

void GLoader::setTimeScale(float value)
{
    if (_playAction)
        _playAction->setTimeScale(value);
}

void GLoader::advance(float time)
{
    if (_playAction)
        _playAction->advance(time);
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
        _contentSourceSize.width = _contentItem->width;
        _contentSourceSize.height = _contentItem->height;

        if (_contentItem->type == PackageItemType::IMAGE)
        {
            _contentStatus = 1;
            _content->initWithSpriteFrame(_contentItem->spriteFrame);
            if (_contentItem->scale9Grid)
                ((FUISprite*)_content)->setScale9Grid(_contentItem->scale9Grid);
            updateLayout();
        }
        else if (_contentItem->type == PackageItemType::MOVIECLIP)
        {
            _contentStatus = 2;
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
                _playAction->setFrame(_frame);

            updateLayout();
        }
        else if (_contentItem->type == PackageItemType::COMPONENT)
        {
            GObject* obj = UIPackage::createObjectFromURL(_url);
            if (obj == nullptr)
                setErrorState();
            else if (dynamic_cast<GComponent*>(obj) == nullptr)
            {
                setErrorState();
            }
            else
            {
                _content2 = obj->as<GComponent>();
                _content2->retain();
                _content2->addEventListener(UIEventType::SizeChange, [this](EventContext*) {
                    if (!_updatingLayout)
                        updateLayout();
                });
                _displayObject->addChild(_content2->displayObject());
                updateLayout();
            }
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
    auto tex = Director::getInstance()->getTextureCache()->addImage(_url);
    if (tex)
    {
        auto sf = SpriteFrame::createWithTexture(tex, Rect(Vec2::ZERO, tex->getContentSize()));
        onExternalLoadSuccess(sf);
    }
    else
        onExternalLoadFailed();
}

void GLoader::freeExternal(cocos2d::SpriteFrame* spriteFrame)
{
}

void GLoader::onExternalLoadSuccess(cocos2d::SpriteFrame* spriteFrame)
{
    _contentStatus = 4;
    _content->setSpriteFrame(spriteFrame);
    Size sz = spriteFrame->getRectInPixels().size;
    _contentSourceSize.width = sz.width;
    _contentSourceSize.height = sz.height;
    updateLayout();
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

    if (_content2 != nullptr)
    {
        _displayObject->removeChild(_content2->displayObject());
        CC_SAFE_RELEASE(_content2);
    }
    ((FUISprite*)_content)->clearContent();

    _contentItem = nullptr;
    _contentStatus = 0;
}

void GLoader::updateLayout()
{
    if (_content2 == nullptr && _contentStatus == 0)
    {
        if (_autoSize)
        {
            _updatingLayout = true;
            setSize(50, 30);
            _updatingLayout = false;
        }
        return;
    }

    _contentSize = _contentSourceSize;

    if (_autoSize)
    {
        _updatingLayout = true;
        if (_contentSize.width == 0)
            _contentSize.width = 50;
        if (_contentSize.height == 0)
            _contentSize.height = 30;
        setSize(_contentSize.width, _contentSize.height);
        _updatingLayout = false;

        if (_size.equals(_contentSize))
        {
            if (_content2 != nullptr)
            {
                _content2->setScale(1, 1);
                _content2->setPosition(0, -_size.height);
            }
            else
            {
                _content->setScale(1, 1);
                _content->setAnchorPoint(Vec2::ZERO);
                _content->setPosition(0, 0);
            }
            return;
        }
    }

    float sx = 1, sy = 1;
    if (_fill != LoaderFillType::NONE)
    {
        sx = _size.width / _contentSourceSize.width;
        sy = _size.height / _contentSourceSize.height;

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
            else if (_fill == LoaderFillType::SCALE_NO_BORDER)
            {
                if (sx > sy)
                    sy = sx;
                else
                    sx = sy;
            }

            if (_shrinkOnly)
            {
                if (sx > 1)
                    sx = 1;
                if (sy > 1)
                    sy = 1;
            }
            _contentSize.width = floor(_contentSourceSize.width * sx);
            _contentSize.height = floor(_contentSourceSize.height * sy);
        }
    }

    if (_content2 != nullptr)
    {
        _content2->setScale(sx, sy);
    }
    else
    {
        _content->setContentSize(_contentSourceSize);
        _content->setScale(sx, sy);
        _content->setAnchorPoint(Vec2::ZERO);
    }

    float nx;
    float ny;
    if (_align == TextHAlignment::CENTER)
        nx = floor((_size.width - _contentSize.width) / 2);
    else if (_align == TextHAlignment::RIGHT)
        nx = floor(_size.width - _contentSize.width);
    else
        nx = 0;

    if (_content2 != nullptr)
    {
        if (_verticalAlign == TextVAlignment::CENTER)
            ny = floor(-_contentSize.height - (_size.height - _contentSize.height) / 2);
        else if (_verticalAlign == TextVAlignment::BOTTOM)
            ny = -_contentSize.height;
        else
            ny = -_size.height;

        _content2->setPosition(nx, ny);
    }
    else
    {
        if (_verticalAlign == TextVAlignment::CENTER)
            ny = floor((_size.height - _contentSize.height) / 2);
        else if (_verticalAlign == TextVAlignment::BOTTOM)
            ny = 0;
        else
            ny = _size.height - _contentSize.height;

        _content->setPosition(nx, ny);
    }
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

void GLoader::handleGrayedChanged()
{
    GObject::handleGrayedChanged();

    ((FUISprite*)_content)->setGrayed(_finalGrayed);
    if (_content2 != nullptr)
        _content2->setGrayed(_finalGrayed);
}

void GLoader::setup_BeforeAdd(TXMLElement * xml)
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

    _shrinkOnly = xml->BoolAttribute("shrinkOnly");
    _autoSize = xml->BoolAttribute("autoSize");

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