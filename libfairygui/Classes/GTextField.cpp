#include "GTextField.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
USING_NS_CC;

void GTextField::setup_BeforeAdd(TXMLElement * xml)
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

void GTextField::setup_AfterAdd(TXMLElement * xml)
{
    GObject::setup_AfterAdd(xml);

    const char* p;

    p = xml->Attribute("text");
    if (p && strlen(p) > 0)
        setText(p);
}

//---------------------------

GBasicTextField::GBasicTextField() :
    _label(nullptr),
    _autoSize(TextAutoSize::BOTH),
    _updatingSize(false)
{
    _touchDisabled = true;
}

GBasicTextField::~GBasicTextField()
{

}

void GBasicTextField::handleInit()
{
    _label = FUILabel::create();
    _label->retain();

    setFontName(UIConfig::defaultFont);
    setFontSize(12);
    setColor(Color3B::BLACK);

    _displayObject = _label;
}

void GBasicTextField::setText(const std::string & text)
{
    _label->setString(text);
    if (!_underConstruct)
        updateSize();
}

void GBasicTextField::setFontName(const std::string & value)
{
    _label->setFontName(value);
    if (!_underConstruct)
        updateSize();
}

void GBasicTextField::setFontSize(int value)
{
    _label->setFontSize(value);
    if (!_underConstruct)
        updateSize();
}

void GBasicTextField::setColor(const Color3B & value)
{
    _color = value;
    _label->setTextColor((Color4B)value);
    updateGear(4);
}

void GBasicTextField::setAutoSize(TextAutoSize value)
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

void GBasicTextField::setAlign(cocos2d::TextHAlignment value)
{
    _label->setAlignment(value);
}

void GBasicTextField::setVerticalAlign(cocos2d::TextVAlignment value)
{
    _label->setVerticalAlignment(value);
}

void GBasicTextField::setUnderline(bool value)
{
    if (value)
        _label->enableUnderline();
    else
        _label->disableEffect(LabelEffect::UNDERLINE);
}

void GBasicTextField::setItalic(bool value)
{
    if (value)
        _label->enableItalics();
    else
        _label->disableEffect(LabelEffect::ITALICS);
}

void GBasicTextField::setBold(bool value)
{
    if (value)
        ;// _label->enableBold();
    else
        _label->disableEffect(LabelEffect::BOLD);
}

void GBasicTextField::setSingleLine(bool value)
{
    _label->enableWrap(!value);
}

void GBasicTextField::setLetterSpacing(float value)
{
}

void GBasicTextField::setLineSpacing(float value)
{
    _label->setLineSpacing(value);
}

void GBasicTextField::enableOutline(const cocos2d::Color4B & color, float size)
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

void GBasicTextField::enableShadow(const cocos2d::Color4B& color, const cocos2d::Size & offset)
{
    if (offset.width != 0 || offset.height != 0)
        _label->enableShadow(color, offset);
    else
        _label->disableEffect(LabelEffect::SHADOW);
}

void GBasicTextField::cg_setOutlineColor(const cocos2d::Color4B & value)
{
    enableOutline(value, _label->getOutlineSize());
}

void GBasicTextField::updateSize()
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

void GBasicTextField::handleSizeChanged()
{
    if (_updatingSize)
        return;

    if (_autoSize != TextAutoSize::BOTH)
        _label->setDimensions(_size.width, _size.height);

    if (_autoSize == TextAutoSize::HEIGHT)
    {
        if (_label->getString().length() > 0)
            setSizeDirectly(_size.width, _label->getContentSize().height);
    }
}

void GBasicTextField::setup_AfterAdd(TXMLElement* xml)
{
    GTextField::setup_AfterAdd(xml);

    updateSize();
}

NS_FGUI_END
