#ifndef __ACTIONS_H__
#define __ACTIONS_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

cocos2d::ActionInterval* createEaseAction(cocos2d::tweenfunc::TweenType tweenType, cocos2d::ActionInterval* action);

class ActionVec2 : public cocos2d::ActionInterval
{
public:
    typedef std::function<void(const cocos2d::Vec2& value)> ActionVec2Callback;
    static ActionVec2* create(float duration, const cocos2d::Vec2& from, const cocos2d::Vec2& to,
        ActionVec2Callback onUpdate, std::function<void()> onComplete);
    void startWithTarget(cocos2d::Node* target) override;
    void update(float delta) override;
    virtual void stop() override;
    ActionVec2* reverse() const override;
    ActionVec2* clone() const override;

CC_CONSTRUCTOR_ACCESS:
    ActionVec2() {};
    virtual ~ActionVec2() {};

    bool initWithDuration(float duration, const cocos2d::Vec2& from, const cocos2d::Vec2& to,
        ActionVec2Callback onUpdate, std::function<void()> onComplete);

protected:
    cocos2d::Vec2 _from;
    cocos2d::Vec2 _to;
    cocos2d::Vec2 _delta;
    ActionVec2Callback _updateCallback;
    std::function<void()> _completeCallback;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(ActionVec2);
};

class ActionVec4 : public cocos2d::ActionInterval
{
public:
    typedef std::function<void(const cocos2d::Vec4& value)> ActionVec4Callback;
    static ActionVec4* create(float duration, const cocos2d::Vec4& from, const cocos2d::Vec4& to,
        ActionVec4Callback onUpdate, std::function<void()> onComplete);
    void startWithTarget(cocos2d::Node* target) override;
    void update(float delta) override;
    virtual void stop() override;
    ActionVec4* reverse() const override;
    ActionVec4* clone() const override;

CC_CONSTRUCTOR_ACCESS:
    ActionVec4() {};
    virtual ~ActionVec4() {};

    bool initWithDuration(float duration, const cocos2d::Vec4& from, const cocos2d::Vec4& to,
        ActionVec4Callback onUpdate, std::function<void()> onComplete);

protected:
    cocos2d::Vec4 _from;
    cocos2d::Vec4 _to;
    cocos2d::Vec4 _delta;
    ActionVec4Callback _updateCallback;
    std::function<void()> _completeCallback;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(ActionVec4);
};

class ActionMovieClip : public cocos2d::Action
{
public:
    static ActionMovieClip* create(cocos2d::Animation *animation, float repeatDelay = 0);
    ActionMovieClip();
    ~ActionMovieClip();

    virtual bool isDone() const override;
    virtual void step(float dt) override;
    virtual void startWithTarget(cocos2d::Node *target) override;
    virtual ActionMovieClip* reverse() const override;
    virtual ActionMovieClip* clone() const override;

    int getCurrentFrame() { return _curFrame; }
    void setCurrentFrame(int value);

    void setPlaySettings(int start, int end, int times, int endAt, std::function<void()> completeCallback = nullptr);
    void setAnimation(cocos2d::Animation *animation, float repeatDelay = 0);

private:
    void drawFrame();

    cocos2d::Animation* _animation;
    int _curFrame;
    float _curFrameDelay;
    int _repeatedCount;

    float _repeatDelay;
    std::function<void()> _completeCallback;
    int _drawingFrame;

    int _start;
    int _end;
    int _times;
    int _endAt;
    int _status; //0-none, 1-next loop, 2-ending, 3-ended
};

NS_FGUI_END

#endif
