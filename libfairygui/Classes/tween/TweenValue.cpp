#include "TweenValue.h"

NS_FGUI_BEGIN
USING_NS_CC;

TweenValue::TweenValue():x(0),y(0),z(0),w(0),d(0)
{
}

cocos2d::Vec2 TweenValue::GetVec2() const
{
    return cocos2d::Vec2(x, y);
}

void TweenValue::SetVec2(const cocos2d::Vec2 & value)
{
    x = value.x;
    y = value.y;
}

cocos2d::Vec3 TweenValue::GetVec3() const
{
    return cocos2d::Vec3(x, y, z);
}

void TweenValue::SetVec3(const cocos2d::Vec3 & value)
{
    x = value.x;
    y = value.y;
    z = value.z;
}

cocos2d::Vec4 TweenValue::GetVec4() const
{
    return cocos2d::Vec4(x, y, z, w);
}

void TweenValue::SetVec4(const cocos2d::Vec4 & value)
{
    x = value.x;
    y = value.y;
    z = value.z;
    w = value.w;
}

cocos2d::Color4B TweenValue::GetColor() const
{
    return cocos2d::Color4B(x, y, z, w);
}

void TweenValue::SetColor(const cocos2d::Color4B & value)
{
    x = value.r;
    y = value.g;
    z = value.b;
    w = value.a;
}

float TweenValue::operator[](int index) const
{
    switch (index)
    {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    case 3:
        return w;
    default:
        throw "Index out of bounds: " + index;
    }
}

float & TweenValue::operator[](int index)
{
    switch (index)
    {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    case 3:
        return w;
    default:
        throw "Index out of bounds: " + index;
    }
}

void TweenValue::SetZero()
{
    x = y = z = w = d = 0;
}

NS_FGUI_END