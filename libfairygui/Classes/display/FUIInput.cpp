#include "FUIInput.h"
#include "UIPackage.h"
#include "GTextInput.h"

NS_FGUI_BEGIN
USING_NS_CC;

FUIInput * FUIInput::create(GTextInput* owner)
{
    FUIInput* pRet = new (std::nothrow) FUIInput();

    if (pRet != nullptr && pRet->initWithSizeAndBackgroundSprite(Size(100, 100),
        (ui::Scale9Sprite*)ui::Scale9Sprite::createWithTexture(UIPackage::getEmptyTexture())))
    {
        pRet->autorelease();
        pRet->init(owner);
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

FUIInput::FUIInput():
    _textFormat(new TextFormat()),
    _password(false),
    _keyboardType(0)
{
}

FUIInput::~FUIInput()
{
    delete _textFormat;
}

std::string FUIInput::getText() const
{
    return ui::EditBox::getText();
}

void FUIInput::setText(const std::string & value)
{
    ui::EditBox::setText(value.c_str());
}

void FUIInput::applyTextFormat()
{
    setFontName(UIConfig::getRealFontName(_textFormat->face).c_str());
    setFontSize(_textFormat->fontSize);
    setPlaceholderFontSize(_textFormat->fontSize);
    setFontColor(_textFormat->color);
    //setPlaceholderFontColor(_textFormat->color);
}

bool FUIInput::isSingleLine() const
{
    return getInputMode() == ui::EditBox::InputMode::SINGLE_LINE;
}

void FUIInput::setSingleLine(bool value)
{
    setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
}

void FUIInput::setPassword(bool value)
{
    _password = value;
    setInputFlag(ui::EditBox::InputFlag::PASSWORD);
}

void FUIInput::setKeyboardType(int value)
{
    //if (!_password)
        //setInputMode((ui::EditBox::InputMode)value);
}

void FUIInput::init(GTextInput* owner)
{
    _owner = owner;
    setDelegate(this);

    applyTextFormat();
}

void FUIInput::editBoxReturn(cocos2d::ui::EditBox * editBox)
{
    //found that this will trigger even when focus is lost
    //if (isSingleLine())
       // _owner->dispatchEvent(UIEventType::Submit);
}

NS_FGUI_END