#ifndef __GEARCOLOR_H__
#define __GEARCOLOR_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "GearBase.h"

NS_FGUI_BEGIN

class GObject;

class IColorGear
{
public:
    virtual cocos2d::Color4B cg_getColor() const = 0;
    virtual void cg_setColor(const cocos2d::Color4B& value) = 0;

    virtual cocos2d::Color4B cg_getOutlineColor() const { return cocos2d::Color4B(0, 0, 0, 0); };
    virtual void cg_setOutlineColor(const cocos2d::Color4B& value) {};
};

class GearColor : public GearBase
{
public:
    GearColor(GObject* owner);
    ~GearColor();

    void apply() override;
    void updateState() override;

protected:
    void addStatus(const std::string&  pageId, const std::string& value) override;
    void init() override;

private:
    void onTweenUpdate(const cocos2d::Vec4& v);
    void onTweenComplete();

    class GearColorValue
    {
    public:
        cocos2d::Color4B color;
        cocos2d::Color4B outlineColor;

        GearColorValue();
        GearColorValue(const cocos2d::Color4B& color, const cocos2d::Color4B& outlineColor);
    };

    std::unordered_map<std::string, GearColorValue> _storage;
    GearColorValue _default;
    cocos2d::Vec4 _tweenTarget;
};

NS_FGUI_END

#endif
