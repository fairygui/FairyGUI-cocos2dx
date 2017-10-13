#include "InputEvent.h"
#include "GObject.h"

NS_FGUI_BEGIN

InputEvent::InputEvent() :
    _target(nullptr),
    _touch(nullptr)
{
}

InputEvent::~InputEvent()
{

}


NS_FGUI_END