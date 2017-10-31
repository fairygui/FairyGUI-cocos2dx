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

    cocos2d::Ref* getSender() const { return _sender; }
    void* getData() const { return _data; }
    InputEvent* getInput() const { return _inputEvent; }
    void stopPropagation() { _isStopped = true; }
    void preventDefault() { _defaultPrevented = true; }
    void captureTouch() { _touchCapture = 1; }
    void uncaptureTouch() { _touchCapture = 2; }

private:
    cocos2d::Ref* _sender;
    InputEvent* _inputEvent;
    void* _data;
    bool _isStopped;
    bool _defaultPrevented;
    int _touchCapture;

    friend class UIEventDispatcher;
};

NS_FGUI_END

#endif
