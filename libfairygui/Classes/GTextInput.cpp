#include "GTextInput.h"
#include "UIPackage.h"
#include "ui/UIEditBox/UIEditBox.h"

NS_FGUI_BEGIN
USING_NS_CC;

GTextInput::GTextInput()
{
}

GTextInput::~GTextInput()
{

}

void GTextInput::handleInit()
{
    _input = FUIInput::create(this);
    _input->retain();

    setFontName(UIConfig::defaultFont);
    setColor(Color3B::BLACK);
    setFontSize(12);
    _input->setPlaceholderFontSize(12);

    _displayObject = _input;
}

const std::string & GTextInput::getText() const
{
    std::string& tmp = (std::string&)_text;
    tmp = _input->getText();
    return _text;
}

void GTextInput::setText(const std::string & value)
{
    _text = value;
    _input->setText(value.c_str());
}

void GTextInput::setFontName(const std::string & value)
{
    _input->setFontName(value.c_str());
}

void GTextInput::setFontSize(int value)
{
    _input->setFontSize(value);
    _input->setPlaceholderFontSize(value);
}

void GTextInput::setSingleLine(bool value)
{
    _input->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
}

void GTextInput::setColor(const cocos2d::Color3B & value)
{
    _color = value;
    _input->setFontColor((Color4B)value);
    updateGear(4);
}

void GTextInput::handleSizeChanged()
{
    _input->setContentSize(_size);
}

void GTextInput::setup_BeforeAdd(TXMLElement * xml)
{
    GTextField::setup_BeforeAdd(xml);

    const char *p;

    p = xml->Attribute("prompt");
    if (p)
        _input->setPlaceHolder(p);

    if (xml->BoolAttribute("password"))
        _input->setInputFlag(ui::EditBox::InputFlag::PASSWORD);

    //xml->Attribute("restrict");
    p = xml->Attribute("maxLength");
    if (p)
        _input->setMaxLength(atoi(p));

    /*p = xml->Attribute("keyboardType");
    if (p)
        _input->setInputMode((ui::EditBox::InputMode)atoi(p));*/
}

NS_FGUI_END