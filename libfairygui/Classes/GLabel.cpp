#include "GLabel.h"
#include "GButton.h"
#include "GTextField.h"
#include "GTextInput.h"
#include "PackageItem.h"
#include "utils/Bytebuffer.h"

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

cocos2d::Color3B GLabel::getTitleColor() const
{
    GTextField* tf = getTextField();
    if (tf)
        return tf->getColor();
    else
        return Color3B::BLACK;
}

void GLabel::setTitleColor(const cocos2d::Color3B & value)
{
    GTextField* tf = getTextField();
    if (tf)
        tf->setColor(value);
}

int GLabel::getTitleFontSize() const
{
    GTextField* tf = getTextField();
    if (tf)
        return tf->getFontSize();
    else
        return 0;
}

void GLabel::setTitleFontSize(int value)
{
    GTextField* tf = getTextField();
    if (tf)
        tf->setFontSize(value);
}

GTextField * GLabel::getTextField() const
{
    if (dynamic_cast<GTextField*>(_titleObject))
        return dynamic_cast<GTextField*>(_titleObject);
    else if (dynamic_cast<GLabel*>(_titleObject))
        return dynamic_cast<GLabel*>(_titleObject)->getTextField();
    else if (dynamic_cast<GButton*>(_titleObject))
        return dynamic_cast<GButton*>(_titleObject)->getTextField();
    else
        return nullptr;
}

void GLabel::constructExtension(ByteBuffer* buffer)
{
    _titleObject = getChild("title");
    _iconObject = getChild("icon");
    if (_titleObject != nullptr)
        _title = _titleObject->getText();
    if (_iconObject != nullptr)
        _icon = _iconObject->getIcon();
}

void GLabel::setup_afterAdd(ByteBuffer* buffer, int beginPos)
{
    GComponent::setup_afterAdd(buffer, beginPos);

    if (!buffer->Seek(beginPos, 6))
        return;

    if ((ObjectType)buffer->ReadByte() != _packageItem->objectType)
        return;

    const std::string* str;

    if ((str = buffer->ReadSP()))
        setTitle(*str);
    if ((str = buffer->ReadSP()))
        setIcon(*str);
    if (buffer->ReadBool())
        setTitleColor((Color3B)buffer->ReadColor());
    int iv = buffer->ReadInt();
    if (iv != 0)
        setTitleFontSize(iv);

    if (buffer->ReadBool())
    {
        GTextInput* input = dynamic_cast<GTextInput*>(getTextField());
        if (input)
        {
            if ((str = buffer->ReadSP()))
                input->setPrompt(*str);
            if ((str = buffer->ReadSP()))
                input->setRestrict(*str);
            iv = buffer->ReadInt();
            if (iv != 0)
                input->setMaxLength(iv);
            iv = buffer->ReadInt();
            if (iv != 0)
                input->setKeyboardType(iv);
            if (buffer->ReadBool())
                input->setPassword(true);
        }
        else
            buffer->Skip(13);
    }
}

NS_FGUI_END