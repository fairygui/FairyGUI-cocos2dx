#ifndef __GEARDISPLAY_H__
#define __GEARDISPLAY_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "GearBase.h"

NS_FGUI_BEGIN

class GObject;

class GearDisplay : public GearBase
{
public:
    GearDisplay(GObject* owner);
    ~GearDisplay();

    void apply() override;
    void updateState() override;

    uint32_t addLock();
    void releaseLock(uint32_t token);
    bool isConnected();

    std::vector<std::string> pages;

protected:
    void addStatus(const std::string&  pageId, const std::string& value) override;
    void init() override;

private:
    int _visible;
};

NS_FGUI_END

#endif // __GEARDISPLAY_H__
