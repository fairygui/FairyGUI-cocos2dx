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

    void addTouchMonitor(int touchId, UIEventDispatcher* target);
    void removeTouchMonitor(UIEventDispatcher* target);

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
    void updateRecentInput(TouchInfo* touch);
    void handleRollOver(TouchInfo* touch);
    void setBegin(TouchInfo* touch);
    void setEnd(TouchInfo* touch);
    GObject* clickTest(TouchInfo* touch);

    cocos2d::EventListenerTouchOneByOne* _touchListener;
    cocos2d::EventListenerMouse* _mouseListener;
    std::vector<TouchInfo*> _touches;
    std::vector<GObject*> _rollOutChain;
    std::vector<GObject*> _rollOverChain;
    GComponent* _owner;
    CaptureEventCallback _captureCallback;
    InputEvent _recentInput;

    static InputProcessor* _activeProcessor;

    friend class UIEventDispatcher;
};

class TouchInfo
{
public:
    TouchInfo();
    ~TouchInfo();

    void reset();

    cocos2d::Touch* touch;
    cocos2d::Vec2 pos;
    int touchId;
    int clickCount;
    int mouseWheelDelta;
    cocos2d::EventMouse::MouseButton button;
    cocos2d::Vec2 downPos;
    bool began;
    bool clickCancelled;
    clock_t lastClickTime;
    GObject* target;
    cocos2d::Vector<GObject*> downTargets;
    GObject* lastRollOver;
    cocos2d::Vector<UIEventDispatcher*> touchMonitors;
};


NS_FGUI_END

#endif
