#ifndef __ACTIONS_H__
#define __ACTIONS_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

cocos2d::ActionInterval* createEaseAction(cocos2d::tweenfunc::TweenType tweenType, cocos2d::ActionInterval* action);
cocos2d::ActionInterval* composeActions(cocos2d::ActionInterval* mainAction, cocos2d::tweenfunc::TweenType easeType, float delay, std::function<void()> func, int tag = cocos2d::Action::INVALID_TAG);

enum ActionTag
{
    GEAR_XY_ACTION = 0xCC2100,
    GEAR_SIZE_ACTION,
    GEAR_LOOK_ACTION,
    GEAR_COLOR_ACTION,
    TRANSITION_ACTION
};

class ActionFloat2 : public cocos2d::ActionInterval
{
public:
    typedef std::function<void(float x, float y)> ActionFloat2Callback;
    static ActionFloat2* create(float duration, const cocos2d::Vec2& from, const cocos2d::Vec2& to, ActionFloat2Callback onUpdate);
    void startWithTarget(cocos2d::Node* target) override;
    void update(float delta) override;
    ActionFloat2* reverse() const override;
    ActionFloat2* clone() const override;

CC_CONSTRUCTOR_ACCESS:
    ActionFloat2() {};
    virtual ~ActionFloat2() {};

    bool initWithDuration(float duration, const cocos2d::Vec2& from, const cocos2d::Vec2& to, ActionFloat2Callback onUpdate);

protected:
    cocos2d::Vec2 _from;
    cocos2d::Vec2 _to;
    cocos2d::Vec2 _delta;
    ActionFloat2Callback _updateCallback;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(ActionFloat2);
};

class ActionVec2 : public cocos2d::ActionInterval
{
public:
    typedef std::function<void(const cocos2d::Vec2& value)> ActionVec2Callback;
    static ActionVec2* create(float duration, const cocos2d::Vec2& from, const cocos2d::Vec2& to, ActionVec2Callback onUpdate);
    void startWithTarget(cocos2d::Node* target) override;
    void update(float delta) override;
    ActionVec2* reverse() const override;
    ActionVec2* clone() const override;

CC_CONSTRUCTOR_ACCESS:
    ActionVec2() {};
    virtual ~ActionVec2() {};

    bool initWithDuration(float duration, const cocos2d::Vec2& from, const cocos2d::Vec2& to, ActionVec2Callback onUpdate);

protected:
    cocos2d::Vec2 _from;
    cocos2d::Vec2 _to;
    cocos2d::Vec2 _delta;
    ActionVec2Callback _updateCallback;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(ActionVec2);
};

class ActionVec4 : public cocos2d::ActionInterval
{
public:
    typedef std::function<void(const cocos2d::Vec4& value)> ActionVec4Callback;
    static ActionVec4* create(float duration, const cocos2d::Vec4& from, const cocos2d::Vec4& to, ActionVec4Callback onUpdate);
    void startWithTarget(cocos2d::Node* target) override;
    void update(float delta) override;
    ActionVec4* reverse() const override;
    ActionVec4* clone() const override;

CC_CONSTRUCTOR_ACCESS:
    ActionVec4() {};
    virtual ~ActionVec4() {};

    bool initWithDuration(float duration, const cocos2d::Vec4& from, const cocos2d::Vec4& to, ActionVec4Callback onUpdate);

protected:
    cocos2d::Vec4 _from;
    cocos2d::Vec4 _to;
    cocos2d::Vec4 _delta;
    ActionVec4Callback _updateCallback;

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

    CC_DISALLOW_COPY_AND_ASSIGN(ActionMovieClip);
};

class RepeatYoyo : public cocos2d::Repeat
{
public:
    static RepeatYoyo* create(cocos2d::FiniteTimeAction *action, unsigned int times, bool yoyo);
    virtual RepeatYoyo* clone() const override;
    virtual RepeatYoyo* reverse() const override;
    virtual void update(float dt) override;

    RepeatYoyo() {}

protected:
    bool _yoyo;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(RepeatYoyo);
};

NS_FGUI_END

#endif
