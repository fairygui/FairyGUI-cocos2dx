#ifndef __GEARICON_H__
#define __GEARICON_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "GearBase.h"

NS_FGUI_BEGIN

class GObject;

class GearIcon : public GearBase
{
public:
    GearIcon(GObject* owner);
    ~GearIcon();

    void apply() override;
    void updateState() override;

protected:
    void addStatus(const std::string&  pageId, const std::string& value) override;
    void init() override;

private:
    std::unordered_map<std::string, cocos2d::Vec2> _storage;
    cocos2d::Vec2 _default;
};

NS_FGUI_END

#endif // __GEARXY_H__
