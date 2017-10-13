#include "EventContext.h"
#include "GObject.h"

NS_FGUI_BEGIN

EventContext::EventContext() :
    _sender(nullptr),
    _data(nullptr),
    _inputEvent(nullptr),
    _isStopped(false),
    _touchEndCapture(false)
{
}

EventContext::~EventContext()
{

}


NS_FGUI_END