#ifndef __EVENTCONTEXT_H__
#define __EVENTCONTEXT_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "InputEvent.h"

NS_FGUI_BEGIN

class GObject;

class EventContext
{
public:
    EventContext();
    ~EventContext();

    cocos2d::Ref* getSender() { return _sender; }
    void* getData() { return _data; }
    InputEvent* getInput() { return _inputEvent; }
    void stopPropagation() { _isStopped = true; }
    void captureTouch() { _touchEndCapture = true; }

private:
    cocos2d::Ref* _sender;
    InputEvent* _inputEvent;
    void* _data;
    bool _isStopped;
    bool _touchEndCapture;

    friend class UIEventDispatcher;
};

NS_FGUI_END

#endif // __EVENTCONTEXT_H__
