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

    _displayObject = _input;
}

bool GTextInput::isSingleLine() const
{
    return _input->isSingleLine();
}

void GTextInput::setSingleLine(bool value)
{
    _input->setSingleLine(value);
}

void GTextInput::applyTextFormat()
{
    _input->applyTextFormat();
}

void GTextInput::setPrompt(const std::string & value)
{
    _input->setPlaceHolder(value.c_str());
}

void GTextInput::setPassword(bool value)
{
    _input->setPassword(value);
}

void GTextInput::setKeyboardType(int value)
{
    _input->setKeyboardType(value);
}

void GTextInput::setMaxLength(int value)
{
    _input->setMaxLength(value);
}

void GTextInput::setRestrict(const std::string & value)
{
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
        setPrompt(p);

    if (xml->BoolAttribute("password"))
        setPassword(true);

    p = xml->Attribute("restrict");
    if (p)
        setRestrict(p);

    p = xml->Attribute("maxLength");
    if (p)
        setMaxLength(atoi(p));

    p = xml->Attribute("keyboardType");
    if (p)
        setKeyboardType(atoi(p));
}

NS_FGUI_END