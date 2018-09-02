#include "GTweener.h"
#include "EaseManager.h"
#include "GObject.h"

NS_FGUI_BEGIN
USING_NS_CC;

GTweener::GTweener() :
    _target(nullptr),
    _refTarget(nullptr),
    _userData(nullptr),
    _onStart(nullptr),
    _onUpdate(nullptr),
    _onComplete(nullptr),
    _onComplete0(nullptr)
{
}

GTweener::~GTweener()
{

}

GTweener * GTweener::SetDelay(float value)
{
    _delay = value;
    return this;
}

GTweener * GTweener::SetDuration(float value)
{
    _duration = value;
    return this;
}

GTweener * GTweener::SetBreakpoint(float value)
{
    _breakpoint = value;
    return this;
}

GTweener * GTweener::SetEase(EaseType value)
{
    _easeType = value;
    return this;
}

GTweener * GTweener::SetEasePeriod(float value)
{
    _easePeriod = value;
    return this;
}

GTweener * GTweener::SetEaseOvershootOrAmplitude(float value)
{
    _easeOvershootOrAmplitude = value;
    return this;
}

GTweener * GTweener::SetRepeat(int repeat, bool yoyo)
{
    _repeat = repeat;
    _yoyo = yoyo;
    return this;
}

GTweener * GTweener::SetTimeScale(float value)
{
    _timeScale = value;
    return this;
}

GTweener * GTweener::SetSnapping(bool value)
{
    _snapping = value;
    return this;
}

GTweener * GTweener::SetTargetAny(void * value)
{
    CC_SAFE_RELEASE(_refTarget);
    _refTarget = nullptr;
    _target = value;
    return this;
}

GTweener * GTweener::SetTarget(cocos2d::Ref* value)
{
    return SetTarget(value, TweenPropType::None);
}

GTweener * GTweener::SetTarget(cocos2d::Ref * target, TweenPropType propType)
{
    CC_SAFE_RELEASE(_refTarget);
    _target = _refTarget = target;
    _propType = propType;
    CC_SAFE_RETAIN(_refTarget);
    return this;
}

GTweener * GTweener::SetUserData(const Value& value)
{
    _userData = value;
    return this;
}

GTweener * GTweener::OnUpdate(GTweenCallback callback)
{
    _onUpdate = callback;
    return this;
}

GTweener * GTweener::OnStart(GTweenCallback callback)
{
    _onStart = callback;
    return this;
}

GTweener * GTweener::OnComplete(GTweenCallback callback)
{
    _onComplete = callback;
    return this;
}

GTweener * GTweener::OnComplete0(GTweenCallback0 callback)
{
    _onComplete0 = callback;
    return this;
}

GTweener * GTweener::SetPaused(bool paused)
{
    _paused = paused;
    return this;
}

void GTweener::Seek(float time)
{
    if (_killed)
        return;

    _elapsedTime = time;
    if (_elapsedTime < _delay)
    {
        if (_started)
            _elapsedTime = _delay;
        else
            return;
    }

    Update();
}

void GTweener::Kill(bool complete)
{
    if (_killed)
        return;

    if (complete)
    {
        if (_ended == 0)
        {
            if (_breakpoint >= 0)
                _elapsedTime = _delay + _breakpoint;
            else if (_repeat >= 0)
                _elapsedTime = _delay + _duration * (_repeat + 1);
            else
                _elapsedTime = _delay + _duration * 2;
            Update();
        }

        CallCompleteCallback();
    }

    _killed = true;
}

GTweener * GTweener::_To(float start, float end, float duration)
{
    _valueSize = 1;
    startValue.x = start;
    endValue.x = end;
    _duration = duration;
    return this;
}

GTweener * GTweener::_To(const cocos2d::Vec2 & start, const cocos2d::Vec2 & end, float duration)
{
    _valueSize = 2;
    startValue.SetVec2(start);
    endValue.SetVec2(end);
    _duration = duration;
    return this;
}

GTweener * GTweener::_To(const cocos2d::Vec3 & start, const cocos2d::Vec3 & end, float duration)
{
    _valueSize = 3;
    startValue.SetVec3(start);
    endValue.SetVec3(end);
    _duration = duration;
    return this;
}

GTweener * GTweener::_To(const cocos2d::Vec4 & start, const cocos2d::Vec4 & end, float duration)
{
    _valueSize = 4;
    startValue.SetVec4(start);
    endValue.SetVec4(end);
    _duration = duration;
    return this;
}

GTweener * GTweener::_To(const cocos2d::Color4B & start, const cocos2d::Color4B & end, float duration)
{
    _valueSize = 4;
    startValue.SetColor(start);
    endValue.SetColor(end);
    _duration = duration;
    return this;
}

GTweener * GTweener::_To(double start, double end, float duration)
{
    _valueSize = 5;
    startValue.d = start;
    endValue.d = end;
    _duration = duration;
    return this;
}

GTweener * GTweener::_Shake(const cocos2d::Vec2 & start, float amplitude, float duration)
{
    _valueSize = 6;
    startValue.SetVec2(start);
    startValue.w = amplitude;
    _duration = duration;
    _easeType = EaseType::Linear;
    return this;
}

void GTweener::_Init()
{
    _delay = 0;
    _duration = 0;
    _breakpoint = -1;
    _easeType = EaseType::QuadOut;
    _timeScale = 1;
    _easePeriod = 0;
    _easeOvershootOrAmplitude = 1.70158f;
    _snapping = false;
    _repeat = 0;
    _yoyo = false;
    _valueSize = 0;
    _started = false;
    _paused = false;
    _killed = false;
    _elapsedTime = 0;
    _normalizedTime = 0;
    _ended = 0;
    startValue.SetZero();
    endValue.SetZero();
    value.SetZero();
    deltaValue.SetZero();
}

void GTweener::_Reset()
{
    CC_SAFE_RELEASE(_refTarget);
    _target = nullptr;
    _refTarget = nullptr;
    _userData = nullptr;
    _onStart = _onUpdate = _onComplete = nullptr;
    _onComplete0 = nullptr;
}

void GTweener::_Update(float dt)
{
    if (_ended != 0) //Maybe completed by seek
    {
        CallCompleteCallback();
        _killed = true;
        return;
    }

    if (_timeScale != 1)
        dt *= _timeScale;
    if (dt == 0)
        return;

    _elapsedTime += dt;
    Update();

    if (_ended != 0)
    {
        if (!_killed)
        {
            CallCompleteCallback();
            _killed = true;
        }
    }
}

void GTweener::Update()
{
    _ended = 0;

    if (_valueSize == 0) //DelayedCall
    {
        if (_elapsedTime >= _delay + _duration)
            _ended = 1;

        return;
    }

    if (!_started)
    {
        if (_elapsedTime < _delay)
            return;

        _started = true;
        CallStartCallback();
        if (_killed)
            return;
    }

    bool reversed = false;
    float tt = _elapsedTime - _delay;
    if (_breakpoint >= 0 && tt >= _breakpoint)
    {
        tt = _breakpoint;
        _ended = 2;
    }

    if (_repeat != 0)
    {
        int round = (int)floor(tt / _duration);
        tt -= _duration * round;
        if (_yoyo)
            reversed = round % 2 == 1;

        if (_repeat > 0 && _repeat - round < 0)
        {
            if (_yoyo)
                reversed = _repeat % 2 == 1;
            tt = _duration;
            _ended = 1;
        }
    }
    else if (tt >= _duration)
    {
        tt = _duration;
        _ended = 1;
    }

    _normalizedTime = EaseManager::Evaluate(_easeType, reversed ? (_duration - tt) : tt, _duration,
        _easeOvershootOrAmplitude, _easePeriod);

    value.SetZero();
    deltaValue.SetZero();

    if (_valueSize == 5)
    {
        double d = startValue.d + (endValue.d - startValue.d) * _normalizedTime;
        if (_snapping)
            d = round(d);
        deltaValue.d = d - value.d;
        value.d = d;
    }
    else if (_valueSize == 6)
    {
        if (_ended == 0)
        {
            float r = startValue.w*(1 - _normalizedTime);
            float rx = (rand_0_1() * 2 - 1)*r;
            float ry = (rand_0_1() * 2 - 1)*r;
            rx = rx > 0 ? ceil(rx) : floor(rx);
            ry = ry > 0 ? ceil(ry) : floor(ry);

            deltaValue.x = rx;
            deltaValue.y = ry;
            value.x = startValue.x + rx;
            value.y = startValue.y + ry;
        }
        else
            value.SetVec3(startValue.GetVec3());
    }
    else
    {
        for (int i = 0; i < _valueSize; i++)
        {
            float n1 = startValue[i];
            float n2 = endValue[i];
            float f = n1 + (n2 - n1) * _normalizedTime;
            if (_snapping)
                f = round(f);
            deltaValue[i] = f - value[i];
            value[i] = f;
        }
    }

    if (_refTarget != nullptr && _propType != TweenPropType::None)
    {
        GObject* gobj = dynamic_cast<GObject*>(_refTarget);
        if (gobj != nullptr)
            TweenPropTypeUtils::SetProps(gobj, _propType, value);
        else
        {
            Node* node = dynamic_cast<Node*>(_refTarget);
            if (node != nullptr)
                TweenPropTypeUtils::SetProps(node, _propType, value);
        }
    }

    CallUpdateCallback();
}

void GTweener::CallStartCallback()
{
    if (_onStart != nullptr)
        _onStart(this);
}

void GTweener::CallUpdateCallback()
{
    if (_onUpdate != nullptr)
        _onUpdate(this);
}

void GTweener::CallCompleteCallback()
{
    if (_onComplete != nullptr)
        _onComplete(this);
    if (_onComplete0 != nullptr)
        _onComplete0();
}

NS_FGUI_END