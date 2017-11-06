#include "GTextField.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
USING_NS_CC;

void GTextFieldDelegate::setup_BeforeAdd(tinyxml2::XMLElement * xml)
{
    GObject::setup_BeforeAdd(xml);

    const char*p;
    p = xml->Attribute("font");
    if (p)
        setFontName(p);

    p = xml->Attribute("fontSize");
    if (p)
        setFontSize(atoi(p));

    p = xml->Attribute("color");
    if (p)
        setColor((Color3B)ToolSet::convertFromHtmlColor(p));

    p = xml->Attribute("align");
    if (p)
        setAlign(ToolSet::parseAlign(p));

    p = xml->Attribute("vAlign");
    if (p)
        setVerticalAlign(ToolSet::parseVerticalAlign(p));

    p = xml->Attribute("leading");
    if (p)
        setLineSpacing(atoi(p));

    p = xml->Attribute("letterSpacing");
    if (p)
        setLetterSpacing(atoi(p));

    p = xml->Attribute("ubb");
    if (p)
        setUBBEnabled(strcmp(p, "true") == 0);

    p = xml->Attribute("autoSize");
    if (p)
        setAutoSize(ToolSet::parseTextAutoSize(p));

    p = xml->Attribute("underline");
    if (p)
        setUnderline(strcmp(p, "true") == 0);

    p = xml->Attribute("italic");
    if (p)
        setItalic(strcmp(p, "true") == 0);

    p = xml->Attribute("bold");
    if (p)
        setBold(strcmp(p, "true") == 0);

    p = xml->Attribute("singleLine");
    if (p)
        setSingleLine(strcmp(p, "true") == 0);

    p = xml->Attribute("strokeColor");
    if (p)
    {
        Color4B col = ToolSet::convertFromHtmlColor(p);
        p = xml->Attribute("strokeSize");
        enableOutline(col, p ? atoi(p) : 1);
    }

    p = xml->Attribute("shadowColor");
    if (p)
    {
        Color4B col = ToolSet::convertFromHtmlColor(p);

        Vec2 offset;
        p = xml->Attribute("shadowOffset");
        if (p)
            ToolSet::splitString(p, ',', offset);
        offset.y = -offset.y;
        enableShadow(col, (cocos2d::Size)offset);
    }
}

void GTextFieldDelegate::setup_AfterAdd(tinyxml2::XMLElement * xml)
{
    GObject::setup_AfterAdd(xml);

    const char* p;

    p = xml->Attribute("text");
    if (p && strlen(p) > 0)
        setText(p);
}

//---------------------------

GTextField::GTextField() :
    _label(nullptr),
    _autoSize(TextAutoSize::BOTH),
    _updatingSize(false)
{
    _touchDisabled = true;
}

GTextField::~GTextField()
{

}

void GTextField::handleInit()
{
    _label = FUILabel::create();
    _label->retain();

    setFontName(UIConfig::defaultFont);
    setFontSize(12);
    setColor(Color3B::BLACK);

    _displayObject = _label;
}

void GTextField::setText(const std::string & text)
{
    _label->setString(text);
    if (!_underConstruct)
        updateSize();
}

void GTextField::setFontName(const std::string & value)
{
    _label->setFontName(value);
    if (!_underConstruct)
        updateSize();
}

void GTextField::setFontSize(int value)
{
    _label->setFontSize(value);
    if (!_underConstruct)
        updateSize();
}

void GTextField::setColor(const Color3B & value)
{
    _color = value;
    _label->setTextColor((Color4B)value);
    updateGear(4);
}

void GTextField::setAutoSize(TextAutoSize value)
{
    _autoSize = value;
    switch (value)
    {
    case TextAutoSize::NONE:
        _label->setOverflow(Label::Overflow::CLAMP);
        break;
    case TextAutoSize::BOTH:
        _label->setOverflow(Label::Overflow::NONE);
        break;
    case TextAutoSize::HEIGHT:
        _label->setOverflow(Label::Overflow::RESIZE_HEIGHT);
        break;
    case TextAutoSize::SHRINK:
        _label->setOverflow(Label::Overflow::SHRINK);
        break;
    }

    if (_autoSize == TextAutoSize::BOTH)
        _label->setDimensions(0, 0);
    else if (_autoSize == TextAutoSize::HEIGHT)
        _label->setDimensions(_size.width, 0);
    else
        _label->setDimensions(_size.width, _size.height);

    if (!_underConstruct)
        updateSize();
}

void GTextField::setAlign(cocos2d::TextHAlignment value)
{
    _label->setAlignment(value);
}

void GTextField::setVerticalAlign(cocos2d::TextVAlignment value)
{
    _label->setVerticalAlignment(value);
}

void GTextField::setUnderline(bool value)
{
    if (value)
        _label->enableUnderline();
    else
        _label->disableEffect(LabelEffect::UNDERLINE);
}

void GTextField::setItalic(bool value)
{
    if (value)
        _label->enableItalics();
    else
        _label->disableEffect(LabelEffect::ITALICS);
}

void GTextField::setBold(bool value)
{
    if (value)
        ;// _label->enableBold();
    else
        _label->disableEffect(LabelEffect::BOLD);
}

void GTextField::setSingleLine(bool value)
{
    _label->enableWrap(!value);
}

void GTextField::setLetterSpacing(float value)
{
}

void GTextField::setLineSpacing(float value)
{
    _label->setLineSpacing(value);
}

void GTextField::enableOutline(const cocos2d::Color4B & color, float size)
{
    if (size == 0)
    {
        _label->disableEffect(LabelEffect::OUTLINE);
    }
    else
    {
        _outlineColor = color;
        _label->enableOutline(_outlineColor, size);
        updateGear(4);
    }
}

void GTextField::enableShadow(const cocos2d::Color4B& color, const cocos2d::Size & offset)
{
    if (offset.width != 0 || offset.height != 0)
        _label->enableShadow(color, offset);
    else
        _label->disableEffect(LabelEffect::SHADOW);
}

void GTextField::cg_setOutlineColor(const cocos2d::Color4B & value)
{
    enableOutline(value, _label->getOutlineSize());
}

void GTextField::updateSize()
{
    if (_updatingSize)
        return;

    _updatingSize = true;

    Size sz = _label->getContentSize();
    if (_autoSize == TextAutoSize::BOTH)
        setSize(sz.width, sz.height);
    else if (_autoSize == TextAutoSize::HEIGHT)
        setHeight(sz.height);

    _updatingSize = false;
}

void GTextField::handleSizeChanged()
{
    if (_updatingSize)
        return;

    if (_autoSize != TextAutoSize::BOTH)
        _label->setDimensions(_size.width, _size.height);

    if (_autoSize == TextAutoSize::HEIGHT)
    {
        if (_label->getString().length() > 0) //文本为空时，不要改掉文本为空时的默认高度，造成关联错误
            setSizeDirectly(_size.width, _label->getContentSize().height);
    }
}

void GTextField::setup_AfterAdd(tinyxml2::XMLElement* xml)
{
    GTextFieldDelegate::setup_AfterAdd(xml);

    updateSize();
}

NS_FGUI_END
