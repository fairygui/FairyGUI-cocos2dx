#include "GTextField.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
USING_NS_CC;

void GTextField::setColor(const cocos2d::Color3B & value)
{
    getTextFormat()->color = value;
    applyTextFormat();
}

void GTextField::setFontSize(float value)
{
    getTextFormat()->fontSize = value;
    applyTextFormat();
}

void GTextField::cg_setColor(const cocos2d::Color4B & value)
{
    getTextFormat()->color = (Color3B)value;
    applyTextFormat();
}

void GTextField::cg_setOutlineColor(const cocos2d::Color4B & value)
{
    getTextFormat()->outlineColor = (Color3B)value;
    applyTextFormat();
}

void GTextField::setup_BeforeAdd(TXMLElement * xml)
{
    GObject::setup_BeforeAdd(xml);

    TextFormat* tf = getTextFormat();
    const char*p;
    p = xml->Attribute("font");
    if (p)
        tf->face = p;

    p = xml->Attribute("fontSize");
    if (p)
        tf->fontSize = atoi(p);

    p = xml->Attribute("color");
    if (p)
        tf->color = (Color3B)ToolSet::convertFromHtmlColor(p);

    p = xml->Attribute("align");
    if (p)
        tf->align = ToolSet::parseAlign(p);

    p = xml->Attribute("vAlign");
    if (p)
        tf->verticalAlign = ToolSet::parseVerticalAlign(p);

    p = xml->Attribute("leading");
    if (p)
        tf->lineSpacing = atoi(p);

    p = xml->Attribute("letterSpacing");
    if (p)
        tf->letterSpacing = atoi(p);

    p = xml->Attribute("ubb");
    if (p)
        setUBBEnabled(strcmp(p, "true") == 0);

    p = xml->Attribute("autoSize");
    if (p)
        setAutoSize(ToolSet::parseTextAutoSize(p));

    p = xml->Attribute("underline");
    if (p)
        tf->underline = strcmp(p, "true") == 0;

    p = xml->Attribute("italic");
    if (p)
        tf->italics = strcmp(p, "true") == 0;

    p = xml->Attribute("bold");
    if (p)
        tf->bold = strcmp(p, "true") == 0;

    p = xml->Attribute("singleLine");
    if (p)
        setSingleLine(strcmp(p, "true") == 0);

    p = xml->Attribute("strokeColor");
    if (p)
    {
        tf->outlineColor = (Color3B)ToolSet::convertFromHtmlColor(p);
        p = xml->Attribute("strokeSize");
        tf->outlineSize = p ? atoi(p) : 1;
        tf->enableEffect(TextFormat::OUTLINE);
    }

    p = xml->Attribute("shadowColor");
    if (p)
    {
        tf->shadowColor = (Color3B)ToolSet::convertFromHtmlColor(p);

        Vec2 offset;
        p = xml->Attribute("shadowOffset");
        if (p)
            ToolSet::splitString(p, ',', offset);
        offset.y = -offset.y;
        tf->shadowOffset = offset;
        tf->enableEffect(TextFormat::SHADOW);
    }
}

void GTextField::setup_AfterAdd(TXMLElement * xml)
{
    GObject::setup_AfterAdd(xml);

    applyTextFormat();

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

    _displayObject = _label;
}

void GBasicTextField::setText(const std::string & text)
{
    _label->setText(text);
    if (!_underConstruct)
        updateSize();
}

void GBasicTextField::applyTextFormat()
{
    _label->applyTextFormat();
    updateGear(4);
    if (!_underConstruct)
        updateSize();
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

void GBasicTextField::setSingleLine(bool value)
{
    _label->enableWrap(!value);
    if (!_underConstruct)
        updateSize();
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
    {
        _label->setDimensions(_size.width, _size.height);

        if (_autoSize == TextAutoSize::HEIGHT)
        {
            if (_label->getString().length() > 0)
                setSizeDirectly(_size.width, _label->getContentSize().height);
        }
    }
}

void GBasicTextField::handleGrayedChanged()
{
    GObject::handleGrayedChanged();

    _label->setGrayed(_finalGrayed);
}

void GBasicTextField::setup_AfterAdd(TXMLElement* xml)
{
    GTextField::setup_AfterAdd(xml);

    updateSize();
}

NS_FGUI_END
