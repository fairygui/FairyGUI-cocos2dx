#include "InputEvent.h"
#include "GObject.h"

NS_FGUI_BEGIN

InputEvent::InputEvent() :
    _target(nullptr),
    _touch(nullptr),
    _inputProcessor(nullptr),
    _touchId(-1),
    _clickCount(0),
    _mouseWheelDelta(0),
    _button(cocos2d::EventMouse::MouseButton::BUTTON_UNSET)
{
}

InputEvent::~InputEvent()
{

}


NS_FGUI_END