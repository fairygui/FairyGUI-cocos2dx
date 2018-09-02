#include "GTween.h"
#include "TweenManager.h"

NS_FGUI_BEGIN
USING_NS_CC;

GTweener* GTween::To(float startValue, float endValue, float duration)
{
    return TweenManager::CreateTween()->_To(startValue, endValue, duration);
}

GTweener* GTween::To(const cocos2d::Vec2& startValue, const cocos2d::Vec2 & endValue, float duration)
{
    return TweenManager::CreateTween()->_To(startValue, endValue, duration);
}

GTweener* GTween::To(const cocos2d::Vec3& startValue, const cocos2d::Vec3 & endValue, float duration)
{
    return TweenManager::CreateTween()->_To(startValue, endValue, duration);
}

GTweener* GTween::To(const cocos2d::Vec4& startValue, const cocos2d::Vec4 & endValue, float duration)
{
    return TweenManager::CreateTween()->_To(startValue, endValue, duration);
}

GTweener* GTween::To(const cocos2d::Color4B& startValue, const cocos2d::Color4B & endValue, float duration)
{
    return TweenManager::CreateTween()->_To(startValue, endValue, duration);
}

GTweener* GTween::ToDouble(double startValue, double endValue, float duration)
{
    return TweenManager::CreateTween()->_To(startValue, endValue, duration);
}

GTweener* GTween::DelayedCall(float delay)
{
    return TweenManager::CreateTween()->SetDelay(delay);
}

GTweener* GTween::Shake(const cocos2d::Vec2& startValue, float amplitude, float duration)
{
    return TweenManager::CreateTween()->_Shake(startValue, amplitude, duration);
}

bool GTween::IsTweening(cocos2d::Ref * target)
{
    return TweenManager::IsTweening(target, TweenPropType::None);
}

bool GTween::IsTweening(cocos2d::Ref * target, TweenPropType propType)
{
    return TweenManager::IsTweening(target, propType);
}

void GTween::Kill(cocos2d::Ref * target)
{
    TweenManager::KillTweens(target, TweenPropType::None, false);
}

void GTween::Kill(cocos2d::Ref * target, bool complete)
{
    TweenManager::KillTweens(target, TweenPropType::None, complete);
}

void GTween::Kill(cocos2d::Ref * target, TweenPropType propType, bool complete)
{
    TweenManager::KillTweens(target, propType, complete);
}

GTweener* GTween::GetTween(cocos2d::Ref * target)
{
    return TweenManager::GetTween(target, TweenPropType::None);
}

GTweener* GTween::GetTween(cocos2d::Ref * target, TweenPropType propType)
{
    return TweenManager::GetTween(target, propType);
}

void GTween::Clean()
{
    TweenManager::Clean();
}

NS_FGUI_END