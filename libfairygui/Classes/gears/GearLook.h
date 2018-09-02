#ifndef __GEARLOOK_H__
#define __GEARLOOK_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "GearBase.h"
#include "tween/GTween.h"

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
    void onTweenUpdate(GTweener* tweener);
    void onTweenComplete(GTweener* tweener);

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
    GTweener* _tweener;
};

NS_FGUI_END

#endif
