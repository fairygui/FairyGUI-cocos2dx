#ifndef __GEARSIZE_H__
#define __GEARSIZE_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "GearBase.h"

NS_FGUI_BEGIN

class GObject;

class GearSize : public GearBase
{
public:
    GearSize(GObject* owner);
    virtual ~GearSize();

    void apply() override;
    void updateState() override;
    void updateFromRelations(float dx, float dy) override;

protected:
    void addStatus(const std::string&  pageId, const std::string& value) override;
    void init() override;

private:
    void onTweenUpdate(const cocos2d::Vec4& v, bool a, bool b);
    void onTweenComplete();

    std::unordered_map<std::string, cocos2d::Vec4> _storage;
    cocos2d::Vec4 _default;
    cocos2d::Vec4 _tweenTarget;
};

NS_FGUI_END

#endif
