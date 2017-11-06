#include "InputProcessor.h"
#include "GComponent.h"
#include "InputEvent.h"
#include "GRoot.h"

NS_FGUI_BEGIN
USING_NS_CC;

InputEvent InputProcessor::_recentInput;

static UIEventDispatcher* __null_monitor__ = new UIEventDispatcher();
static bool __hook_mouse_event__ = false;
static cocos2d::EventMouse::MouseButton __mouseButton__ = EventMouse::MouseButton::BUTTON_LEFT;

#ifdef CC_PLATFORM_PC
void __hook_onGLFWMouseCallBack__(GLFWwindow *window, int button, int action, int p)
{
    if (action == GLFW_PRESS || action == GLFW_RELEASE)
        __mouseButton__ = static_cast<cocos2d::EventMouse::MouseButton>(button);
    else
        __mouseButton__ = EventMouse::MouseButton::BUTTON_UNSET;

    GLFWEventHandler::onGLFWMouseCallBack(window, button, action, p);

    __mouseButton__ = EventMouse::MouseButton::BUTTON_UNSET;
}
#endif

InputProcessor::InputProcessor(GComponent* owner)
{
    _owner = owner;

#ifdef CC_PLATFORM_PC
    if (!__hook_mouse_event__)
    {
        __hook_mouse_event__ = true;
        GLFWwindow* window = ((GLViewImpl*)Director::getInstance()->getOpenGLView())->getWindow();
        glfwSetMouseButtonCallback(window, __hook_onGLFWMouseCallBack__);
        glfwSetCursorPosCallback(window, GLFWEventHandler::onGLFWMouseMoveCallBack);
    }

    _mouseListener = EventListenerMouse::create();
    CC_SAFE_RETAIN(_mouseListener);
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
}

InputProcessor::~InputProcessor()
{
#ifdef CC_PLATFORM_PC
    _owner->displayObject()->getEventDispatcher()->removeEventListener(_mouseListener);
#endif
    _owner->displayObject()->getEventDispatcher()->removeEventListener(_touchListener);
    CC_SAFE_RELEASE_NULL(_touchListener);
    CC_SAFE_RELEASE_NULL(_mouseListener);

    for (auto &ti : _touches)
        ti->reset();

    _owner = nullptr;
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

void InputProcessor::updateRecentInput(TouchInfo* ti)
{
    _recentInput._pos.x = (int)ti->pos.x;
    _recentInput._pos.y = (int)ti->pos.y;
    _recentInput._target = ti->target;
    _recentInput._clickCount = ti->clickCount;
    _recentInput._button = ti->button;
    _recentInput._mouseWheelDelta = ti->mouseWheelDelta;
    _recentInput._touch = ti->touch;
    _recentInput._touchId = ti->touch ? ti->touchId : -1;
    _recentInput._inputProcessor = this;
}

void InputProcessor::handleRollOver(TouchInfo* touch)
{
    GObject* element;
    element = touch->lastRollOver;
    while (element != nullptr)
    {
        _rollOutChain.push_back(element);
        element = element->getParent();
    }

    if (touch->lastRollOver)
        CC_SAFE_RELEASE(touch->lastRollOver);
    touch->lastRollOver = touch->target;
    if (touch->lastRollOver)
        CC_SAFE_RETAIN(touch->lastRollOver);

    element = touch->target;
    int i;
    while (element != nullptr)
    {
        auto iter = std::find(_rollOutChain.cbegin(), _rollOutChain.cend(), element);
        if (iter != _rollOutChain.cend())
        {
            _rollOutChain.resize(iter - _rollOutChain.cbegin());
            break;
        }
        _rollOverChain.push_back(element);

        element = element->getParent();
    }

    int cnt = _rollOutChain.size();
    if (cnt > 0)
    {
        for (i = 0; i < cnt; i++)
        {
            element = _rollOutChain[i];
            if (element->onStage())
                element->dispatchEvent(UIEventType::RollOut, Value::Null);
        }
        _rollOutChain.clear();
    }

    cnt = _rollOverChain.size();
    if (cnt > 0)
    {
        for (i = 0; i < cnt; i++)
        {
            element = _rollOverChain[i];
            if (element->onStage())
                element->dispatchEvent(UIEventType::RollOver, Value::Null);
        }
        _rollOverChain.clear();
    }
}

void InputProcessor::addTouchMonitor(int touchId, UIEventDispatcher * target)
{
    TouchInfo* ti = getTouch(touchId, false);
    if (ti && ti->touchMonitors.getIndex(target) == -1)
        ti->touchMonitors.pushBack(target);
}

void InputProcessor::removeTouchMonitor(UIEventDispatcher * target)
{
    for (auto it = _touches.cbegin(); it != _touches.cend(); ++it)
    {
        int i = (*it)->touchMonitors.getIndex(target);
        if (i != -1)
            (*it)->touchMonitors.replace(i, __null_monitor__);
    }
}

void InputProcessor::cancelClick(int touchId)
{
    TouchInfo* ti = getTouch(touchId, false);
    if (ti)
        ti->clickCancelled = true;
}

void InputProcessor::setBegin(TouchInfo* touch)
{
    touch->began = true;
    touch->clickCancelled = false;
    touch->downPos = touch->pos;

    touch->downTargets.clear();
    if (touch->target != nullptr)
    {
        touch->downTargets.pushBack(touch->target);
        GObject* obj = touch->target;
        while (obj != nullptr)
        {
            touch->downTargets.pushBack(obj);
            obj = obj->getParent();
        }
    }
}

void InputProcessor::setEnd(TouchInfo* touch)
{
    touch->began = false;

    auto now = clock();

    if ((now - touch->lastClickTime) / CLOCKS_PER_SEC < 0.35f)
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

GObject* InputProcessor::clickTest(TouchInfo* touch)
{
    if (touch->downTargets.size() == 0
        || touch->clickCancelled
        || abs(touch->pos.x - touch->downPos.x) > 50 || abs(touch->pos.y - touch->downPos.y) > 50)
        return nullptr;

    GObject* obj = touch->downTargets.at(0);
    if (obj->onStage()) //依然派发到原来的downTarget，虽然可能它已经偏离当前位置，主要是为了正确处理点击缩放的效果
        return obj;

    obj = touch->target;
    while (obj != nullptr)
    {
        int i = touch->downTargets.getIndex(obj);
        if (i != -1 && obj->onStage())
            return obj;

        obj = obj->getParent();
    }

    return obj;
}

bool InputProcessor::onTouchBegan(Touch *touch, Event* /*unusedEvent*/)
{
    auto camera = Camera::getVisitingCamera();
    Vec2 pt = touch->getLocation();
    GObject* target = _owner->hitTest(pt, camera);
    if (!target)
        target = _owner;

    pt.y = GRoot::getInstance()->getHeight() - pt.y;
    TouchInfo* ti = getTouch(touch->getID());
    ti->target = target;
    ti->pos = pt;
    ti->button = __mouseButton__;
    ti->touch = touch;
    setBegin(ti);

    updateRecentInput(ti);

    if (_captureCallback)
        _captureCallback(UIEventType::TouchBegin);

    target->bubbleEvent(UIEventType::TouchBegin, Value::Null);

    if (ti->lastRollOver != ti->target)
        handleRollOver(ti);

    //_touchListener->setSwallowTouches(target != _owner);

    return true;
}

void InputProcessor::onTouchMoved(Touch *touch, Event* /*unusedEvent*/)
{
    auto camera = Camera::getVisitingCamera();
    Vec2 pt = touch->getLocation();
    GObject* target = _owner->hitTest(pt, camera);
    if (!target)
        target = _owner;

    pt.y = GRoot::getInstance()->getHeight() - pt.y;
    TouchInfo* ti = getTouch(touch->getID());
    ti->target = target;
    ti->pos = pt;
    ti->button = __mouseButton__;
    ti->touch = touch;
    updateRecentInput(ti);

    if (_captureCallback)
        _captureCallback(UIEventType::TouchMove);

    if (ti->lastRollOver != ti->target)
        handleRollOver(ti);

    if (ti->began)
    {
        bool done = false;
        int cnt = ti->touchMonitors.size();
        if (cnt > 0)
        {
            for (int i = 0; i < cnt; i++)
            {
                UIEventDispatcher* mm = ti->touchMonitors.at(i);
                if (mm == __null_monitor__)
                    continue;

                mm->dispatchEvent(UIEventType::TouchMove, Value::Null);
                if (mm == _owner)
                    done = true;
            }
        }
        if (!done)
            _owner->dispatchEvent(UIEventType::TouchMove, Value::Null);
    }
}

void InputProcessor::onTouchEnded(Touch *touch, Event* /*unusedEvent*/)
{
    auto camera = Camera::getVisitingCamera();
    Vec2 pt = touch->getLocation();
    GObject* target = _owner->hitTest(pt, camera);
    if (!target)
        target = _owner;

    pt.y = GRoot::getInstance()->getHeight() - pt.y;
    TouchInfo* ti = getTouch(touch->getID());
    ti->target = target;
    ti->pos = pt;
    ti->button = __mouseButton__;
    ti->touch = touch;
    setEnd(ti);

    updateRecentInput(ti);

    if (_captureCallback)
        _captureCallback(UIEventType::TouchEnd);

    int cnt = ti->touchMonitors.size();
    if (cnt > 0)
    {
        for (int i = 0; i < cnt; i++)
        {
            UIEventDispatcher* mm = ti->touchMonitors.at(i);
            if (mm == __null_monitor__)
                continue;

            if (mm != target
                && (!dynamic_cast<GComponent*>(mm) || !((GComponent*)mm)->isAncestorOf(target)))
                mm->dispatchEvent(UIEventType::TouchEnd, Value::Null);
        }
        ti->touchMonitors.clear();
    }
    target->bubbleEvent(UIEventType::TouchEnd, Value::Null);

    target = clickTest(ti);
    if (target)
    {
        ti->target = target;
        updateRecentInput(ti);
        target->bubbleEvent(UIEventType::Click);
    }

#ifndef CC_PLATFORM_PC
    //移动平台上，手指抬起就当RollOut，但在PC上不是
    ti->target = nullptr;
#endif

    if (ti->lastRollOver != ti->target)
        handleRollOver(ti);

    ti->touchId = -1;
    ti->button = EventMouse::MouseButton::BUTTON_UNSET;
}

void InputProcessor::onTouchCancelled(Touch* touch, Event* /*unusedEvent*/)
{
    TouchInfo* ti = getTouch(touch->getID());
    if (ti == nullptr)
        return;

    ti->target = _owner;
    ti->touch = touch;

    updateRecentInput(ti);

    if (_captureCallback)
        _captureCallback(UIEventType::TouchEnd);

    int cnt = ti->touchMonitors.size();
    if (cnt > 0)
    {
        for (int i = 0; i < cnt; i++)
        {
            UIEventDispatcher* mm = ti->touchMonitors.at(i);
            if (mm == __null_monitor__)
                continue;

            if (mm != _owner)
                mm->dispatchEvent(UIEventType::TouchEnd);
        }
        ti->touchMonitors.clear();
    }
    _owner->dispatchEvent(UIEventType::TouchEnd);

    ti->target = nullptr;
    if (ti->lastRollOver != ti->target)
        handleRollOver(ti);

    ti->touchId = -1;
}

void InputProcessor::onMouseMove(cocos2d::EventMouse * event)
{
    TouchInfo* ti = getTouch(0);
    if (abs(ti->pos.x - event->getCursorX()) < 1
        && abs(ti->pos.y - (GRoot::getInstance()->getHeight() - event->getCursorY())) < 1)
        return;

    auto camera = Camera::getVisitingCamera();
    Vec2 pt(event->getCursorX(), event->getCursorY());
    GObject* target = _owner->hitTest(pt, camera);
    if (!target)
        target = _owner;

    pt.y = GRoot::getInstance()->getHeight() - pt.y;
    ti->target = target;
    ti->pos = pt;
    ti->touch = nullptr;
    updateRecentInput(ti);

    if (_captureCallback)
        _captureCallback(UIEventType::TouchMove);

    if (ti->lastRollOver != ti->target)
        handleRollOver(ti);

    if (ti->began)
    {
        bool done = false;
        int cnt = ti->touchMonitors.size();
        if (cnt > 0)
        {
            for (int i = 0; i < cnt; i++)
            {
                UIEventDispatcher* mm = ti->touchMonitors.at(i);
                if (mm == __null_monitor__)
                    continue;

                mm->dispatchEvent(UIEventType::TouchMove);
                if (mm == _owner)
                    done = true;
            }
        }
        if (!done)
            _owner->dispatchEvent(UIEventType::TouchMove);
    }
}

void InputProcessor::onMouseScroll(cocos2d::EventMouse * event)
{
    auto camera = Camera::getVisitingCamera();
    Vec2 pt(event->getCursorX(), event->getCursorY());
    GObject* target = _owner->hitTest(pt, camera);
    if (!target)
        target = _owner;

    pt.y = GRoot::getInstance()->getHeight() - pt.y;
    TouchInfo* ti = getTouch(0);
    ti->target = target;
    ti->pos = pt;
    ti->touch = nullptr;
    ti->mouseWheelDelta = MAX(event->getScrollX(), event->getScrollY());
    updateRecentInput(ti);
    ti->target->bubbleEvent(UIEventType::MouseWheel);
    ti->mouseWheelDelta = 0;
}

TouchInfo::TouchInfo() :
    touch(nullptr),
    touchId(-1),
    clickCount(0),
    mouseWheelDelta(0),
    button(EventMouse::MouseButton::BUTTON_UNSET),
    began(false),
    lastClickTime(0),
    clickCancelled(false),
    target(nullptr),
    lastRollOver(nullptr)
{
}

TouchInfo::~TouchInfo()
{
    downTargets.clear();
    if (lastRollOver)
        CC_SAFE_RELEASE(lastRollOver);
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
    target = nullptr;
    downTargets.clear();
    if (lastRollOver)
        CC_SAFE_RELEASE(lastRollOver);
    clickCancelled = false;
    touchMonitors.clear();
}

NS_FGUI_END