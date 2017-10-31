#ifndef __GEARANIMATION_H__
#define __GEARANIMATION_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "GearBase.h"

NS_FGUI_BEGIN

class IAnimationGear
{
public:
    virtual bool isPlaying() const = 0;
    virtual void setPlaying(bool value) = 0;

    virtual int getCurrentFrame() const = 0;
    virtual void setCurrentFrame(int value) = 0;
};

class GObject;

class GearAnimation : public GearBase
{
public:
    GearAnimation(GObject* owner);
    ~GearAnimation();

    void apply() override;
    void updateState() override;

protected:
    void addStatus(const std::string&  pageId, const std::string& value) override;
    void init() override;

private:
    class GearAnimationValue
    {
    public:
        bool playing;
        int frame;

        GearAnimationValue();
        GearAnimationValue(bool playing, int frame);
    };
    std::unordered_map<std::string, GearAnimationValue> _storage;
    GearAnimationValue _default;
};

NS_FGUI_END

#endif
