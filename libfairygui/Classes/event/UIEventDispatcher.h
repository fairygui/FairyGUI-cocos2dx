#ifndef __UIEVENTDISPATCHER_H__
#define __UIEVENTDISPATCHER_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "EventContext.h"
#include "UIEventType.h"

NS_FGUI_BEGIN

typedef std::function<void(EventContext* context)> EventCallback;

class InputProcessor;

class UIEventDispatcher : public cocos2d::Ref
{
public:
    UIEventDispatcher();
    ~UIEventDispatcher();

    void addEventListener(int eventType, const EventCallback& callback) { return addEventListener(eventType, callback, cocos2d::Node::INVALID_TAG); }
    void addEventListener(int eventType, const EventCallback& callback, int tag);
    void removeEventListener(int eventType) { removeEventListener(eventType, cocos2d::Node::INVALID_TAG); }
    void removeEventListener(int eventType, int tag);
    void removeEventListeners();
    bool hasEventListener(int eventType) const { return hasEventListener(eventType, cocos2d::Node::INVALID_TAG); }
    bool hasEventListener(int eventType, int tag) const;

    bool dispatchEvent(int eventType, cocos2d::Value data = cocos2d::Value::Null);
    bool bubbleEvent(int eventType, cocos2d::Value data = cocos2d::Value::Null);

    bool isDispatchingEvent(int eventType);

private:
    void doDispatch(int eventType, EventContext* context);
    void doBubble(int eventType, EventContext* context);

    struct EventCallbackItem
    {
        EventCallback callback;
        int eventType;
        int tag;
        int dispatching;
    };
    std::vector<EventCallbackItem> _callbacks;
    int _dispatching;
};

NS_FGUI_END

#endif
