#ifndef __FUIINPUT_H__
#define __FUIINPUT_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "ui/UIEditBox/UIEditBox.h"

NS_FGUI_BEGIN

class GTextInput;

class FUIInput : public cocos2d::ui::EditBox
{
public:
    static FUIInput* create(GTextInput* owner);

    FUIInput();
    ~FUIInput();

    GTextInput* getOwner() const { return _owner; }

private:
    GTextInput* _owner;
};

NS_FGUI_END

#endif
