#ifndef __GEARLOOK_H__
#define __GEARLOOK_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "GearBase.h"

NS_FGUI_BEGIN

class GObject;

class GearLook : public GearBase
{
public:
    GearLook(GObject* owner);
    virtual ~GearLook();

    void apply() override;
    void updateState() override;

protected:
    void addStatus(const std::string&  pageId, const std::string& value) override;
    void init() override;

private:
    void onTweenUpdate(const cocos2d::Vec2& v, bool a, bool b);
    void onTweenComplete();

    class GearLookValue
    {
    public:
        float alpha;
        float rotation;
        bool grayed;
        bool touchable;

        GearLookValue();
        GearLookValue(float alpha, float rotation, bool grayed, bool touchable);
    };

    std::unordered_map<std::string, GearLookValue> _storage;
    GearLookValue _default;
    cocos2d::Vec2 _tweenTarget;
};

NS_FGUI_END

#endif
