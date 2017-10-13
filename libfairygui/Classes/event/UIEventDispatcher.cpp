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

uint32_t UIEventDispatcher::addEventListener(UIEventType eventType, const EventCallback& callback, uint32_t callbackId)
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
    _callbacks.push_back(item);
    _count++;

    return item.id;
}

void UIEventDispatcher::removeEventListener(UIEventType eventType, uint32_t callbackId)
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

bool UIEventDispatcher::hasEventListener(UIEventType eventType)
{
    for (auto it = _callbacks.cbegin(); it != _callbacks.cend(); ++it)
    {
        if (it->eventType == eventType && it->callback != nullptr)
            return true;
    }
    return false;
}

void UIEventDispatcher::dispatchEvent(UIEventType eventType, void* data)
{
    if (_count == 0)
        return;

    EventContext context;
    context._sender = this;
    context._inputEvent = InputProcessor::getRecentInput();
    context._data = data;

    doDispatch(eventType, &context);
}

void UIEventDispatcher::bubbleEvent(UIEventType eventType, void* data)
{
    EventContext context;
    context._inputEvent = InputProcessor::getRecentInput();
    context._data = data;

    doBubble(eventType, &context);
}

void UIEventDispatcher::doDispatch(UIEventType eventType, EventContext* context)
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
            context->_touchEndCapture = false;
            it->callback(context);
            if (context->_touchEndCapture && eventType == UIEventType::TouchBegin)
                ((InputProcessor*)context->_data)->addTouchEndMonitor(context->getInput()->getTouchId(), this);
        }
    }
    release();

    _dispatching--;
}

void UIEventDispatcher::doBubble(UIEventType eventType, EventContext* context)
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