#ifndef __TWEENVALUE_H__
#define __TWEENVALUE_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class TweenValue
{
public:
    float x;
    float y;
    float z;
    float w;
    double d;

    TweenValue();

    cocos2d::Vec2 GetVec2() const;
    void SetVec2(const cocos2d::Vec2& value);
    cocos2d::Vec3 GetVec3() const;
    void SetVec3(const cocos2d::Vec3& value);
    cocos2d::Vec4 GetVec4() const;
    void SetVec4(const cocos2d::Vec4& value);
    cocos2d::Color4B GetColor() const;
    void SetColor(const cocos2d::Color4B& value);
    float operator[] (int index) const;
    float& operator[] (int index);
    void SetZero();
};

NS_FGUI_END

#endif
