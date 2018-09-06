#include "GTextInput.h"
#include "UIPackage.h"
#include "ui/UIEditBox/UIEditBox.h"
#include "utils/Bytebuffer.h"

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

const std::string & GTextInput::getText() const
{
    std::string* tmp = const_cast<std::string*>(&_text);
    tmp->clear();
    tmp->append(_input->getText());
    return _text;
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

void GTextInput::setup_beforeAdd(ByteBuffer* buffer, int beginPos)
{
    GTextField::setup_beforeAdd(buffer, beginPos);

    buffer->Seek(beginPos, 4);

    const std::string* str;
    if ((str = buffer->ReadSP()))
        setPrompt(*str);

    if ((str = buffer->ReadSP()))
        setRestrict(*str);

    int iv = buffer->ReadInt();
    if (iv != 0)
        setMaxLength(iv);
    iv = buffer->ReadInt();
    if (iv != 0)
        setKeyboardType(iv);
    if (buffer->ReadBool())
        setPassword(true);
}

void GTextInput::setTextFieldText()
{
    if (_templateVars != nullptr)
        _input->setText(parseTemplate(_text.c_str()));
    else
        _input->setText(_text);
}

NS_FGUI_END