#include "GLoader.h"
#include "UIPackage.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
USING_NS_CC;

GLoader::GLoader() :
    _autoSize(false),
    _align(TextHAlignment::LEFT),
    _verticalAlign(TextVAlignment::TOP),
    _fill(FillType::FT_NONE),
    _updatingLayout(false),
    _contentItem(nullptr),
    _contentWidth(0),
    _contentHeight(0),
    _contentSourceWidth(0),
    _contentSourceHeight(0),
    _contentStatus(0),
    _content(nullptr),
    _playAction(nullptr)
{
}

GLoader::~GLoader()
{
    CC_SAFE_RELEASE(_playAction);
    CC_SAFE_RELEASE(_content);
}

void GLoader::setURL(const std::string & value)
{
    if (_url.compare(value) == 0)
        return;

    _url = value;
    loadContent();
    updateGear(7);
}

void GLoader::loadContent()
{
    clearContent();

    if (_url.length() == 0)
        return;

    if (_url.compare(0, 5, "ui://") == 0)
        loadFromPackage();
    else
        loadExternal();
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
            _content->initWithSpriteFrame(_contentItem->spriteFrame);
            _content->setAnchorPoint(Vec2(0, 1));
            _contentSourceWidth = _contentItem->width;
            _contentSourceHeight = _contentItem->height;
            updateLayout();
        }
        else if (_contentItem->type == PackageItemType::MOVIECLIP)
        {
            _contentStatus = 2;
            _contentSourceWidth = _contentItem->width;
            _contentSourceHeight = _contentItem->height;

            _playAction = RepeatForever::create(Animate::create(_contentItem->animation));
            _playAction->retain();
            _content->runAction(_playAction);
            _content->setAnchorPoint(Vec2(0, 1));

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

void GLoader::freeExternal(cocos2d::Texture2D* texture)
{

}

void GLoader::clearContent()
{
    clearErrorState();
    if (_contentItem == nullptr && _contentStatus != 0)
        freeExternal(_content->getTexture());

    _contentStatus = 0;

    CC_SAFE_RELEASE_NULL(_playAction);

    _content->initWithTexture(nullptr, Rect::ZERO);
    _contentItem = nullptr;
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

        if (getWidth() == _contentWidth && getHeight() == _contentHeight)
        {
            _content->setScale(1, 1);
            //if (_content->getTexture() != nullptr)
            //	_content.SetNativeSize();
            return;
        }
        //如果不相等，可能是由于大小限制造成的，要后续处理
    }

    float sx = 1, sy = 1;
    if (_fill != FillType::FT_NONE)
    {
        sx = getWidth() / _contentSourceWidth;
        sy = getHeight() / _contentSourceHeight;

        if (sx != 1 || sy != 1)
        {
            if (_fill == FillType::FT_SCALE_MATCH_HEIGHT)
                sx = sy;
            else if (_fill == FillType::FT_SCALE_MATCH_WIDTH)
                sy = sx;
            else if (_fill == FillType::FT_SCALE)
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

    _content->setScale(sx, sy);

    float nx;
    float ny;
    if (_align == TextHAlignment::CENTER)
        nx = floor((getWidth() - _contentWidth) / 2);
    else if (_align == TextHAlignment::RIGHT)
        nx = floor(getWidth() - _contentWidth);
    else
        nx = 0;
    if (_verticalAlign == TextVAlignment::CENTER)
        ny = floor((getHeight() - _contentHeight) / 2);
    else if (_verticalAlign == TextVAlignment::BOTTOM)
        ny = floor(getHeight() - _contentHeight);
    else
        ny = 0;

    _content->setPosition(nx, getHeight() - ny);
}

void GLoader::setErrorState()
{
}

void GLoader::clearErrorState()
{
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

    if (_url.length() > 0)
        loadContent();
}

bool GLoader::init()
{
    _content = Sprite::create();
    _content->retain();

    _displayObject = Node::create();
    _displayObject->retain();
    _displayObject->setAnchorPoint(Vec2(0, 1));
    _displayObject->addChild(_content);

    return true;
}

void GLoader::handleSizeChanged()
{
    GObject::handleSizeChanged();

    if (!_updatingLayout)
        updateLayout();
}

NS_FGUI_END