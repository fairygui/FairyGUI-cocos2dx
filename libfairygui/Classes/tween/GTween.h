#ifndef __GTWEEN_H__
#define __GTWEEN_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "GTweener.h"
#include "EaseType.h"
#include "TweenValue.h"
#include "TweenPropType.h"

NS_FGUI_BEGIN

class GTween
{
public:
    static GTweener* To(float startValue, float endValue, float duration);
    static GTweener* To(const cocos2d::Vec2& startValue, const cocos2d::Vec2& endValue, float duration);
    static GTweener* To(const cocos2d::Vec3& startValue, const cocos2d::Vec3& endValue, float duration);
    static GTweener* To(const cocos2d::Vec4& startValue, const cocos2d::Vec4& endValue, float duration);
    static GTweener* To(const cocos2d::Color4B& startValue, const cocos2d::Color4B& endValue, float duration);
    static GTweener* ToDouble(double startValue, double endValue, float duration);
    static GTweener* DelayedCall(float delay);
    static GTweener* Shake(const cocos2d::Vec2& startValue, float amplitude, float duration);
    static bool IsTweening(cocos2d::Ref* target);
    static bool IsTweening(cocos2d::Ref* target, TweenPropType propType);
    static void Kill(cocos2d::Ref* target);
    static void Kill(cocos2d::Ref* target, bool complete);
    static void Kill(cocos2d::Ref* target, TweenPropType propType, bool complete);
    static GTweener* GetTween(cocos2d::Ref* target);
    static GTweener* GetTween(cocos2d::Ref* target, TweenPropType propType);
    static void Clean();
};

NS_FGUI_END

#endif
