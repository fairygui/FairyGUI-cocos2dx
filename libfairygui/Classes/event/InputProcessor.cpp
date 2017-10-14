#include "InputProcessor.h"
#include "GComponent.h"
#include "InputEvent.h"
#include "GRoot.h"

NS_FGUI_BEGIN
USING_NS_CC;

InputEvent InputProcessor::_recentInput;

InputProcessor::InputProcessor(GComponent* owner)
{
    _owner = owner;

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
    _owner->displayObject()->getEventDispatcher()->removeEventListener(_touchListener);
    CC_SAFE_RELEASE_NULL(_touchListener);

    _owner = nullptr;
}

TouchInfo* InputProcessor::getTouch(int touchId, bool createIfNotExisits)
{
    TouchInfo* ret = nullptr;
    for (auto it = _touches.cbegin(); it != _touches.cend(); ++it)
    {
        if ((*it)->touchId == touchId)
            return (*it);
        else if ((*it)->touchId == -1)
            ret = *it;
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

void InputProcessor::updateRecentInput(TouchInfo* touch)
{
    _recentInput._pos.x = (int)touch->pos.x;
    _recentInput._pos.y = (int)(GRoot::getInstance()->getHeight() - touch->pos.y);
    _recentInput._target = touch->target;
    _recentInput._clickCount = touch->clickCount;
    _recentInput._button = touch->button;
    _recentInput._touch = touch->touch;
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
                element->dispatchEvent(UIEventType::RollOut, this);
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
                element->dispatchEvent(UIEventType::RollOver, this);
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

    if ((now - touch->lastClickTime) / CLOCKS_PER_SEC < 0.35)
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

    TouchInfo* ti = getTouch(touch->getID());
    ti->target = target;
    ti->pos = pt;
    ti->touch = touch;
    setBegin(ti);

    updateRecentInput(ti);
    target->bubbleEvent(UIEventType::TouchBegin, this);

    if (ti->lastRollOver != ti->target)
        handleRollOver(ti);

    _touchListener->setSwallowTouches(target != _owner);

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
    ti->target = target;
    ti->pos = pt;
    ti->touch = touch;
    updateRecentInput(ti);

    if (ti->lastRollOver != ti->target)
        handleRollOver(ti);

    if (ti->touchMonitors.size() > 0)
    {
        for (auto it = ti->touchMonitors.begin(); it != ti->touchMonitors.end(); ++it)
        {
            if (*it != target)
                (*it)->dispatchEvent(UIEventType::TouchMove, false);
        }
    }

    target->bubbleEvent(UIEventType::TouchMove, this);
}

void InputProcessor::onTouchEnded(Touch *touch, Event* /*unusedEvent*/)
{
    auto camera = Camera::getVisitingCamera();
    Vec2 pt = touch->getLocation();
    GObject* target = _owner->hitTest(pt, camera);
    if (!target)
        target = _owner;

    TouchInfo* ti = getTouch(touch->getID());
    ti->target = target;
    ti->pos = pt;
    ti->touch = touch;
    setEnd(ti);

    updateRecentInput(ti);

    if (ti->touchMonitors.size() > 0)
    {
        for (auto it = ti->touchMonitors.begin(); it != ti->touchMonitors.end(); ++it)
        {
            if (*it != target)
                (*it)->dispatchEvent(UIEventType::TouchEnd, false);
        }
    }
    target->bubbleEvent(UIEventType::TouchEnd, this);

    target = clickTest(ti);
    if (target)
    {
        ti->target = target;
        updateRecentInput(ti);
        target->bubbleEvent(UIEventType::Click, this);
    }

    ti->target = nullptr;
    handleRollOver(ti);

    ti->reset();
}

void InputProcessor::onTouchCancelled(Touch* touch, Event* /*unusedEvent*/)
{
    TouchInfo* ti = getTouch(touch->getID());
    if (ti == nullptr)
        return;

    ti->target = _owner;
    ti->touch = touch;

    updateRecentInput(ti);

    _owner->dispatchEvent(UIEventType::TouchEnd, this);

    ti->target = nullptr;
    handleRollOver(ti);
}


TouchInfo::TouchInfo() :
    touch(nullptr),
    touchId(-1),
    clickCount(0),
    button(0),
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
    button = 0;
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