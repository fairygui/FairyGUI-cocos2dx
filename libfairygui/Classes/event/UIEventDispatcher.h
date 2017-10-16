#ifndef __UIEVENTDISPATCHER_H__
#define __UIEVENTDISPATCHER_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "EventContext.h"
#include "UIEventType.h"

NS_FGUI_BEGIN

typedef std::function<void(EventContext* context)> EventCallback;

class UIEventDispatcher : public cocos2d::Ref
{
public:
    UIEventDispatcher();
    ~UIEventDispatcher();

    uint32_t addEventListener(int eventType, const EventCallback& callback, uint32_t callbackId = 0);
    void removeEventListener(int eventType, uint32_t callbackId);
    void removeEventListeners();
    bool hasEventListener(int eventType);

    void dispatchEvent(int eventType, void* data = nullptr);
    void bubbleEvent(int eventType, void* data = nullptr);

private:
    void doDispatch(int eventType, EventContext* context);
    void doBubble(int eventType, EventContext* context);

    struct EventCallbackItem
    {
        EventCallback callback;
        int eventType;
        uint32_t id;
    };
    std::vector<EventCallbackItem> _callbacks;
    int _count;
    int _dispatching;

    static int _gCallbackCounter;
};

NS_FGUI_END

#endif // __UIEVENTDISPATCHER_H__
