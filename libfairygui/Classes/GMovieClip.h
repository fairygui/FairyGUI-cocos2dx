#ifndef __GMOVIECLIP_H__
#define __GMOVIECLIP_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GObject.h"
#include "display/Actions.h"

NS_FGUI_BEGIN

class GMovieClip : public GObject, public IColorGear, public IAnimationGear
{
public:
    GMovieClip();
    ~GMovieClip();

    CREATE_FUNC(GMovieClip);

    bool isPlaying() const override { return _playing; }
    void setPlaying(bool value) override;

    int getCurrentFrame() const override { return _playAction->getCurrentFrame(); }
    void setCurrentFrame(int value) override;

    FlipType getFlip() const;
    void setFlip(FlipType value);

    const cocos2d::Color3B& getColor() const { return _content->getColor(); }
    void setColor(const cocos2d::Color3B& value);

    //from start to end(-1 means ending)£¬repeat times(0 means infinite loop)£¬when all is over, stopping at endAt(-1 means same value of end)
    void setPlaySettings(int start = 0, int end = -1, int times = 0, int endAt = -1, std::function<void()> completeCallback = nullptr);

    virtual void constructFromResource() override;

    cocos2d::Color4B cg_getColor() const override;
    void cg_setColor(const cocos2d::Color4B& value) override;

protected:
    virtual void handleInit() override;
    virtual void setup_BeforeAdd(TXMLElement* xml) override;

private:
    cocos2d::Sprite* _content;
    ActionMovieClip* _playAction;
    bool _playing;
};

NS_FGUI_END

#endif
