#ifndef __GEARBASE_H__
#define __GEARBASE_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class GObject;
class Controller;

class GearBase
{
public:
    GearBase(GObject* owner);
    ~GearBase();

    Controller* getController() const { return _controller; }
    void setController(Controller* value);

    virtual void updateFromRelations(float dx, float dy);
    virtual void apply();
    virtual void updateState();

    void setup(tinyxml2::XMLElement * xml);

    static bool disableAllTweenEffect;
    bool tween;
    cocos2d::tweenfunc::TweenType easeType;
    float tweenTime;
    float delay;

protected:
    enum ActionTag
    {
        XY_ACTION = 0xCC2100,
        SIZE_ACTION,
        LOOK_ACTION,
        COLOR_ACTION
    };
    virtual void addStatus(const std::string&  pageId, const std::string& value);
    virtual void init();

    GObject* _owner;
    Controller* _controller;
    uint32_t _displayLockToken;
};

NS_FGUI_END

#endif
