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

    //从start帧开始，播放到end帧（ - 1表示结尾），重复times次（0表示无限循环），循环结束后，停止在endAt帧（ - 1表示参数end）
    void setPlaySettings(int start = 0, int end = -1, int times = 0, int endAt = -1, std::function<void()> completeCallback = nullptr);

    virtual void constructFromResource() override;

    cocos2d::Color4B cg_getColor() const override;
    void cg_setColor(const cocos2d::Color4B& value) override;

protected:
    virtual void handleInit() override;
    virtual void setup_BeforeAdd(tinyxml2::XMLElement* xml) override;

private:
    cocos2d::Sprite* _content;
    ActionMovieClip* _playAction;
    bool _playing;
};

NS_FGUI_END

#endif
