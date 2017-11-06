#include "Actions.h"

NS_FGUI_BEGIN
USING_NS_CC;

ActionInterval * createEaseAction(cocos2d::tweenfunc::TweenType tweenType, cocos2d::ActionInterval * action)
{
    switch (tweenType)
    {
    case tweenfunc::Linear:
        return EaseIn::create(action, 1);
    case tweenfunc::Elastic_EaseIn:
        return EaseElasticIn::create(action);
    case tweenfunc::Elastic_EaseOut:
        return EaseElasticOut::create(action);
    case tweenfunc::Elastic_EaseInOut:
        return EaseElasticInOut::create(action);

    case tweenfunc::Quad_EaseIn:
        return EaseQuadraticActionIn::create(action);
    case tweenfunc::Quad_EaseOut:
        return EaseQuadraticActionOut::create(action);
    case tweenfunc::Quad_EaseInOut:
        return EaseQuadraticActionInOut::create(action);

    case tweenfunc::Cubic_EaseIn:
        return EaseCubicActionIn::create(action);
    case tweenfunc::Cubic_EaseOut:
        return EaseCubicActionOut::create(action);
    case tweenfunc::Cubic_EaseInOut:
        return EaseCubicActionInOut::create(action);

    case tweenfunc::Quart_EaseIn:
        return EaseQuarticActionIn::create(action);
    case tweenfunc::Quart_EaseOut:
        return EaseQuarticActionOut::create(action);
    case tweenfunc::Quart_EaseInOut:
        return EaseQuarticActionInOut::create(action);

    case tweenfunc::Sine_EaseIn:
        return EaseSineIn::create(action);
    case tweenfunc::Sine_EaseOut:
        return EaseSineOut::create(action);
    case tweenfunc::Sine_EaseInOut:
        return EaseSineInOut::create(action);

    case tweenfunc::Bounce_EaseIn:
        return EaseBounceIn::create(action);
    case tweenfunc::Bounce_EaseOut:
        return EaseBounceOut::create(action);
    case tweenfunc::Bounce_EaseInOut:
        return EaseBounceInOut::create(action);

    case tweenfunc::Circ_EaseIn:
        return EaseCircleActionIn::create(action);
    case tweenfunc::Circ_EaseOut:
        return EaseCircleActionOut::create(action);
    case tweenfunc::Circ_EaseInOut:
        return EaseCircleActionInOut::create(action);

    case tweenfunc::Expo_EaseIn:
        return EaseExponentialIn::create(action);
    case tweenfunc::Expo_EaseOut:
        return EaseExponentialOut::create(action);
    case tweenfunc::Expo_EaseInOut:
        return EaseExponentialInOut::create(action);

    case tweenfunc::Back_EaseIn:
        return EaseBackIn::create(action);
    case tweenfunc::Back_EaseOut:
        return EaseBackOut::create(action);
    case tweenfunc::Back_EaseInOut:
        return EaseBackInOut::create(action);

    default:
        return EaseExponentialOut::create(action);
    }
}

ActionVec2* ActionVec2::create(float duration, const cocos2d::Vec2& from, const cocos2d::Vec2& to,
    ActionVec2Callback onUpdate, std::function<void()> onComplete)
{
    auto ref = new (std::nothrow) ActionVec2();
    if (ref && ref->initWithDuration(duration, from, to, onUpdate, onComplete))
    {
        ref->autorelease();
        return ref;
    }

    delete ref;
    return nullptr;
}

bool ActionVec2::initWithDuration(float duration, const cocos2d::Vec2& from, const cocos2d::Vec2& to,
    ActionVec2Callback onUpdate, std::function<void()> onComplete)
{
    if (ActionInterval::initWithDuration(duration))
    {
        _from = from;
        _to = to;
        _updateCallback = onUpdate;
        _completeCallback = onComplete;
        return true;
    }
    return false;
}

ActionVec2* ActionVec2::clone() const
{
    return ActionVec2::create(_duration, _from, _to, _updateCallback, _completeCallback);
}

void ActionVec2::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);
    _delta = _to - _from;
}

void ActionVec2::update(float delta)
{
    Vec2 value = _to - _delta * (1 - delta);

    if (_updateCallback)
    {
        // report back value to caller
        _updateCallback(value);
    }
}

void ActionVec2::stop()
{
    ActionInterval::stop();

    if (_completeCallback)
        _completeCallback();
}

ActionVec2* ActionVec2::reverse() const
{
    return ActionVec2::create(_duration, _to, _from, _updateCallback, _completeCallback);
}

//--------------------

ActionVec4* ActionVec4::create(float duration, const cocos2d::Vec4& from, const cocos2d::Vec4& to,
    ActionVec4Callback onUpdate, std::function<void()> onComplete)
{
    auto ref = new (std::nothrow) ActionVec4();
    if (ref && ref->initWithDuration(duration, from, to, onUpdate, onComplete))
    {
        ref->autorelease();
        return ref;
    }

    delete ref;
    return nullptr;
}

bool ActionVec4::initWithDuration(float duration, const cocos2d::Vec4& from, const cocos2d::Vec4& to,
    ActionVec4Callback onUpdate, std::function<void()> onComplete)
{
    if (ActionInterval::initWithDuration(duration))
    {
        _from = from;
        _to = to;
        _updateCallback = onUpdate;
        _completeCallback = onComplete;
        return true;
    }
    return false;
}

ActionVec4* ActionVec4::clone() const
{
    return ActionVec4::create(_duration, _from, _to, _updateCallback, _completeCallback);
}

void ActionVec4::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);
    _delta = _to - _from;
}

void ActionVec4::update(float delta)
{
    Vec4 value = _to - _delta * (1 - delta);

    if (_updateCallback)
    {
        // report back value to caller
        _updateCallback(value);
    }
}

void ActionVec4::stop()
{
    ActionInterval::stop();

    if (_completeCallback)
        _completeCallback();
}

ActionVec4* ActionVec4::reverse() const
{
    return ActionVec4::create(_duration, _to, _from, _updateCallback, _completeCallback);
}

//-------------------

ActionMovieClip * ActionMovieClip::create(cocos2d::Animation * animation, float repeatDelay)
{
    ActionMovieClip *action = new (std::nothrow) ActionMovieClip();
    if (action)
    {
        action->setAnimation(animation, repeatDelay);
        action->autorelease();
        return action;
    }

    delete action;
    return nullptr;
}

ActionMovieClip::ActionMovieClip() :
    _animation(nullptr),
    _curFrame(0),
    _curFrameDelay(0),
    _repeatedCount(0),
    _repeatDelay(0),
    _start(0),
    _end(0),
    _times(0),
    _endAt(0),
    _status(0),
    _drawingFrame(-1)
{
}

ActionMovieClip::~ActionMovieClip()
{
    CC_SAFE_RELEASE(_animation);
}

bool ActionMovieClip::isDone() const
{
    return false;
}

void ActionMovieClip::step(float dt)
{
    if (_animation == nullptr)
        return;

    auto frames = _animation->getFrames();
    if (frames.size() == 0)
        return;

    _curFrameDelay += dt;
    AnimationFrame* frame = frames.at(_curFrame);
    float interval = (frame->getDelayUnits() + ((_curFrame == 0 && _repeatedCount > 0) ? _repeatDelay : 0))*_animation->getDelayPerUnit();
    if (_curFrameDelay < interval)
        return;

    _curFrameDelay -= interval;
    if (_curFrameDelay > _animation->getDelayPerUnit())
        _curFrameDelay = _animation->getDelayPerUnit();

    _curFrame++;
    if (_curFrame > frames.size() - 1)
    {
        _curFrame = 0;
        _repeatedCount++;
    }

    if (_status == 1)
    {
        _curFrame = _start;
        _status = 0;
    }
    else if (_status == 2)
    {
        _curFrame = _endAt;
        _status = 3;

        if (_completeCallback)
            _completeCallback();
    }
    else
    {
        if (_curFrame == _end)
        {
            if (_times > 0)
            {
                _times--;
                if (_times == 0)
                    _status = 2;
                else
                    _status = 1;
            }
            else if (_start != 0)
                _status = 1;
        }
    }

    drawFrame();
}

void ActionMovieClip::startWithTarget(Node * target)
{
    Action::startWithTarget(target);

    drawFrame();
}

ActionMovieClip * ActionMovieClip::reverse() const
{
    CC_ASSERT(0);
    return nullptr;
}

ActionMovieClip * ActionMovieClip::clone() const
{
    return ActionMovieClip::create(_animation->clone(), _repeatDelay);
}

void ActionMovieClip::setCurrentFrame(int value)
{
    _curFrame = value;
    _curFrameDelay = 0;
    drawFrame();
}

void ActionMovieClip::setPlaySettings(int start, int end, int times, int endAt, std::function<void()> completeCallback)
{
    auto frames = _animation->getFrames();

    _start = start;
    _end = end;
    if (_end == -1 || _end > frames.size() - 1)
        _end = frames.size() - 1;
    _times = times;
    _endAt = endAt;
    if (_endAt == -1)
        _endAt = _end;
    _curFrame = start;
    _curFrameDelay = 0;
    _status = 0;
    _repeatedCount = 0;
    _completeCallback = completeCallback;
    _drawingFrame = -1;

    drawFrame();
}

void ActionMovieClip::setAnimation(cocos2d::Animation *animation, float repeatDelay)
{
    if (_animation != animation)
    {
        CC_SAFE_RETAIN(animation);
        CC_SAFE_RELEASE(_animation);
        _animation = animation;
    }

    if (_animation == nullptr)
        return;

    _repeatDelay = repeatDelay;
    _completeCallback = nullptr;

    auto frames = _animation->getFrames();
    _start = 0;
    _end = _endAt = frames.size() - 1;
    _times = 0;
    _status = 0;
    _repeatedCount = 0;
    _curFrame = 0;
    _drawingFrame = -1;

    drawFrame();
}

void ActionMovieClip::drawFrame()
{
    if (_target == nullptr || _animation == nullptr)
        return;

    auto frames = _animation->getFrames();
    if (_curFrame == _drawingFrame || frames.size() == 0 || _curFrame > frames.size() - 1)
        return;

    _drawingFrame = _curFrame;
    AnimationFrame* frame = frames.at(_curFrame);
    //auto blend = static_cast<Sprite*>(_target)->getBlendFunc();
    static_cast<Sprite*>(_target)->setSpriteFrame(frame->getSpriteFrame());
    //static_cast<Sprite*>(_target)->setBlendFunc(blend);
}

NS_FGUI_END
