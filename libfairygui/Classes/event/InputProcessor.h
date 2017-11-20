#ifndef __INPUTPROCESSOR_H__
#define __INPUTPROCESSOR_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "UIEventDispatcher.h"
#include "InputEvent.h"

NS_FGUI_BEGIN

class GComponent;
class TouchInfo;

class InputProcessor
{
public:
    typedef std::function<void(int eventType)> CaptureEventCallback;

    InputEvent* getRecentInput() { return &_recentInput; }

    InputProcessor(GComponent* owner);
    ~InputProcessor();

    cocos2d::Vec2 getTouchPosition(int touchId);

    void addTouchMonitor(int touchId, GObject* target);
    void removeTouchMonitor(GObject* target);

    void cancelClick(int touchId);

    void setCaptureCallback(CaptureEventCallback value) { _captureCallback = value; }

private:
    bool onTouchBegan(cocos2d::Touch * touch, cocos2d::Event *);
    void onTouchMoved(cocos2d::Touch * touch, cocos2d::Event *);
    void onTouchEnded(cocos2d::Touch * touch, cocos2d::Event *);
    void onTouchCancelled(cocos2d::Touch * touch, cocos2d::Event *);

    void onMouseMove(cocos2d::EventMouse* event);
    void onMouseScroll(cocos2d::EventMouse* event);

    TouchInfo* getTouch(int touchId, bool createIfNotExisits = true);
    void updateRecentInput(TouchInfo* touch, GObject* target);
    void handleRollOver(TouchInfo* touch, GObject* target);
    void setBegin(TouchInfo* touch, GObject* target);
    void setEnd(TouchInfo* touch, GObject* target);
    GObject* clickTest(TouchInfo* touch, GObject* target);

    cocos2d::EventListenerTouchOneByOne* _touchListener;
    cocos2d::EventListenerMouse* _mouseListener;
    std::vector<TouchInfo*> _touches;
    GComponent* _owner;
    CaptureEventCallback _captureCallback;
    InputEvent _recentInput;

    static InputProcessor* _activeProcessor;

    friend class UIEventDispatcher;
};

NS_FGUI_END

#endif
