#include "Transition.h"
#include "GComponent.h"
#include "GRoot.h"
#include "tween/GTween.h"
#include "utils/ByteBuffer.h"

NS_FGUI_BEGIN
USING_NS_CC;
using namespace std;

const int OPTION_IGNORE_DISPLAY_CONTROLLER = 1;
const int OPTION_AUTO_STOP_DISABLED = 2;
const int OPTION_AUTO_STOP_AT_END = 4;

class TValue_Visible
{
public:
    bool visible;
};

class TValue_Animation
{
public:
    int frame;
    bool playing;
    bool flag;
};

class TValue_Sound
{
public:
    std::string sound;
    float volume;
};

class TValue_Transition
{
public:
    std::string transName;
    int playTimes;
    Transition* trans;
    float stopTime;
};

class TValue_Shake
{
public:
    float amplitude;
    float duration;
    cocos2d::Vec2 lastOffset;
    cocos2d::Vec2 offset;
};

class TValue_Text
{
public:
    std::string text;
};

class TValue
{
public:
    float f1;
    float f2;
    float f3;
    float f4;
    bool b1;
    bool b2;

    TValue();
    cocos2d::Vec2 getVec2() const;
    void setVec2(const cocos2d::Vec2& value);
    cocos2d::Vec4 getVec4() const;
    void setVec4(const cocos2d::Vec4& value);
    cocos2d::Color4B getColor() const;
    void setColor(const cocos2d::Color4B& value);
};

TValue::TValue()
{
    f1 = f2 = f3 = f4 = 0;
    b1 = b2 = true;
}

cocos2d::Vec2 TValue::getVec2() const
{
    return cocos2d::Vec2(f1, f2);
}

void TValue::setVec2(const cocos2d::Vec2 & value)
{
    f1 = value.x;
    f2 = value.y;
}

cocos2d::Vec4 TValue::getVec4() const
{
    return cocos2d::Vec4(f1, f2, f3, f4);
}

void TValue::setVec4(const cocos2d::Vec4 & value)
{
    f1 = value.x;
    f2 = value.y;
    f3 = value.z;
    f4 = value.w;
}

cocos2d::Color4B TValue::getColor() const
{
    return cocos2d::Color4B(f1, f2, f3, f4);
}

void TValue::setColor(const cocos2d::Color4B & value)
{
    f1 = value.r;
    f2 = value.g;
    f3 = value.b;
    f4 = value.a;
}

class TweenConfig
{
public:
    float duration;
    EaseType easeType;
    int repeat;
    bool yoyo;

    TValue* startValue;
    TValue* endValue;

    string endLabel;
    Transition::TransitionHook endHook;

    TweenConfig();
};

TweenConfig::TweenConfig()
{
    easeType = EaseType::QuadOut;
    startValue = new TValue();
    endValue = new TValue();
}

class TransitionItem
{
public:
    float time;
    string targetId;
    TransitionActionType type;
    TweenConfig* tweenConfig;
    string label;
    void* value;
    Transition::TransitionHook hook;

    //running properties
    GTweener* tweener;
    GObject* target;
    uint32_t displayLockToken;

    TransitionItem(TransitionActionType aType);
};

TransitionItem::TransitionItem(TransitionActionType aType) :
    time(0),
    type(aType),
    hook(nullptr),
    tweenConfig(nullptr),
    tweener(nullptr),
    target(nullptr),
    displayLockToken(0)
{
    switch (type)
    {
    case TransitionActionType::XY:
    case TransitionActionType::Size:
    case TransitionActionType::Scale:
    case TransitionActionType::Pivot:
    case TransitionActionType::Skew:
    case TransitionActionType::Alpha:
    case TransitionActionType::Rotation:
    case TransitionActionType::Color:
    case TransitionActionType::ColorFilter:
        value = new TValue();
        break;

    case TransitionActionType::Animation:
        value = new TValue_Animation();
        break;

    case TransitionActionType::Shake:
        value = new TValue_Shake();
        break;

    case TransitionActionType::Sound:
        value = new TValue_Sound();
        break;

    case TransitionActionType::Transition:
        value = new TValue_Transition();
        break;

    case TransitionActionType::Visible:
        value = new TValue_Visible();
        break;

    case TransitionActionType::Text:
    case TransitionActionType::Icon:
        value = new TValue_Text();
        break;

    default:
        value = nullptr;
        break;
    }
}

Transition::Transition(GComponent* owner) :
    _owner(owner),
    _totalTimes(0),
    _totalTasks(0),
    _playing(false),
    _paused(false),
    _ownerBaseX(0),
    _ownerBaseY(0),
    _onComplete(nullptr),
    _options(0),
    _reversed(false),
    _totalDuration(0),
    _autoPlay(false),
    _autoPlayDelay(0),
    _timeScale(1),
    _startTime(0),
    _endTime(0)
{
}

Transition::~Transition()
{
    if (_playing)
        GTween::kill(this);//delay start

    for (auto &item : _items)
    {
        if (item->tweener != nullptr)
        {
            item->tweener->kill();
            item->tweener = nullptr;
        }

        item->target = nullptr;
        item->hook = nullptr;
        if (item->tweenConfig != nullptr)
            item->tweenConfig->endHook = nullptr;
    }

    _playing = false;
    _onComplete = nullptr;
}

void Transition::play(PlayCompleteCallback callback)
{
    play(1, 0, 0, -1, callback, false);
}

void Transition::play(int times, float delay, PlayCompleteCallback callback)
{
    play(times, delay, 0, -1, callback, false);
}

void Transition::play(int times, float delay, float startTime, float endTime, PlayCompleteCallback callback)
{
    play(times, delay, startTime, endTime, callback, false);
}

void Transition::playReverse(PlayCompleteCallback callback)
{
    play(1, 0, 0, -1, callback, true);
}

void Transition::playReverse(int times, float delay, PlayCompleteCallback callback)
{
    play(times, delay, 0, -1, callback, true);
}

void Transition::play(int times, float delay, float startTime, float endTime, PlayCompleteCallback onComplete, bool reverse)
{
    stop(true, true);

    _totalTimes = times;
    _reversed = reverse;
    _startTime = startTime;
    _endTime = endTime;
    _playing = true;
    _paused = false;
    _onComplete = onComplete;

    int cnt = (int)_items.size();
    for (int i = 0; i < cnt; i++)
    {
        TransitionItem* item = _items[i];
        if (item->target == nullptr)
        {
            if (!item->targetId.empty())
                item->target = _owner->getChildById(item->targetId);
            else
                item->target = _owner;
        }
        else if (item->target != _owner && item->target->getParent() != _owner) //maybe removed
            item->target = nullptr;

        if (item->target != nullptr && item->type == TransitionActionType::Transition)
        {
            TValue_Transition* value = (TValue_Transition*)item->value;
            Transition* trans = dynamic_cast<GComponent*>(item->target)->getTransition(value->transName);
            if (trans == this)
                trans = nullptr;
            if (trans != nullptr)
            {
                if (value->playTimes == 0) //stop
                {
                    int j;
                    for (j = i - 1; j >= 0; j--)
                    {
                        TransitionItem* item2 = _items[j];
                        if (item2->type == TransitionActionType::Transition)
                        {
                            TValue_Transition* value2 = (TValue_Transition*)item2->value;
                            if (value2->trans == trans)
                            {
                                value2->stopTime = item->time - item2->time;
                                break;
                            }
                        }
                    }
                    if (j < 0)
                        value->stopTime = 0;
                    else
                        trans = nullptr; //no need to handle stop anymore
                }
                else
                    value->stopTime = -1;
            }
            value->trans = trans;
        }
    }

    if (delay == 0)
        onDelayedPlay();
    else
        GTween::delayedCall(delay)->setTarget(this)->onComplete(CC_CALLBACK_0(Transition::onDelayedPlay, this));
}

void Transition::changePlayTimes(int value)
{
    _totalTimes = value;
}

void Transition::setAutoPlay(bool autoPlay, int times, float delay)
{
    if (_autoPlay != autoPlay)
    {
        _autoPlay = autoPlay;
        _autoPlayTimes = times;
        _autoPlayDelay = delay;
        if (_autoPlay)
        {
            if (_owner->onStage())
                play(_autoPlayTimes, _autoPlayDelay, nullptr);
        }
        else
        {
            if (!_owner->onStage())
                stop(false, true);
        }
    }
}

void Transition::stop()
{
    stop(true, false);
}

void Transition::stop(bool setToComplete, bool processCallback)
{
    if (!_playing)
        return;

    _playing = false;
    _totalTasks = 0;
    _totalTimes = 0;
    PlayCompleteCallback func = _onComplete;
    _onComplete = nullptr;

    int cnt = (int)_items.size();
    if (_reversed)
    {
        for (int i = cnt - 1; i >= 0; i--)
        {
            TransitionItem* item = _items[i];
            if (item->target == nullptr)
                continue;

            stopItem(item, setToComplete);
        }
    }
    else
    {
        for (int i = 0; i < cnt; i++)
        {
            TransitionItem *item = _items[i];
            if (item->target == nullptr)
                continue;

            stopItem(item, setToComplete);
        }
    }
    if (processCallback && func != nullptr)
        func();
}

void Transition::stopItem(TransitionItem * item, bool setToComplete)
{
    if (item->displayLockToken != 0)
    {
        item->target->releaseDisplayLock(item->displayLockToken);
        item->displayLockToken = 0;
    }

    if (item->tweener != nullptr)
    {
        item->tweener->kill(setToComplete);
        item->tweener = nullptr;

        if (item->type == TransitionActionType::Shake && !setToComplete) //震动必须归位，否则下次就越震越远了。
        {
            item->target->_gearLocked = true;
            item->target->setPosition(item->target->getX() - ((TValue_Shake*)item->value)->lastOffset.x, item->target->getY() - ((TValue_Shake*)item->value)->lastOffset.y);
            item->target->_gearLocked = false;
        }
    }
}

void Transition::setPaused(bool paused)
{
    if (!_playing || _paused == paused)
        return;

    _paused = paused;
    GTweener* tweener = GTween::getTween(this);
    if (tweener != nullptr)
        tweener->setPaused(paused);

    for (auto &item : _items)
    {
        if (item->target == nullptr)
            continue;

        if (item->type == TransitionActionType::Transition)
        {
            if (((TValue_Transition*)item->value)->trans != nullptr)
                ((TValue_Transition*)item->value)->trans->setPaused(paused);
        }
        else if (item->type == TransitionActionType::Animation)
        {
            if (paused)
            {
                ((TValue_Animation*)item->value)->flag = (dynamic_cast<IAnimationGear*>(item->target))->isPlaying();
                (dynamic_cast<IAnimationGear*>(item->target))->setPlaying(false);
            }
            else
                (dynamic_cast<IAnimationGear*>(item->target))->setPlaying(((TValue_Animation*)item->value)->flag);
        }

        if (item->tweener != nullptr)
            item->tweener->setPaused(paused);
    }
}

void Transition::setValue(const std::string & label, const ValueVector& values)
{
    for (auto &item : _items)
    {
        void* value;
        if (item->label == label)
        {
            if (item->tweenConfig != nullptr)
                value = item->tweenConfig->startValue;
            else
                value = item->value;
        }
        else if (item->tweenConfig != nullptr && item->tweenConfig->endLabel == label)
        {
            value = item->tweenConfig->endValue;
        }
        else
            continue;

        switch (item->type)
        {
        case TransitionActionType::XY:
        case TransitionActionType::Size:
        case TransitionActionType::Pivot:
        case TransitionActionType::Scale:
        case TransitionActionType::Skew:
        {
            TValue* tvalue = (TValue*)value;
            tvalue->b1 = true;
            tvalue->b2 = true;
            tvalue->f1 = values[0].asFloat();
            tvalue->f2 = values[1].asFloat();
            break;
        }

        case TransitionActionType::Alpha:
        case TransitionActionType::Rotation:
            ((TValue*)value)->f1 = values[0].asFloat();;
            break;

        case TransitionActionType::Color:
        {
            uint32_t v = values[0].asUnsignedInt();
            ((TValue*)value)->setColor(Color4B((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF, (v >> 24) & 0xFF));
            break;
        }

        case TransitionActionType::Animation:
        {
            TValue_Animation* tvalue = (TValue_Animation*)value;
            tvalue->frame = values[0].asInt();
            if (values.size() > 1)
                tvalue->playing = values[1].asBool();
            break;
        }

        case TransitionActionType::Visible:
            ((TValue_Visible*)value)->visible = values[0].asBool();
            break;

        case TransitionActionType::Sound:
        {
            TValue_Sound* tvalue = (TValue_Sound*)value;
            tvalue->sound = values[0].asString();
            if (values.size() > 1)
                tvalue->volume = values[1].asFloat();
            break;
        }

        case TransitionActionType::Transition:
        {
            TValue_Transition* tvalue = (TValue_Transition*)value;
            tvalue->transName = values[0].asString();
            if (values.size() > 1)
                tvalue->playTimes = values[1].asInt();
            break;
        }

        case TransitionActionType::Shake:
        {
            ((TValue_Shake*)value)->amplitude = values[0].asFloat();
            if (values.size() > 1)
                ((TValue_Shake*)value)->duration = values[1].asFloat();
            break;
        }

        case TransitionActionType::ColorFilter:
        {
            TValue* tvalue = (TValue*)value;
            tvalue->f1 = values[0].asFloat();
            tvalue->f2 = values[1].asFloat();
            tvalue->f3 = values[2].asFloat();
            tvalue->f4 = values[3].asFloat();
            break;
        }

        case TransitionActionType::Text:
        case TransitionActionType::Icon:
            ((TValue_Text*)value)->text = values[0].asString();
            break;
        default:
            break;
        }
    }
}

void Transition::setHook(const std::string & label, TransitionHook callback)
{
    for (auto &item : _items)
    {
        if (item->label == label)
        {
            item->hook = callback;
            break;
        }
        else if (item->tweenConfig != nullptr && item->tweenConfig->endLabel == label)
        {
            item->tweenConfig->endHook = callback;
            break;
        }
    }
}

void Transition::clearHooks()
{
    for (auto &item : _items)
    {
        item->hook = nullptr;
        if (item->tweenConfig != nullptr)
            item->tweenConfig->endHook = nullptr;
    }
}

void Transition::setTarget(const std::string & label, GObject * newTarget)
{
    for (auto &item : _items)
    {
        if (item->label == label)
        {

            item->targetId = newTarget->id;
            item->target = nullptr;
        }
    }
}

void Transition::setDuration(const std::string & label, float value)
{
    for (auto &item : _items)
    {
        if (item->tweenConfig != nullptr && item->label == label)
            item->tweenConfig->duration = value;
    }
}

float Transition::getLabelTime(const string& label) const
{
    for (auto &item : _items)
    {
        if (item->label == label)
            return item->time;
        else if (item->tweenConfig != nullptr && item->label == label)
            return item->time + item->tweenConfig->duration;
    }

    return NAN;
}

void Transition::setTimeScale(float value)
{
    if (_timeScale != value)
    {
        _timeScale = value;

        for (auto &item : _items)
        {
            if (item->tweener != nullptr)
                item->tweener->setTimeScale(value);
            else if (item->type == TransitionActionType::Transition)
            {
                if (((TValue_Transition*)item->value)->trans != nullptr)
                    ((TValue_Transition*)item->value)->trans->setTimeScale(value);
            }
            else if (item->type == TransitionActionType::Animation)
            {
                if (item->target != nullptr)
                    (dynamic_cast<IAnimationGear*>(item->target))->setTimeScale(value);
            }
        }
    }
}

void Transition::updateFromRelations(const std::string & targetId, float dx, float dy)
{
    int cnt = (int)_items.size();
    if (cnt == 0)
        return;

    for (auto &item : _items)
    {
        if (item->type == TransitionActionType::XY && item->targetId == targetId)
        {
            if (item->tweenConfig != nullptr)
            {
                item->tweenConfig->startValue->f1 += dx;
                item->tweenConfig->startValue->f2 += dy;
                item->tweenConfig->endValue->f1 += dx;
                item->tweenConfig->endValue->f2 += dy;
            }
            else
            {
                ((TValue*)item->value)->f1 += dx;
                ((TValue*)item->value)->f2 += dy;
            }
        }
    }
}

void Transition::onOwnerAddedToStage()
{
    if (_autoPlay && !_playing)
        play(_autoPlayTimes, _autoPlayDelay, nullptr);
}

void Transition::onOwnerRemovedFromStage()
{
    if ((_options & OPTION_AUTO_STOP_DISABLED) == 0)
        stop((_options & OPTION_AUTO_STOP_AT_END) != 0 ? true : false, false);
}

void Transition::onDelayedPlay()
{
    internalPlay();

    _playing = _totalTasks > 0;
    if (_playing)
    {
        if ((_options & OPTION_IGNORE_DISPLAY_CONTROLLER) != 0)
        {
            for (auto &item : _items)
            {
                if (item->target != nullptr && item->target != _owner)
                    item->displayLockToken = item->target->addDisplayLock();
            }
        }
    }
    else if (_onComplete != nullptr)
    {
        PlayCompleteCallback func = _onComplete;
        _onComplete = nullptr;
        func();
    }
}

void Transition::internalPlay()
{
    _ownerBaseX = _owner->getX();
    _ownerBaseY = _owner->getY();

    _totalTasks = 0;

    bool needSkipAnimations = false;
    int cnt = (int)_items.size();
    if (!_reversed)
    {
        for (int i = 0; i < cnt; i++)
        {
            TransitionItem* item = _items[i];
            if (item->target == nullptr)
                continue;

            if (item->type == TransitionActionType::Animation && _startTime != 0 && item->time <= _startTime)
            {
                needSkipAnimations = true;
                ((TValue_Animation*)item->value)->flag = false;
            }
            else
                playItem(item);
        }
    }
    else
    {
        for (int i = cnt - 1; i >= 0; i--)
        {
            TransitionItem* item = _items[i];
            if (item->target == nullptr)
                continue;

            playItem(item);
        }
    }

    if (needSkipAnimations)
        skipAnimations();
}

void Transition::playItem(TransitionItem* item)
{
    float time;
    if (item->tweenConfig != nullptr)
    {
        if (_reversed)
            time = (_totalDuration - item->time - item->tweenConfig->duration);
        else
            time = item->time;

        if (_endTime == -1 || time <= _endTime)
        {
            TValue* startValue;
            TValue* endValue;

            if (_reversed)
            {
                startValue = item->tweenConfig->endValue;
                endValue = item->tweenConfig->startValue;
            }
            else
            {
                startValue = item->tweenConfig->startValue;
                endValue = item->tweenConfig->endValue;
            }

            ((TValue*)item->value)->b1 = startValue->b1 || endValue->b1;
            ((TValue*)item->value)->b2 = startValue->b2 || endValue->b2;

            switch (item->type)
            {
            case TransitionActionType::XY:
            case TransitionActionType::Size:
            case TransitionActionType::Scale:
            case TransitionActionType::Skew:
                item->tweener = GTween::to(startValue->getVec2(), endValue->getVec2(), item->tweenConfig->duration);
                break;

            case TransitionActionType::Alpha:
            case TransitionActionType::Rotation:
                item->tweener = GTween::to(startValue->f1, endValue->f1, item->tweenConfig->duration);
                break;

            case TransitionActionType::Color:
                item->tweener = GTween::to(startValue->getColor(), endValue->getColor(), item->tweenConfig->duration);
                break;

            case TransitionActionType::ColorFilter:
                item->tweener = GTween::to(startValue->getVec4(), endValue->getVec4(), item->tweenConfig->duration);
                break;
            default:
                break;
            }

            item->tweener->setDelay(time)
                ->setEase(item->tweenConfig->easeType)
                ->setRepeat(item->tweenConfig->repeat, item->tweenConfig->yoyo)
                ->setTimeScale(_timeScale)
                ->setTargetAny(item)
                ->onStart(CC_CALLBACK_1(Transition::onTweenStart, this))
                ->onUpdate(CC_CALLBACK_1(Transition::onTweenUpdate, this))
                ->onComplete1(CC_CALLBACK_1(Transition::onTweenComplete, this));

            if (_endTime >= 0)
                item->tweener->setBreakpoint(_endTime - time);

            _totalTasks++;
        }
    }
    else if (item->type == TransitionActionType::Shake)
    {
        TValue_Shake* value = (TValue_Shake*)item->value;

        if (_reversed)
            time = (_totalDuration - item->time - value->duration);
        else
            time = item->time;

        if (_endTime == -1 || time <= _endTime)
        {
            value->lastOffset.setZero();
            value->offset.setZero();
            item->tweener = GTween::shake(Vec2::ZERO, value->amplitude, value->duration)
                ->setDelay(time)
                ->setTimeScale(_timeScale)
                ->setTargetAny(item)
                ->onStart(CC_CALLBACK_1(Transition::onTweenStart, this))
                ->onUpdate(CC_CALLBACK_1(Transition::onTweenUpdate, this))
                ->onComplete1(CC_CALLBACK_1(Transition::onTweenComplete, this));

            if (_endTime >= 0)
                item->tweener->setBreakpoint(_endTime - item->time);

            _totalTasks++;
        }
    }
    else
    {
        if (_reversed)
            time = (_totalDuration - item->time);
        else
            time = item->time;

        if (time <= _startTime)
        {
            applyValue(item);
            callHook(item, false);
        }
        else if (_endTime == -1 || time <= _endTime)
        {
            _totalTasks++;
            item->tweener = GTween::delayedCall(time)
                ->setTimeScale(_timeScale)
                ->setTargetAny(item)
                ->onComplete1(CC_CALLBACK_1(Transition::onDelayedPlayItem, this));
        }
    }

    if (item->tweener != nullptr)
        item->tweener->seek(_startTime);
}

void Transition::skipAnimations()
{
    int frame;
    float playStartTime;
    float playTotalTime;
    TValue_Animation* value;
    IAnimationGear* target;
    TransitionItem* item;

    int cnt = (int)_items.size();
    for (int i = 0; i < cnt; i++)
    {
        item = _items[i];
        if (item->type != TransitionActionType::Animation || item->time > _startTime)
            continue;

        value = (TValue_Animation*)item->value;
        if (value->flag)
            continue;

        target = dynamic_cast<IAnimationGear*>(item->target);
        frame = target->getFrame();
        playStartTime = target->isPlaying() ? 0 : -1;
        playTotalTime = 0;

        for (int j = i; j < cnt; j++)
        {
            item = _items[j];
            if (item->type != TransitionActionType::Animation || item->target != (void*)target || item->time > _startTime)
                continue;

            value = (TValue_Animation*)item->value;
            value->flag = true;

            if (value->frame != -1)
            {
                frame = value->frame;
                if (value->playing)
                    playStartTime = item->time;
                else
                    playStartTime = -1;
                playTotalTime = 0;
            }
            else
            {
                if (value->playing)
                {
                    if (playStartTime < 0)
                        playStartTime = item->time;
                }
                else
                {
                    if (playStartTime >= 0)
                        playTotalTime += (item->time - playStartTime);
                    playStartTime = -1;
                }
            }

            callHook(item, false);
        }

        if (playStartTime >= 0)
            playTotalTime += (_startTime - playStartTime);

        target->setPlaying(playStartTime >= 0);
        target->setFrame(frame);
        if (playTotalTime > 0)
            target->advance(playTotalTime);
    }
}

void Transition::onDelayedPlayItem(GTweener* tweener)
{
    TransitionItem* item = (TransitionItem*)tweener->getTarget();
    item->tweener = nullptr;
    _totalTasks--;

    applyValue(item);
    callHook(item, false);

    checkAllComplete();
}

void Transition::onTweenStart(GTweener* tweener)
{
    TransitionItem* item = (TransitionItem*)tweener->getTarget();

    if (item->type == TransitionActionType::XY || item->type == TransitionActionType::Size) //位置和大小要到start才最终确认起始值
    {
        TValue* startValue;
        TValue* endValue;

        if (_reversed)
        {
            startValue = item->tweenConfig->endValue;
            endValue = item->tweenConfig->startValue;
        }
        else
        {
            startValue = item->tweenConfig->startValue;
            endValue = item->tweenConfig->endValue;
        }

        if (item->type == TransitionActionType::XY)
        {
            if (item->target != _owner)
            {
                if (!startValue->b1)
                    startValue->f1 = item->target->getX();
                if (!startValue->b2)
                    startValue->f2 = item->target->getY();
            }
            else
            {
                if (!startValue->b1)
                    startValue->f1 = item->target->getX() - _ownerBaseX;
                if (!startValue->b2)
                    startValue->f2 = item->target->getY() - _ownerBaseY;
            }
        }
        else
        {
            if (!startValue->b1)
                startValue->f1 = item->target->getWidth();
            if (!startValue->b2)
                startValue->f2 = item->target->getHeight();
        }

        if (!endValue->b1)
            endValue->f1 = startValue->f1;
        if (!endValue->b2)
            endValue->f2 = startValue->f2;

        tweener->startValue.setVec2(startValue->getVec2());
        tweener->endValue.setVec2(endValue->getVec2());
    }

    callHook(item, false);
}

void Transition::onTweenUpdate(GTweener* tweener)
{
    TransitionItem* item = (TransitionItem*)tweener->getTarget();
    switch (item->type)
    {
    case TransitionActionType::XY:
    case TransitionActionType::Size:
    case TransitionActionType::Scale:
    case TransitionActionType::Skew:
        ((TValue*)item->value)->setVec2(tweener->value.getVec2());
        break;

    case TransitionActionType::Alpha:
    case TransitionActionType::Rotation:
        ((TValue*)item->value)->f1 = tweener->value.x;
        break;

    case TransitionActionType::Color:
        ((TValue*)item->value)->setColor(tweener->value.getColor());
        break;

    case TransitionActionType::ColorFilter:
        ((TValue*)item->value)->setVec4(tweener->value.getVec4());
        break;

    case TransitionActionType::Shake:
        ((TValue_Shake*)item->value)->offset = tweener->deltaValue.getVec2();
        break;
    default:
        break;
    }
    applyValue(item);
}

void Transition::onTweenComplete(GTweener* tweener)
{
    TransitionItem* item = (TransitionItem*)tweener->getTarget();
    item->tweener = nullptr;
    _totalTasks--;

    if (tweener->allCompleted()) //当整体播放结束时间在这个tween的中间时不应该调用结尾钩子
        callHook(item, true);

    checkAllComplete();
}

void Transition::onPlayTransCompleted(TransitionItem* item)
{
    _totalTasks--;

    checkAllComplete();
}

void Transition::callHook(TransitionItem* item, bool tweenEnd)
{
    if (tweenEnd)
    {
        if (item->tweenConfig != nullptr && item->tweenConfig->endHook != nullptr)
            item->tweenConfig->endHook();
    }
    else
    {
        if (item->time >= _startTime && item->hook != nullptr)
            item->hook();
    }
}

void Transition::checkAllComplete()
{
    if (_playing && _totalTasks == 0)
    {
        if (_totalTimes < 0)
        {
            internalPlay();
        }
        else
        {
            _totalTimes--;
            if (_totalTimes > 0)
                internalPlay();
            else
            {
                _playing = false;

                for (auto &item : _items)
                {
                    if (item->target != nullptr && item->displayLockToken != 0)
                    {
                        item->target->releaseDisplayLock(item->displayLockToken);
                        item->displayLockToken = 0;
                    }
                }

                if (_onComplete != nullptr)
                {
                    PlayCompleteCallback func = _onComplete;
                    _onComplete = nullptr;
                    func();
                }
            }
        }
    }
}

void Transition::applyValue(TransitionItem* item)
{
    item->target->_gearLocked = true;

    switch (item->type)
    {
    case TransitionActionType::XY:
    {
        TValue* value = (TValue*)item->value;
        if (item->target == _owner)
        {
            float f1, f2;
            if (!value->b1)
                f1 = item->target->getX();
            else
                f1 = value->f1 + _ownerBaseX;
            if (!value->b2)
                f2 = item->target->getY();
            else
                f2 = value->f2 + _ownerBaseY;
            item->target->setPosition(f1, f2);
        }
        else
        {
            if (!value->b1)
                value->f1 = item->target->getX();
            if (!value->b2)
                value->f2 = item->target->getY();
            item->target->setPosition(value->f1, value->f2);
        }
    }
    break;

    case TransitionActionType::Size:
    {
        TValue* value = (TValue*)item->value;
        if (!value->b1)
            value->f1 = item->target->getWidth();
        if (!value->b2)
            value->f2 = item->target->getHeight();
        item->target->setSize(value->f1, value->f2);
    }
    break;

    case TransitionActionType::Pivot:
        item->target->setPivot(((TValue*)item->value)->f1, ((TValue*)item->value)->f2, item->target->isPivotAsAnchor());
        break;

    case TransitionActionType::Alpha:
        item->target->setAlpha(((TValue*)item->value)->f1);
        break;

    case TransitionActionType::Rotation:
        item->target->setRotation(((TValue*)item->value)->f1);
        break;

    case TransitionActionType::Scale:
        item->target->setScale(((TValue*)item->value)->f1, ((TValue*)item->value)->f2);
        break;

    case TransitionActionType::Skew:
        item->target->setSkewX(((TValue*)item->value)->f1);
        item->target->setSkewY(((TValue*)item->value)->f2);
        break;

    case TransitionActionType::Color:
        (dynamic_cast<IColorGear*>(item->target))->setColor((Color3B)((TValue*)item->value)->getColor());
        break;

    case TransitionActionType::Animation:
    {
        TValue_Animation* value = (TValue_Animation*)item->value;
        IAnimationGear* target = (dynamic_cast<IAnimationGear*>(item->target));
        if (value->frame >= 0)
            target->setFrame(value->frame);
        target->setPlaying(value->playing);
        target->setTimeScale(_timeScale);
    }
    break;

    case TransitionActionType::Visible:
        item->target->setVisible(((TValue_Visible*)item->value)->visible);
        break;

    case TransitionActionType::Shake:
    {
        TValue_Shake* value = (TValue_Shake*)item->value;
        item->target->setPosition(item->target->getX() - value->lastOffset.x + value->offset.x, item->target->getY() - value->lastOffset.y + value->offset.y);
        value->lastOffset = value->offset;
    }
    break;

    case TransitionActionType::Transition:
        if (_playing)
        {
            TValue_Transition* value = (TValue_Transition*)item->value;
            if (value->trans != nullptr)
            {
                _totalTasks++;

                float startTime = _startTime > item->time ? (_startTime - item->time) : 0;
                float endTime = _endTime >= 0 ? (_endTime - item->time) : -1;
                if (value->stopTime >= 0 && (endTime < 0 || endTime > value->stopTime))
                    endTime = value->stopTime;
                value->trans->setTimeScale(_timeScale);
                value->trans->play(value->playTimes, 0, startTime, endTime, [this, item]() { onPlayTransCompleted(item); }, _reversed);
            }
        }
        break;

    case TransitionActionType::Sound:
        if (_playing && item->time >= _startTime)
        {
            TValue_Sound* value = (TValue_Sound*)item->value;
            if (!value->sound.empty())
                UIRoot->playSound(value->sound, value->volume);
            break;
        }

    case TransitionActionType::ColorFilter:
        break;

    case TransitionActionType::Text:
        item->target->setText(((TValue_Text*)item->value)->text);
        break;

    case TransitionActionType::Icon:
        item->target->setIcon(((TValue_Text*)item->value)->text);
        break;
    default:
        break;
    }

    item->target->_gearLocked = false;
}

void Transition::setup(ByteBuffer* buffer)
{
    name = buffer->ReadS();
    _options = buffer->ReadInt();
    _autoPlay = buffer->ReadBool();
    _autoPlayTimes = buffer->ReadInt();
    _autoPlayDelay = buffer->ReadFloat();

    int cnt = buffer->ReadShort();
    for (int i = 0; i < cnt; i++)
    {
        int dataLen = buffer->ReadShort();
        int curPos = buffer->position;

        buffer->Seek(curPos, 0);

        TransitionItem* item = new TransitionItem((TransitionActionType)buffer->ReadByte());
        _items.push_back(item);

        item->time = buffer->ReadFloat();
        int targetId = buffer->ReadShort();
        if (targetId < 0)
            item->targetId = STD_STRING_EMPTY;
        else
            item->targetId = _owner->getChildAt(targetId)->id;
        item->label = buffer->ReadS();

        if (buffer->ReadBool())
        {
            buffer->Seek(curPos, 1);

            item->tweenConfig = new TweenConfig();
            item->tweenConfig->duration = buffer->ReadFloat();
            if (item->time + item->tweenConfig->duration > _totalDuration)
                _totalDuration = item->time + item->tweenConfig->duration;
            item->tweenConfig->easeType = (EaseType)buffer->ReadByte();
            item->tweenConfig->repeat = buffer->ReadInt();
            item->tweenConfig->yoyo = buffer->ReadBool();
            item->tweenConfig->endLabel = buffer->ReadS();

            buffer->Seek(curPos, 2);

            decodeValue(item, buffer, item->tweenConfig->startValue);

            buffer->Seek(curPos, 3);

            decodeValue(item, buffer, item->tweenConfig->endValue);
        }
        else
        {
            if (item->time > _totalDuration)
                _totalDuration = item->time;

            buffer->Seek(curPos, 2);

            decodeValue(item, buffer, item->value);
        }

        buffer->position = curPos + dataLen;
    }
}

void Transition::decodeValue(TransitionItem* item, ByteBuffer * buffer, void* value)
{
    switch (item->type)
    {
    case TransitionActionType::XY:
    case TransitionActionType::Size:
    case TransitionActionType::Pivot:
    case TransitionActionType::Skew:
    {
        TValue* tvalue = (TValue*)value;
        tvalue->b1 = buffer->ReadBool();
        tvalue->b2 = buffer->ReadBool();
        tvalue->f1 = buffer->ReadFloat();
        tvalue->f2 = buffer->ReadFloat();
        break;
    }

    case TransitionActionType::Alpha:
    case TransitionActionType::Rotation:
        ((TValue*)value)->f1 = buffer->ReadFloat();
        break;

    case TransitionActionType::Scale:
    {
        ((TValue*)value)->f1 = buffer->ReadFloat();
        ((TValue*)value)->f2 = buffer->ReadFloat();
        break;
    }

    case TransitionActionType::Color:
        ((TValue*)value)->setColor(buffer->ReadColor());
        break;

    case TransitionActionType::Animation:
    {
        ((TValue_Animation*)value)->playing = buffer->ReadBool();
        ((TValue_Animation*)value)->frame = buffer->ReadInt();
        break;
    }

    case TransitionActionType::Visible:
        ((TValue_Visible*)value)->visible = buffer->ReadBool();
        break;

    case TransitionActionType::Sound:
    {
        ((TValue_Sound*)value)->sound = buffer->ReadS();
        ((TValue_Sound*)value)->volume = buffer->ReadFloat();
        break;
    }

    case TransitionActionType::Transition:
    {
        ((TValue_Transition*)value)->transName = buffer->ReadS();
        ((TValue_Transition*)value)->playTimes = buffer->ReadInt();
        break;
    }

    case TransitionActionType::Shake:
    {
        ((TValue_Shake*)value)->amplitude = buffer->ReadFloat();
        ((TValue_Shake*)value)->duration = buffer->ReadFloat();
        break;
    }

    case TransitionActionType::ColorFilter:
    {
        TValue* tvalue = (TValue*)value;
        tvalue->f1 = buffer->ReadFloat();
        tvalue->f2 = buffer->ReadFloat();
        tvalue->f3 = buffer->ReadFloat();
        tvalue->f4 = buffer->ReadFloat();
        break;
    }

    case TransitionActionType::Text:
    case TransitionActionType::Icon:
        ((TValue_Text*)value)->text = buffer->ReadS();
        break;

    default:
        break;
    }
}

NS_FGUI_END
