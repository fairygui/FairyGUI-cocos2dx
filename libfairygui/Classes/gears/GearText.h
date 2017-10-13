#ifndef __GEARTEXT_H__
#define __GEARTEXT_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "GearBase.h"

NS_FGUI_BEGIN

class GObject;

class GearText : public GearBase
{
public:
    GearText(GObject* owner);
    ~GearText();

    void apply() override;
    void updateState() override;

protected:
    void addStatus(const std::string&  pageId, const std::string& value) override;
    void init() override;

private:
    std::unordered_map<std::string, cocos2d::Vec2> _storage;
    cocos2d::Vec2 _default;
    cocos2d::Vec2 _tweenTarget;
};

NS_FGUI_END

#endif // __GEARTEXT_H__
