#ifndef __GEARBASE_H__
#define __GEARBASE_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "tinyxml2/tinyxml2.h"

NS_FGUI_BEGIN

class GObject;
class Controller;

class GearBase
{
public:
    GearBase(GObject* owner);
    ~GearBase();

    Controller* getController() { return _controller; }
    void setController(Controller* value);

    virtual void updateFromRelations(float dx, float dy);
    virtual void apply();
    virtual void updateState();

    void setup(tinyxml2::XMLElement * xml);

    static bool disableAllTweenEffect;
    bool tween;
    //Ease easeType;
    float tweenTime;
    float delay;

protected:
    virtual void addStatus(const std::string&  pageId, const std::string& value);
    virtual void init();

    GObject* _owner;
    Controller* _controller;
    uint32_t _displayLockToken;
};

NS_FGUI_END

#endif // __GEARBASE_H__
