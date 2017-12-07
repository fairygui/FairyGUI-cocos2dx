#ifndef __FUIINPUT_H__
#define __FUIINPUT_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "ui/UIEditBox/UIEditBox.h"
#include "TextFormat.h"

NS_FGUI_BEGIN

class GTextInput;

class FUIInput : public cocos2d::ui::EditBox, cocos2d::ui::EditBoxDelegate
{
public:
    static FUIInput* create(GTextInput* owner);

    FUIInput();
    virtual ~FUIInput();
    
    const std::string& getText() const;
    void setText(const std::string& value);

    TextFormat* getTextFormat() const { return _textFormat; }
    void applyTextFormat();

    bool isSingleLine() const;
    void setSingleLine(bool value);

    bool isPassword() const { return _password; }
    void setPassword(bool value);

    int keyboardType() const { return _keyboardType;  }
    void setKeyboardType(int value);

    GTextInput* getOwner() const { return _owner; }

private:
    void init(GTextInput* owner);
    void editBoxReturn(cocos2d::ui::EditBox* editBox);

    GTextInput* _owner;
    std::string _text;
    TextFormat* _textFormat;
    bool _password;
    int _keyboardType;
};

NS_FGUI_END

#endif
