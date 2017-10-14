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
    InputProcessor(GComponent* owner);
    ~InputProcessor();

    static InputEvent* getRecentInput() { return &_recentInput; }

    void addTouchMonitor(int touchId, UIEventDispatcher* target);

private:
    bool onTouchBegan(cocos2d::Touch * touch, cocos2d::Event *);
    void onTouchMoved(cocos2d::Touch * touch, cocos2d::Event *);
    void onTouchEnded(cocos2d::Touch * touch, cocos2d::Event *);
    void onTouchCancelled(cocos2d::Touch * touch, cocos2d::Event *);

    TouchInfo* getTouch(int touchId, bool createIfNotExisits = true);
    void updateRecentInput(TouchInfo* touch);
    void handleRollOver(TouchInfo* touch);
    void setBegin(TouchInfo* touch);
    void setEnd(TouchInfo* touch);
    GObject* clickTest(TouchInfo* touch);

    cocos2d::EventListenerTouchOneByOne* _touchListener;
    std::vector<TouchInfo*> _touches;
    std::vector<GObject*> _rollOutChain;
    std::vector<GObject*> _rollOverChain;
    GComponent* _owner;

    static InputEvent _recentInput;
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
    int button;
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

#endif // __INPUTPROCESSOR_H__
