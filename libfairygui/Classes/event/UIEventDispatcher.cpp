#include "UIEventDispatcher.h"
#include "GComponent.h"
#include "InputProcessor.h"

NS_FGUI_BEGIN

int UIEventDispatcher::_gCallbackCounter = 0;

UIEventDispatcher::UIEventDispatcher() :_count(0), _dispatching(0)
{
}

UIEventDispatcher::~UIEventDispatcher()
{

}

uint32_t UIEventDispatcher::addEventListener(int eventType, const EventCallback& callback, uint32_t callbackId)
{
    if (callbackId == 0)
    {
        _gCallbackCounter++;
        if (_gCallbackCounter > 0x0EEEEEEE)
            _gCallbackCounter = 0;
        callbackId = _gCallbackCounter;
    }
    EventCallbackItem item;
    item.callback = callback;
    item.eventType = eventType;
    item.id = callbackId;
    item.dispatching = 0;
    _callbacks.push_back(item);
    _count++;

    return item.id;
}

void UIEventDispatcher::removeEventListener(int eventType, uint32_t callbackId)
{
    for (auto it = _callbacks.begin(); it != _callbacks.end(); ++it)
    {
        if (it->eventType == eventType && it->id == callbackId)
        {
            if (_dispatching > 0)
                it->callback = nullptr;
            else
                _callbacks.erase(it);
            _count--;
            break;
        }
    }
}

void UIEventDispatcher::removeEventListeners()
{
    if (_dispatching > 0)
    {
        for (auto it = _callbacks.begin(); it != _callbacks.end(); ++it)
            it->callback = nullptr;
    }
    else
        _callbacks.clear();
}

bool UIEventDispatcher::hasEventListener(int eventType)
{
    for (auto it = _callbacks.cbegin(); it != _callbacks.cend(); ++it)
    {
        if (it->eventType == eventType && it->callback != nullptr)
            return true;
    }
    return false;
}

void UIEventDispatcher::dispatchEvent(int eventType, void* data)
{
    if (_count == 0)
        return;

    EventContext context;
    context._sender = this;
    context._inputEvent = InputProcessor::getRecentInput();
    context._data = data;

    doDispatch(eventType, &context);
}

void UIEventDispatcher::bubbleEvent(int eventType, void* data)
{
    EventContext context;
    context._inputEvent = InputProcessor::getRecentInput();
    context._data = data;

    doBubble(eventType, &context);
}

bool UIEventDispatcher::isDispatchingEvent(int eventType)
{
    for (auto it = _callbacks.begin(); it != _callbacks.end(); ++it)
    {
        if (it->eventType == eventType)
            return it->dispatching > 0;
    }
    return false;
}

void UIEventDispatcher::doDispatch(int eventType, EventContext* context)
{
    _dispatching++;

    retain();
    context->_sender = this;
    for (auto it = _callbacks.begin(); it != _callbacks.end(); ++it)
    {
        if (it->callback == nullptr)
            _callbacks.erase(it);
        else if (it->eventType == eventType)
        {
            it->dispatching++;
            context->_touchEndCapture = false;
            it->callback(context);
            it->dispatching--;
            if (context->_touchEndCapture && eventType == UIEventType::TouchBegin)
                ((InputProcessor*)context->_data)->addTouchMonitor(context->getInput()->getTouchId(), this);
        }
    }
    release();

    _dispatching--;
}

void UIEventDispatcher::doBubble(int eventType, EventContext* context)
{
    if (_count > 0)
    {
        context->_isStopped = false;
        doDispatch(eventType, context);
        if (context->_isStopped)
            return;
    }

    GComponent* p = ((GObject*)this)->getParent();
    if (p)
        p->doBubble(eventType, context);
}

NS_FGUI_END