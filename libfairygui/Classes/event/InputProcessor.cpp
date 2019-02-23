#include "InputProcessor.h"
#include "GComponent.h"
#include "InputEvent.h"
#include "GRoot.h"
#include "GRichTextField.h"
#include "GTextInput.h"
#include "utils/WeakPtr.h"

NS_FGUI_BEGIN
USING_NS_CC;

InputProcessor* InputProcessor::_activeProcessor = nullptr;
bool InputProcessor::_touchOnUI = false;
unsigned int InputProcessor::_touchOnUIFlagFrameId = 0;

class TouchInfo
{
public:
    TouchInfo();
    ~TouchInfo();

    void reset();

    cocos2d::Touch* touch;
    cocos2d::Vec2 pos;
    cocos2d::Vec2 lastTouchLocation;
    int touchId;
    int clickCount;
    int mouseWheelDelta;
    cocos2d::EventMouse::MouseButton button;
    cocos2d::Vec2 downPos;
    bool began;
    bool clickCancelled;
    clock_t lastClickTime;
    WeakPtr lastRollOver;
    std::vector<WeakPtr> downTargets;
    std::vector<WeakPtr> touchMonitors;
};

InputProcessor::InputProcessor(GComponent* owner) :
    _keyModifiers(0),
    _mouseListener(nullptr),
    _touchListener(nullptr),
    _keyboardListener(nullptr)
{
    _owner = owner;
    _recentInput._inputProcessor = this;

#ifdef CC_PLATFORM_PC
    _mouseListener = EventListenerMouse::create();
    CC_SAFE_RETAIN(_mouseListener);
    _mouseListener->onMouseDown = CC_CALLBACK_1(InputProcessor::onMouseDown, this);
    _mouseListener->onMouseUp = CC_CALLBACK_1(InputProcessor::onMouseUp, this);
    _mouseListener->onMouseMove = CC_CALLBACK_1(InputProcessor::onMouseMove, this);
    _mouseListener->onMouseScroll = CC_CALLBACK_1(InputProcessor::onMouseScroll, this);
    _owner->displayObject()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_mouseListener, _owner->displayObject());
#endif

    _touchListener = EventListenerTouchOneByOne::create();
    CC_SAFE_RETAIN(_touchListener);
    _touchListener->setSwallowTouches(false);
    _touchListener->onTouchBegan = CC_CALLBACK_2(InputProcessor::onTouchBegan, this);
    _touchListener->onTouchMoved = CC_CALLBACK_2(InputProcessor::onTouchMoved, this);
    _touchListener->onTouchEnded = CC_CALLBACK_2(InputProcessor::onTouchEnded, this);
    _touchListener->onTouchCancelled = CC_CALLBACK_2(InputProcessor::onTouchCancelled, this);
    _owner->displayObject()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_touchListener, _owner->displayObject());

    _keyboardListener = EventListenerKeyboard::create();
    CC_SAFE_RETAIN(_keyboardListener);
    _keyboardListener->onKeyPressed = CC_CALLBACK_2(InputProcessor::onKeyDown, this);
    _keyboardListener->onKeyReleased = CC_CALLBACK_2(InputProcessor::onKeyUp, this);
    _owner->displayObject()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_keyboardListener, _owner->displayObject());
}

InputProcessor::~InputProcessor()
{
#ifdef CC_PLATFORM_PC
    _owner->displayObject()->getEventDispatcher()->removeEventListener(_mouseListener);
#endif
    _owner->displayObject()->getEventDispatcher()->removeEventListener(_touchListener);
    _owner->displayObject()->getEventDispatcher()->removeEventListener(_keyboardListener);
    CC_SAFE_RELEASE_NULL(_touchListener);
    CC_SAFE_RELEASE_NULL(_mouseListener);
    CC_SAFE_RELEASE_NULL(_keyboardListener);

    for (auto &ti : _touches)
        delete ti;
}

cocos2d::Vec2 InputProcessor::getTouchPosition(int touchId)
{
    for (auto &ti : _touches)
    {
        if (ti->touchId == touchId)
            return ti->pos;
    }
    return _recentInput.getPosition();
}

TouchInfo* InputProcessor::getTouch(int touchId, bool createIfNotExisits)
{
    TouchInfo* ret = nullptr;
    for (auto &ti : _touches)
    {
        if (ti->touchId == touchId)
            return ti;
        else if (ti->touchId == -1)
            ret = ti;
    }

    if (!ret)
    {
        if (!createIfNotExisits)
            return nullptr;

        ret = new TouchInfo();
        _touches.push_back(ret);
    }
    ret->touchId = touchId;
    return ret;
}

void InputProcessor::updateRecentInput(TouchInfo* ti, GObject* target)
{
    _recentInput._pos.x = (int)ti->pos.x;
    _recentInput._pos.y = (int)ti->pos.y;
    _recentInput._target = target;
    _recentInput._clickCount = ti->clickCount;
    _recentInput._button = ti->button;
    _recentInput._mouseWheelDelta = ti->mouseWheelDelta;
    _recentInput._touch = ti->touch;
    _recentInput._touchId = ti->touch ? ti->touchId : -1;

    int curFrame = Director::getInstance()->getTotalFrames();
    bool flag = target != _owner;
    if (curFrame != _touchOnUIFlagFrameId)
        _touchOnUI = flag;
    else if (flag)
        _touchOnUI = true;
    _touchOnUIFlagFrameId = curFrame;
}

void InputProcessor::handleRollOver(TouchInfo* touch, GObject* target)
{
    if (touch->lastRollOver == target)
        return;

    std::vector<WeakPtr> rollOutChain;
    std::vector<WeakPtr> rollOverChain;
    GObject* element = touch->lastRollOver.ptr();
    while (element != nullptr)
    {
        rollOutChain.push_back(WeakPtr(element));
        element = element->findParent();
    }

    element = target;
    while (element != nullptr)
    {
        auto iter = std::find(rollOutChain.cbegin(), rollOutChain.cend(), element);
        if (iter != rollOutChain.cend())
        {
            rollOutChain.resize(iter - rollOutChain.cbegin());
            break;
        }
        rollOverChain.push_back(WeakPtr(element));

        element = element->findParent();
    }

    touch->lastRollOver = target;

    for (auto &wptr : rollOutChain)
    {
        element = wptr.ptr();
        if (element && element->onStage())
            element->dispatchEvent(UIEventType::RollOut);
    }

    for (auto &wptr : rollOverChain)
    {
        element = wptr.ptr();
        if (element && element->onStage())
            element->dispatchEvent(UIEventType::RollOver);
    }
}

void InputProcessor::addTouchMonitor(int touchId, GObject * target)
{
    TouchInfo* ti = getTouch(touchId, false);
    if (!ti)
        return;

    auto it = std::find(ti->touchMonitors.cbegin(), ti->touchMonitors.cend(), target);
    if (it == ti->touchMonitors.cend())
        ti->touchMonitors.push_back(WeakPtr(target));
}

void InputProcessor::removeTouchMonitor(GObject * target)
{
    for (auto it = _touches.cbegin(); it != _touches.cend(); ++it)
    {
        auto it2 = std::find((*it)->touchMonitors.begin(), (*it)->touchMonitors.end(), target);
        if (it2 != (*it)->touchMonitors.end())
            *it2 = nullptr;
    }
}

void InputProcessor::cancelClick(int touchId)
{
    TouchInfo* ti = getTouch(touchId, false);
    if (ti)
        ti->clickCancelled = true;
}

void InputProcessor::simulateClick(GObject* target, int touchId)
{
    _activeProcessor = this;

    Vec2 pt = target->localToGlobal(Vec2::ZERO);
    _recentInput._pos.x = pt.x;
    _recentInput._pos.y = pt.y;
    _recentInput._target = target;
    _recentInput._clickCount = 1;
    _recentInput._button = EventMouse::MouseButton::BUTTON_LEFT;
    _recentInput._touch = nullptr;
    _recentInput._touchId = touchId;

    if (_captureCallback)
        _captureCallback(UIEventType::TouchBegin);

    WeakPtr wptr(target);
    target->bubbleEvent(UIEventType::TouchBegin);

    target = wptr.ptr();
    if (target)
    {
        target->bubbleEvent(UIEventType::TouchEnd);

        target = wptr.ptr();
        if (target)
            target->bubbleEvent(UIEventType::Click);
    }

    _activeProcessor = nullptr;
}

void InputProcessor::setBegin(TouchInfo* touch, GObject* target)
{
    touch->began = true;
    touch->clickCancelled = false;
    touch->downPos = touch->pos;

    touch->downTargets.clear();
    GObject* obj = target;
    while (obj != nullptr)
    {
        touch->downTargets.push_back(WeakPtr(obj));
        obj = obj->findParent();
    }
}

void InputProcessor::setEnd(TouchInfo* touch, GObject* target)
{
    touch->began = false;

    auto now = clock();
    float elapsed = (now - touch->lastClickTime) / (double)CLOCKS_PER_SEC;

    if (elapsed < 0.45f)
    {
        if (touch->clickCount == 2)
            touch->clickCount = 1;
        else
            touch->clickCount++;
    }
    else
        touch->clickCount = 1;
    touch->lastClickTime = now;
}

GObject* InputProcessor::clickTest(TouchInfo* touch, GObject* target)
{
    if (touch->downTargets.empty()
        || touch->clickCancelled
        || std::abs(touch->pos.x - touch->downPos.x) > 50 || std::abs(touch->pos.y - touch->downPos.y) > 50)
        return nullptr;

    GObject* obj = touch->downTargets[0].ptr();
    if (obj && obj->onStage())
        return obj;

    obj = target;
    while (obj != nullptr)
    {
        auto it = std::find(touch->downTargets.cbegin(), touch->downTargets.cend(), obj);
        if (it != touch->downTargets.cend() && it->onStage())
        {
            obj = it->ptr();
            break;
        }

        obj = obj->findParent();
    }

    return obj;
}

bool InputProcessor::isTouchOnUI()
{
    return _touchOnUI;
}

bool InputProcessor::onTouchBegan(Touch *touch, Event* /*unusedEvent*/)
{
    auto camera = Camera::getVisitingCamera();
    Vec2 pt = touch->getLocation();
    GObject* target = _owner->hitTest(pt, camera);
    if (!target)
        target = _owner;
    _touchListener->setSwallowTouches(target != _owner);

    TouchInfo* ti = getTouch(touch->getID());
    ti->pos = Vec2(pt.x, UIRoot->getHeight() - pt.y);
	ti->lastTouchLocation = pt;
    ti->button = EventMouse::MouseButton::BUTTON_LEFT;
    ti->touch = touch;
    setBegin(ti, target);

    updateRecentInput(ti, target);
    _activeProcessor = this;

    if (_captureCallback)
        _captureCallback(UIEventType::TouchBegin);

    WeakPtr wptr(target);
    target->bubbleEvent(UIEventType::TouchBegin);
    target = wptr.ptr();

    handleRollOver(ti, target);

    _activeProcessor = nullptr;

    return true;
}

void InputProcessor::onTouchMoved(Touch *touch, Event* /*unusedEvent*/)
{
    auto camera = Camera::getVisitingCamera();
    Vec2 pt = touch->getLocation();
    GObject* target = _owner->hitTest(pt, camera);
    if (!target)
        target = _owner;

    TouchInfo* ti = getTouch(touch->getID());
	if(ti->lastTouchLocation == pt) {
		return;
	}
    ti->pos = Vec2(pt.x, UIRoot->getHeight() - pt.y);
    ti->button = EventMouse::MouseButton::BUTTON_LEFT;
    ti->lastTouchLocation = pt;
    ti->touch = touch;

    updateRecentInput(ti, target);
    _activeProcessor = this;

    if (_captureCallback)
        _captureCallback(UIEventType::TouchMove);

    handleRollOver(ti, target);

    if (ti->began)
    {
        bool done = false;
        size_t cnt = ti->touchMonitors.size();
        if (cnt > 0)
        {
            for (size_t i = 0; i < cnt; i++)
            {
                GObject* mm = ti->touchMonitors[i].ptr();
                if (!mm)
                    continue;

                mm->dispatchEvent(UIEventType::TouchMove);
                if (mm == _owner)
                    done = true;
            }
        }
        if (!done)
            _owner->dispatchEvent(UIEventType::TouchMove);
    }

    _activeProcessor = nullptr;
}

void InputProcessor::onTouchEnded(Touch *touch, Event* /*unusedEvent*/)
{
    auto camera = Camera::getVisitingCamera();
    Vec2 pt = touch->getLocation();
    GObject* target = _owner->hitTest(pt, camera);
    if (!target)
        target = _owner;

    TouchInfo* ti = getTouch(touch->getID());
    ti->pos = Vec2(pt.x, UIRoot->getHeight() - pt.y);
    ti->lastTouchLocation = pt;
    ti->button = EventMouse::MouseButton::BUTTON_LEFT;
    ti->touch = touch;
    setEnd(ti, target);

    updateRecentInput(ti, target);
    _activeProcessor = this;

    if (_captureCallback)
        _captureCallback(UIEventType::TouchEnd);

    WeakPtr wptr(target);
    size_t cnt = ti->touchMonitors.size();
    if (cnt > 0)
    {
        for (size_t i = 0; i < cnt; i++)
        {
            GObject* mm = ti->touchMonitors[i].ptr();
            if (!mm)
                continue;

            if (mm != target
                && (!dynamic_cast<GComponent*>(mm) || !((GComponent*)mm)->isAncestorOf(target)))
                mm->dispatchEvent(UIEventType::TouchEnd);
        }
        ti->touchMonitors.clear();
        target = wptr.ptr();
    }
    if (target)
    {
        target->bubbleEvent(UIEventType::TouchEnd);
        target = wptr.ptr();
    }

    target = clickTest(ti, target);
    if (target)
    {
        wptr = target;
        updateRecentInput(ti, target);

        GRichTextField* tf = dynamic_cast<GRichTextField*>(target);
        if (tf)
        {
            const char* linkHref = dynamic_cast<FUIRichText*>(tf->displayObject())->hitTestLink(pt);
            if (linkHref)
            {
                tf->bubbleEvent(UIEventType::ClickLink, nullptr, Value(linkHref));
                target = wptr.ptr();
            }
        }

        target->bubbleEvent(UIEventType::Click);
        target = wptr.ptr();
    }

#ifndef CC_PLATFORM_PC
    //on mobile platform, trigger RollOut on up event, but not on PC
    handleRollOver(ti, nullptr);
#else
    handleRollOver(ti, target);
#endif
    ti->touchId = -1;
    ti->button = EventMouse::MouseButton::BUTTON_UNSET;

    _activeProcessor = nullptr;
}

void InputProcessor::onTouchCancelled(Touch* touch, Event* /*unusedEvent*/)
{
    TouchInfo* ti = getTouch(touch->getID());
    if (ti == nullptr)
        return;

    ti->touch = touch;
    updateRecentInput(ti, _owner);
    _activeProcessor = this;

    if (_captureCallback)
        _captureCallback(UIEventType::TouchEnd);

    size_t cnt = ti->touchMonitors.size();
    if (cnt > 0)
    {
        for (size_t i = 0; i < cnt; i++)
        {
            GObject* mm = ti->touchMonitors[i].ptr();
            if (!mm)
                continue;

            if (mm != _owner)
                mm->dispatchEvent(UIEventType::TouchEnd);
        }
        ti->touchMonitors.clear();
    }
    _owner->dispatchEvent(UIEventType::TouchEnd);

    handleRollOver(ti, nullptr);

    ti->touchId = -1;
    ti->button = EventMouse::MouseButton::BUTTON_UNSET;
    _activeProcessor = nullptr;
}

void InputProcessor::onMouseDown(cocos2d::EventMouse * event)
{
    if (event->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT)
        return;

    auto camera = Camera::getVisitingCamera();
    Vec2 pt(event->getCursorX(), event->getCursorY());
    GObject* target = _owner->hitTest(pt, camera);
    if (!target)
        target = _owner;
    _touchListener->setSwallowTouches(target != _owner);

    TouchInfo* ti = getTouch(0);
    ti->pos = Vec2(pt.x, UIRoot->getHeight() - pt.y);
    ti->button = event->getMouseButton();
    ti->touch = nullptr;
    setBegin(ti, target);

    updateRecentInput(ti, target);
    _activeProcessor = this;

    if (_captureCallback)
        _captureCallback(UIEventType::TouchBegin);

    WeakPtr wptr(target);
    target->bubbleEvent(UIEventType::TouchBegin);

    _activeProcessor = nullptr;
}

void InputProcessor::onMouseUp(cocos2d::EventMouse * event)
{
    if (event->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT)
        return;

    auto camera = Camera::getVisitingCamera();
    Vec2 pt(event->getCursorX(), event->getCursorY());
    GObject* target = _owner->hitTest(pt, camera);
    if (!target)
        target = _owner;

    TouchInfo* ti = getTouch(0);
    ti->pos = Vec2(pt.x, UIRoot->getHeight() - pt.y);
    ti->button = event->getMouseButton();
    ti->touch = nullptr;
    setEnd(ti, target);

    updateRecentInput(ti, target);
    _activeProcessor = this;

    if (_captureCallback)
        _captureCallback(UIEventType::TouchEnd);

    WeakPtr wptr(target);
    size_t cnt = ti->touchMonitors.size();
    if (cnt > 0)
    {
        for (size_t i = 0; i < cnt; i++)
        {
            GObject* mm = ti->touchMonitors[i].ptr();
            if (!mm)
                continue;

            if (mm != target
                && (!dynamic_cast<GComponent*>(mm) || !((GComponent*)mm)->isAncestorOf(target)))
                mm->dispatchEvent(UIEventType::TouchEnd);
        }
        ti->touchMonitors.clear();
        target = wptr.ptr();
    }
    if (target)
    {
        target->bubbleEvent(UIEventType::TouchEnd);
        target = wptr.ptr();
    }

    target = clickTest(ti, target);
    if (target)
    {
        wptr = target;
        updateRecentInput(ti, target);

        if (ti->button == EventMouse::MouseButton::BUTTON_MIDDLE)
            target->bubbleEvent(UIEventType::MiddleClick);
        else
            target->bubbleEvent(UIEventType::RightClick);
    }

    ti->touchId = -1;
    ti->button = EventMouse::MouseButton::BUTTON_UNSET;

    _activeProcessor = nullptr;
}

void InputProcessor::onMouseMove(cocos2d::EventMouse * event)
{
    TouchInfo* ti = getTouch(0);
    if (std::abs(ti->pos.x - event->getCursorX()) < 1
        && std::abs(ti->pos.y - (UIRoot->getHeight() - event->getCursorY())) < 1)
        return;

    auto camera = Camera::getVisitingCamera();
    Vec2 pt(event->getCursorX(), event->getCursorY());
    GObject* target = _owner->hitTest(pt, camera);
    if (!target)
        target = _owner;

    ti->pos = Vec2(pt.x, UIRoot->getHeight() - pt.y);
    ti->touch = nullptr;

    updateRecentInput(ti, target);
    _activeProcessor = this;

    if (_captureCallback)
        _captureCallback(UIEventType::TouchMove);

    handleRollOver(ti, target);

    if (ti->began)
    {
        bool done = false;
        size_t cnt = ti->touchMonitors.size();
        if (cnt > 0)
        {
            for (size_t i = 0; i < cnt; i++)
            {
                GObject* mm = ti->touchMonitors[i].ptr();
                if (!mm)
                    continue;

                mm->dispatchEvent(UIEventType::TouchMove);
                if (mm == _owner)
                    done = true;
            }
        }
        if (!done)
            _owner->dispatchEvent(UIEventType::TouchMove);
    }

    _activeProcessor = nullptr;
}

void InputProcessor::onMouseScroll(cocos2d::EventMouse * event)
{
    auto camera = Camera::getVisitingCamera();
    Vec2 pt(event->getCursorX(), event->getCursorY());
    GObject* target = _owner->hitTest(pt, camera);
    if (!target)
        target = _owner;

    TouchInfo* ti = getTouch(0);
    ti->pos = Vec2(pt.x, UIRoot->getHeight() - pt.y);
    ti->touch = nullptr;
    ti->mouseWheelDelta = MAX(event->getScrollX(), event->getScrollY());

    updateRecentInput(ti, target);
    _activeProcessor = this;

    target->bubbleEvent(UIEventType::MouseWheel);
    ti->mouseWheelDelta = 0;

    _activeProcessor = nullptr;
}

void InputProcessor::onKeyDown(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event * event)
{
    if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_CTRL || keyCode == EventKeyboard::KeyCode::KEY_RIGHT_CTRL)
        _keyModifiers |= 1;
    else if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ALT || keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ALT)
        _keyModifiers |= 2;
    else if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_SHIFT || keyCode == EventKeyboard::KeyCode::KEY_RIGHT_SHIFT)
        _keyModifiers |= 4;

    _recentInput._keyCode = keyCode;
    _recentInput._target = _owner;
    _recentInput._target->dispatchEvent(UIEventType::KeyDown);
}

void InputProcessor::onKeyUp(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *)
{
    if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_CTRL || keyCode == EventKeyboard::KeyCode::KEY_RIGHT_CTRL)
        _keyModifiers &= ~1;
    else if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ALT || keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ALT)
        _keyModifiers &= ~2;
    else if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_SHIFT || keyCode == EventKeyboard::KeyCode::KEY_RIGHT_SHIFT)
        _keyModifiers &= ~4;

    _recentInput._keyCode = keyCode;
    _recentInput._target = _owner;
    _recentInput._target->dispatchEvent(UIEventType::KeyUp);
}

TouchInfo::TouchInfo() :
    touch(nullptr),
    touchId(-1),
    clickCount(0),
    mouseWheelDelta(0),
    button(EventMouse::MouseButton::BUTTON_UNSET),
    began(false),
    lastClickTime(0),
    clickCancelled(false)
{
}

TouchInfo::~TouchInfo()
{
    downTargets.clear();
    touchMonitors.clear();
}

void TouchInfo::reset()
{
    touchId = -1;
    mouseWheelDelta = 0;
    button = EventMouse::MouseButton::BUTTON_UNSET;
    touch = nullptr;
    pos.setZero();
    downPos.setZero();
    clickCount = 0;
    lastClickTime = 0;
    began = false;
    downTargets.clear();
    lastRollOver = nullptr;
    clickCancelled = false;
    touchMonitors.clear();
}

NS_FGUI_END
