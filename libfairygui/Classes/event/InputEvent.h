#ifndef __INPUTEVENT_H__
#define __INPUTEVENT_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class GObject;

class InputEvent
{
public:
    InputEvent();
    ~InputEvent();

    GObject* getTarget() { return _target; }
    const int getX() { return _pos.x; }
    const int getY() { return _pos.y; }
    const cocos2d::Vec2& getPosition() { return _pos; }
    cocos2d::Touch* getTouch() { return _touch; }
    int getTouchId() { return _touchId; }
    int isDoubleClick() { return _clickCount == 2; }
    cocos2d::EventMouse::MouseButton getButton() { return _button; }
    int getMouseWheelDelta() { return _mouseWheelDelta; }

    bool isShift() { return false; }
    bool isCtrl() { return false; }

private:
    GObject* _target;
    cocos2d::Touch* _touch;
    cocos2d::Vec2 _pos;
    int _touchId;
    int _clickCount;
    int _mouseWheelDelta;
    cocos2d::EventMouse::MouseButton _button;

    friend class InputProcessor;
};

NS_FGUI_END

#endif
