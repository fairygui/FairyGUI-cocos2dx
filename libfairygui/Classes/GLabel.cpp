#include "GLabel.h"
#include "GButton.h"
#include "GTextField.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
USING_NS_CC;

GLabel::GLabel() :
    _titleObject(nullptr),
    _iconObject(nullptr)
{
}
GLabel::~GLabel()
{
}

void GLabel::setTitle(const std::string & value)
{
    _title = value;
    if (_titleObject != nullptr)
        _titleObject->setText(_title);
    updateGear(6);
}

void GLabel::setIcon(const std::string & value)
{
    _icon = value;
    if (_iconObject != nullptr)
        _iconObject->setIcon(_icon);
    updateGear(7);
}

const cocos2d::Color3B GLabel::getTitleColor() const
{
    if (dynamic_cast<GTextField*>(_titleObject))
        return ((GTextField*)_titleObject)->getColor();
    else if (dynamic_cast<GLabel*>(_titleObject))
        return ((GLabel*)_titleObject)->getTitleColor();
    else if (dynamic_cast<GButton*>(_titleObject))
        return ((GButton*)_titleObject)->getTitleColor();
    else
        return Color3B::BLACK;
}

void GLabel::setTitleColor(const cocos2d::Color3B & value)
{
    if (dynamic_cast<GTextField*>(_titleObject))
        ((GTextField*)_titleObject)->setColor(value);
    else if (dynamic_cast<GLabel*>(_titleObject))
        ((GLabel*)_titleObject)->setTitleColor(value);
    else if (dynamic_cast<GButton*>(_titleObject))
        ((GButton*)_titleObject)->setTitleColor(value);
}

int GLabel::getTitleFontSize() const
{
    if (dynamic_cast<GTextField*>(_titleObject))
        return ((GTextField*)_titleObject)->getFontSize();
    else if (dynamic_cast<GLabel*>(_titleObject))
        return ((GLabel*)_titleObject)->getTitleFontSize();
    else if (dynamic_cast<GButton*>(_titleObject))
        return ((GButton*)_titleObject)->getTitleFontSize();
    else
        return 0;
}

void GLabel::setTitleFontSize(int value)
{
    if (dynamic_cast<GTextField*>(_titleObject))
        ((GTextField*)_titleObject)->setFontSize(value);
    else if (dynamic_cast<GLabel*>(_titleObject))
        ((GLabel*)_titleObject)->setTitleFontSize(value);
    else if (dynamic_cast<GButton*>(_titleObject))
        ((GButton*)_titleObject)->setTitleFontSize(value);
}

void GLabel::constructFromXML(tinyxml2::XMLElement * xml)
{
    GComponent::constructFromXML(xml);

    _titleObject = getChild("title");
    _iconObject = getChild("icon");
    if (_titleObject != nullptr)
        _title = _titleObject->getText();
    if (_iconObject != nullptr)
        _icon = _iconObject->getIcon();
}

void GLabel::setup_AfterAdd(tinyxml2::XMLElement * xml)
{
    GComponent::setup_AfterAdd(xml);

    xml = xml->FirstChildElement("Label");
    if (!xml)
        return;

    const char *p;

    p = xml->Attribute("title");
    if (p)
        this->setTitle(p);

    p = xml->Attribute("icon");
    if (p)
        this->setIcon(p);

    p = xml->Attribute("titleColor");
    if (p)
        setTitleColor((Color3B)ToolSet::convertFromHtmlColor(p));

    p = xml->Attribute("titleFontSize");
    if (p)
        setTitleFontSize(atoi(p));

    GTextInput* input = dynamic_cast<GTextInput*>(_titleObject);
    if (input)
    {/*
        p = xml->Attribute("prompt");
        if (p)
            input->setPlaceHolder(p);

        if (xml->BoolAttribute("password"))
            input->setInputFlag(ui::EditBox::InputFlag::PASSWORD);

        //xml->Attribute("restrict");
        p = xml->Attribute("maxLength");
        if (p)
            input->setMaxLength(atoi(p));

        p = xml->Attribute("keyboardType");
        if (p)
            input->setInputMode((ui::EditBox::InputMode)atoi(p));*/
    }
}

NS_FGUI_END