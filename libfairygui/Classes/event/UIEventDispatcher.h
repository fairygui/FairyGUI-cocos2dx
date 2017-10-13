#ifndef __UIEVENTDISPATCHER_H__
#define __UIEVENTDISPATCHER_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "EventContext.h"

NS_FGUI_BEGIN

enum UIEventType
{
    TouchBegin,
    TouchMove,
    TouchEnd,
    Click,

    RollOver,
    RollOut,

    PositionChange,
    SizeChange,

    StatusChange
};

typedef std::function<void(EventContext* context)> EventCallback;

class UIEventDispatcher : public cocos2d::Ref
{
public:
    UIEventDispatcher();
    ~UIEventDispatcher();

    uint32_t addEventListener(UIEventType eventType, const EventCallback& callback, uint32_t callbackId = 0);
    void removeEventListener(UIEventType eventType, uint32_t callbackId);
    void removeEventListeners();
    bool hasEventListener(UIEventType eventType);

    void dispatchEvent(UIEventType eventType, void* data = nullptr);
    void bubbleEvent(UIEventType eventType, void* data = nullptr);

private:
    void doDispatch(UIEventType eventType, EventContext* context);
    void doBubble(UIEventType eventType, EventContext* context);

    struct EventCallbackItem
    {
        EventCallback callback;
        UIEventType eventType;
        uint32_t id;
    };
    std::vector<EventCallbackItem> _callbacks;
    int _count;
    int _dispatching;

    static int _gCallbackCounter;
};

NS_FGUI_END

#endif // __UIEVENTDISPATCHER_H__
