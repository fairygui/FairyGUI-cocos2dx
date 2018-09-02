#ifndef __GEARCOLOR_H__
#define __GEARCOLOR_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "GearBase.h"
#include "tween/GTween.h"

NS_FGUI_BEGIN

class GObject;

class IColorGear
{
public:
    virtual cocos2d::Color3B getColor() const = 0;
    virtual void setColor(const cocos2d::Color3B& value) = 0;
};

class GearColor : public GearBase
{
public:
    GearColor(GObject* owner);
    virtual ~GearColor();

    void apply() override;
    void updateState() override;

protected:
    void addStatus(const std::string&  pageId, const std::string& value) override;
    void init() override;

private:
    void onTweenUpdate(GTweener* tweener);
    void onTweenComplete(GTweener* tweener);

    class GearColorValue
    {
    public:
        cocos2d::Color3B color;
        cocos2d::Color3B outlineColor;

        GearColorValue();
        GearColorValue(const cocos2d::Color3B& color, const cocos2d::Color3B& outlineColor);
    };

    std::unordered_map<std::string, GearColorValue> _storage;
    GearColorValue _default;
    GTweener* _tweener;
};

NS_FGUI_END

#endif
