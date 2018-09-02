#ifndef __GTWEENER_H__
#define __GTWEENER_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "EaseType.h"
#include "TweenValue.h"
#include "TweenPropType.h"

NS_FGUI_BEGIN

class GTweener
{
public:
    typedef std::function<void(GTweener* tweener)> GTweenCallback;
    typedef std::function<void()> GTweenCallback0;

    GTweener();
    ~GTweener();
    GTweener* SetDelay(float value);
    float GetDelay() const { return _delay; }
    GTweener* SetDuration(float value);
    float GetDuration() const { return _duration; }
    GTweener* SetBreakpoint(float value);
    GTweener* SetEase(EaseType value);
    GTweener* SetEasePeriod(float value);
    GTweener* SetEaseOvershootOrAmplitude(float value);
    GTweener* SetRepeat(int repeat, bool yoyo = false);
    int GetRepeat() const { return _repeat; }
    GTweener* SetTimeScale(float value);
    GTweener* SetSnapping(bool value);
    GTweener* SetTargetAny(void* value);
    GTweener* SetTarget(cocos2d::Ref* target);
    GTweener* SetTarget(cocos2d::Ref* target, TweenPropType propType);
    void* GetTarget() const { return _target; }
    GTweener* SetUserData(const cocos2d::Value& value);
    const cocos2d::Value& GetUserData() const { return _userData; }
    GTweener* OnUpdate(GTweenCallback callback);
    GTweener* OnStart(GTweenCallback callback);
    GTweener* OnComplete(GTweenCallback callback);
    GTweener* OnComplete0(GTweenCallback0 callback);

    float GetNormalizedTime() const { return _normalizedTime; }
    bool isCompleted() const { return _ended != 0; }
    bool allCompleted() const { return _ended == 1; }
    GTweener* SetPaused(bool paused);
    void Seek(float time);
    void Kill(bool complete = false);

    TweenValue startValue;
    TweenValue endValue;
    TweenValue value;
    TweenValue deltaValue;

private:
    GTweener* _To(float start, float end, float duration);
    GTweener* _To(const cocos2d::Vec2& start, const cocos2d::Vec2& end, float duration);
    GTweener* _To(const cocos2d::Vec3& start, const cocos2d::Vec3& end, float duration);
    GTweener* _To(const cocos2d::Vec4& start, const cocos2d::Vec4& end, float duration);
    GTweener* _To(const cocos2d::Color4B& start, const cocos2d::Color4B& end, float duration);
    GTweener* _To(double start, double end, float duration);
    GTweener* _Shake(const cocos2d::Vec2& start, float amplitude, float duration);
    void _Init();
    void _Reset();
    void _Update(float dt);
    void Update();
    void CallStartCallback();
    void CallUpdateCallback();
    void CallCompleteCallback();

private:
    void* _target;
    cocos2d::Ref* _refTarget;
    TweenPropType _propType;
    bool _killed;
    bool _paused;

    float _delay;
    float _duration;
    float _breakpoint;
    EaseType _easeType;
    float _easeOvershootOrAmplitude;
    float _easePeriod;
    int _repeat;
    bool _yoyo;
    float _timeScale;
    bool _snapping;
    cocos2d::Value _userData;
    int _valueSize;

    GTweenCallback _onUpdate;
    GTweenCallback _onStart;
    GTweenCallback _onComplete;
    GTweenCallback0 _onComplete0;

    bool _started;
    int _ended;
    float _elapsedTime;
    float _normalizedTime;

    friend class GTween;
    friend class TweenManager;
};

NS_FGUI_END

#endif
