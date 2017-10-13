#include "GTextField.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
USING_NS_CC;

GTextField::GTextField() :_label(nullptr), _fontSize(12)
{
}

GTextField::~GTextField()
{

}

void GTextField::setText(const std::string & text)
{
    _label->setString(text);
}

const std::string& GTextField::getText()
{
    return _label->getString();
}

bool GTextField::init()
{
    _label = Label::createWithSystemFont("", UIConfig::defaultFont, 12);
    _label->retain();
    _label->setAnchorPoint(Vec2(0, 1));
    _label->setTextColor(Color4B::BLACK);

    _displayObject = _label;

    return true;
}

void GTextField::handleSizeChanged()
{
    _label->setDimensions(getWidth(), getHeight());
}

void GTextField::setup_BeforeAdd(tinyxml2::XMLElement * xml)
{
    GObject::setup_BeforeAdd(xml);

    const char*p;
    p = xml->Attribute("font");
    if (p)
        _font = p;

    p = xml->Attribute("fontSize");
    if (p)
        _fontSize = atoi(p);

    TTFConfig ttfConfig(UIConfig::defaultFont, _fontSize, GlyphCollection::DYNAMIC);
    _label->setTTFConfig(ttfConfig);

    p = xml->Attribute("color");
    if (p)
        _label->setTextColor(ToolSet::convertFromHtmlColor(p));

    p = xml->Attribute("align");
    if (p)
        _label->setAlignment(ToolSet::parseAlign(p));

    p = xml->Attribute("vAlign");
    if (p)
        _label->setVerticalAlignment(ToolSet::parseVerticalAlign(p));

    p = xml->Attribute("strokeColor");
    if (p)
    {
        Color4B strokeColor = ToolSet::convertFromHtmlColor(p);
        p = xml->Attribute("strokeSize");
        _label->enableOutline(strokeColor, p ? atoi(p) : 1);
    }

    p = xml->Attribute("shadowColor");
    if (p)
    {
        Color4B shadowColor = ToolSet::convertFromHtmlColor(p);
        Vec2 offset;
        ToolSet::getArrayAttribute(xml, "shadowOffset", offset);
        offset.y = -offset.y;
        _label->enableShadow(shadowColor, (cocos2d::Size)offset);
    }
}

void GTextField::setup_AfterAdd(tinyxml2::XMLElement * xml)
{
    GObject::setup_AfterAdd(xml);

    const char* p = xml->Attribute("text");
    if (p && strlen(p) > 0)
        this->setText(p);
}

NS_FGUI_END