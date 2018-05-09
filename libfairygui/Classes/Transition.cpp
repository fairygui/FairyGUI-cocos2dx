#include "Transition.h"
#include "GComponent.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
USING_NS_CC;
using namespace std;

const int FRAME_RATE = 24;

const int OPTION_IGNORE_DISPLAY_CONTROLLER = 1;
const int OPTION_AUTO_STOP_DISABLED = 2;
const int OPTION_AUTO_STOP_AT_END = 4;

class TransitionValue
{
public:
    float f1;//x, scalex, pivotx,alpha,shakeAmplitude,rotation
    float f2;//y, scaley, pivoty, shakePeriod
    float f3;
    float f4;
    int i;//frame
    Color4B c;//color
    bool b;//playing
    string s;//sound,transName

    bool b1;
    bool b2;

    TransitionValue();
    TransitionValue(TransitionValue& source);
    TransitionValue& operator= (const TransitionValue& other);
};

TransitionValue::TransitionValue() :
    f1(0), f2(0), f3(0), f4(0), i(0), b(false), b1(false), b2(false)
{
}

TransitionValue::TransitionValue(TransitionValue& other)
{
    *this = other;
}

TransitionValue& TransitionValue::operator= (const TransitionValue& other)
{
    f1 = other.f1;
    f2 = other.f2;
    f3 = other.f3;
    f4 = other.f4;
    i = other.i;
    c = other.c;
    b = other.b;
    s = other.s;
    b1 = other.b1;
    b2 = other.b2;
    return *this;
}

class TransitionItem
{
public:
    float time;
    string targetId;
    TransitionActionType type;
    float duration;
    TransitionValue value;
    TransitionValue startValue;
    TransitionValue endValue;
    tweenfunc::TweenType easeType;
    int repeat;
    bool yoyo;
    bool tween;
    string label;
    string label2;

    //hooks
    Transition::TransitionHook hook;
    Transition::TransitionHook hook2;

    //running properties
    bool completed;
    GObject* target;
    bool filterCreated;
    uint32_t displayLockToken;

    TransitionItem();
};

TransitionItem::TransitionItem() :
    time(0),
    type(TransitionActionType::XY),
    duration(0),
    easeType(tweenfunc::TweenType::Quad_EaseOut),
    repeat(0),
    yoyo(false),
    tween(false),
    hook(nullptr),
    hook2(nullptr),
    completed(false),
    target(nullptr),
    filterCreated(false),
    displayLockToken(0)
{

}

Transition::Transition(GComponent* owner, int index) :
    autoPlayRepeat(1),
    autoPlayDelay(0),
    _owner(owner),
    _totalTimes(0),
    _totalTasks(0),
    _playing(false),
    _ownerBaseX(0),
    _ownerBaseY(0),
    _onComplete(nullptr),
    _options(0),
    _reversed(false),
    _maxTime(0),
    _autoPlay(false),
    _actionTag(ActionTag::TRANSITION_ACTION + index)
{
}

Transition::~Transition()
{
    for (auto &item : _items)
        delete item;
}

void Transition::setAutoPlay(bool value)
{
    if (_autoPlay != value)
    {
        _autoPlay = value;
        if (_autoPlay)
        {
            if (_owner->onStage())
                play(autoPlayRepeat, autoPlayDelay, nullptr);
        }
        else
        {
            if (!_owner->onStage())
                stop(false, true);
        }
    }
}

void Transition::play(PlayCompleteCallback callback)
{
    play(1, 0, callback);
}

void Transition::play(int times, float delay, PlayCompleteCallback callback)
{
    play(times, delay, callback, false);
}

void Transition::playReverse(PlayCompleteCallback callback)
{
    playReverse(1, 0, callback);
}

void Transition::playReverse(int times, float delay, PlayCompleteCallback callback)
{
    play(times, delay, callback, true);
}

void Transition::changeRepeat(int value)
{
    _totalTimes = value;
}

void Transition::play(int times, float delay, PlayCompleteCallback onComplete, bool reverse)
{
    stop(true, true);

    _totalTimes = times;
    _reversed = reverse;

    internalPlay(delay);
    _playing = _totalTasks > 0;
    if (_playing)
    {
        _onComplete = onComplete;

        if ((_options & OPTION_IGNORE_DISPLAY_CONTROLLER) != 0)
        {
            for (auto &item : _items)
            {
                if (item->target != nullptr && item->target != _owner)
                    item->displayLockToken = item->target->addDisplayLock();
            }
        }
    }
    else if (onComplete != nullptr)
        onComplete();
}

void Transition::stop()
{
    stop(true, false);
}

void Transition::stop(bool setToComplete, bool processCallback)
{
    if (_playing)
    {
        _playing = false;
        _totalTasks = 0;
        _totalTimes = 0;
        PlayCompleteCallback func = _onComplete;
        _onComplete = nullptr;
        _owner->displayObject()->stopAllActionsByTag(_actionTag);

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
}

void Transition::stopItem(TransitionItem * item, bool setToComplete)
{
    if (item->displayLockToken != 0)
    {
        item->target->releaseDisplayLock(item->displayLockToken);
        item->displayLockToken = 0;
    }

    //if (item->type == TransitionActionType::ColorFilter && item->filterCreated)
      //  item->target->setFilter(nullptr);

    if (item->completed)
        return;

    if (item->type == TransitionActionType::Transition)
    {
        Transition* trans = item->target->as<GComponent>()->getTransition(item->value.s);
        if (trans != nullptr)
            trans->stop(setToComplete, false);
    }
    else if (item->type == TransitionActionType::Shake)
    {
        Director::getInstance()->getScheduler()->unschedule("-", item);

        item->target->_gearLocked = true;
        item->target->setPosition(item->target->getX() - item->startValue.f1, item->target->getY() - item->startValue.f2);
        item->target->_gearLocked = false;
    }
    else
    {
        if (setToComplete)
        {
            if (item->tween)
            {
                if (!item->yoyo || item->repeat % 2 == 0)
                    applyValue(item, _reversed ? item->startValue : item->endValue);
                else
                    applyValue(item, _reversed ? item->endValue : item->startValue);
            }
            else if (item->type != TransitionActionType::Sound)
                applyValue(item, item->value);
        }
    }
}

void Transition::setValue(const std::string & label, const ValueVector& values)
{
    for (auto &item : _items)
    {
        if (item->label == label)
        {
            if (item->tween)
                setValue(item, item->startValue, values);
            else
                setValue(item, item->value, values);
        }
        else if (item->label2 == label)
        {
            setValue(item, item->endValue, values);
        }
    }
}

void Transition::setValue(TransitionItem* item, TransitionValue& value, const ValueVector& values)
{
    switch (item->type)
    {
    case TransitionActionType::XY:
    case TransitionActionType::Size:
    case TransitionActionType::Pivot:
    case TransitionActionType::Scale:
    case TransitionActionType::Skew:
        value.b1 = true;
        value.b2 = true;
        value.f1 = values[0].asFloat();
        value.f2 = values[1].asFloat();
        break;

    case TransitionActionType::Alpha:
        value.f1 = values[0].asFloat();
        break;

    case TransitionActionType::Rotation:
        value.f1 = values[0].asFloat();
        break;

    case TransitionActionType::Color:
    {
        uint32_t v = values[0].asUnsignedInt();
        value.c = Color4B((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF, (v >> 24) & 0xFF);
        break;
    }

    case TransitionActionType::Animation:
        value.i = values[0].asInt();
        if (values.size() > 1)
            value.b = values[1].asBool();
        break;

    case TransitionActionType::Visible:
        value.b = values[0].asBool();
        break;

    case TransitionActionType::Sound:
        value.s = values[0].asString();
        if (values.size() > 1)
            value.f1 = values[1].asFloat();
        break;

    case TransitionActionType::Transition:
        value.s = values[0].asString();
        if (values.size() > 1)
            value.i = values[1].asInt();
        break;

    case TransitionActionType::Shake:
        value.f1 = values[0].asFloat();
        if (values.size() > 1)
            value.f2 = values[1].asFloat();
        break;

    case TransitionActionType::ColorFilter:
        value.f1 = values[1].asFloat();
        value.f2 = values[2].asFloat();
        value.f3 = values[3].asFloat();
        value.f4 = values[4].asFloat();
        break;

    default:
        break;
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
        else if (item->label2 == label)
        {
            item->hook2 = callback;
            break;
        }
    }
}

void Transition::clearHooks()
{
    for (auto &item : _items)
    {
        item->hook = nullptr;
        item->hook2 = nullptr;
    }
}

void Transition::setTarget(const std::string & label, GObject * newTarget)
{
    for (auto &item : _items)
    {
        if (item->label == label)
            item->targetId = newTarget->id;
    }
}

void Transition::setDuration(const std::string & label, float value)
{
    for (auto &item : _items)
    {
        if (item->tween && item->label == label)
            item->duration = value;
    }
}

void Transition::updateFromRelations(const std::string & targetId, float dx, float dy)
{
    int cnt = (int)_items.size();
    if (cnt == 0)
        return;

    for (int i = 0; i < cnt; i++)
    {
        TransitionItem* item = _items[i];
        if (item->type == TransitionActionType::XY && item->targetId == targetId)
        {
            if (item->tween)
            {
                item->startValue.f1 += dx;
                item->startValue.f2 += dy;
                item->endValue.f1 += dx;
                item->endValue.f2 += dy;
            }
            else
            {
                item->value.f1 += dx;
                item->value.f2 += dy;
            }
        }
    }
}

void Transition::OnOwnerRemovedFromStage()
{
    if ((_options & OPTION_AUTO_STOP_DISABLED) == 0)
        stop((_options & OPTION_AUTO_STOP_AT_END) != 0 ? true : false, false);
}

void Transition::internalPlay(float delay)
{
    _ownerBaseX = _owner->getX();
    _ownerBaseY = _owner->getY();

    _totalTasks = 0;
    for (auto &item : _items)
    {
        if (!item->targetId.empty())
            item->target = _owner->getChildById(item->targetId);
        else
            item->target = _owner;
        if (item->target == nullptr)
            continue;

        if (item->tween)
        {
            float startTime = delay;
            if (_reversed)
                startTime += (_maxTime - item->time - item->duration);
            else
                startTime += item->time;
            if (startTime > 0 && (item->type == TransitionActionType::XY || item->type == TransitionActionType::Size))
            {
                _totalTasks++;
                item->completed = false;
                ActionInterval* action = Sequence::createWithTwoActions(DelayTime::create(startTime), CallFunc::create([item, this]
                {
                    _totalTasks--;

                    startTween(item, 0);
                }));
                action->setTag(ActionTag::TRANSITION_ACTION);
                _owner->displayObject()->runAction(action);
            }
            else
                startTween(item, startTime);
        }
        else
        {
            float startTime = delay;
            if (_reversed)
                startTime += (_maxTime - item->time);
            else
                startTime += item->time;
            if (startTime == 0)
                applyValue(item, item->value);
            else
            {
                item->completed = false;
                _totalTasks++;
                ActionInterval* action = Sequence::createWithTwoActions(DelayTime::create(startTime), CallFunc::create([item, this]
                {
                    item->completed = true;
                    _totalTasks--;

                    applyValue(item, item->value);
                    if (item->hook)
                        item->hook();

                    checkAllComplete();
                }));
                action->setTag(ActionTag::TRANSITION_ACTION);
                _owner->displayObject()->runAction(action);
            }
        }
    }
}

void Transition::startTween(TransitionItem * item, float delay)
{
    if (_reversed)
        startTween(item, delay, item->endValue, item->startValue);
    else
        startTween(item, delay, item->startValue, item->endValue);
}

void Transition::startTween(TransitionItem * item, float delay, TransitionValue& startValue, TransitionValue& endValue)
{
    ActionInterval* mainAction = nullptr;

    switch (item->type)
    {
    case TransitionActionType::XY:
    case TransitionActionType::Size:
    {
        if (item->type == TransitionActionType::XY)
        {
            if (item->target == _owner)
            {
                if (!startValue.b1)
                    startValue.f1 = 0;
                if (!startValue.b2)
                    startValue.f2 = 0;
            }
            else
            {
                if (!startValue.b1)
                    startValue.f1 = item->target->getX();
                if (!startValue.b2)
                    startValue.f2 = item->target->getY();
            }
        }
        else
        {
            if (!startValue.b1)
                startValue.f1 = item->target->getWidth();
            if (!startValue.b2)
                startValue.f2 = item->target->getHeight();
        }

        item->value.f1 = startValue.f1;
        item->value.f2 = startValue.f2;

        if (!endValue.b1)
            endValue.f1 = item->value.f1;
        if (!endValue.b2)
            endValue.f2 = item->value.f2;

        item->value.b1 = startValue.b1 || endValue.b1;
        item->value.b2 = startValue.b2 || endValue.b2;

        mainAction = ActionVec2::create(item->duration,
            Vec2(startValue.f1, startValue.f2),
            Vec2(endValue.f1, endValue.f2),
            [this, item](const Vec2& value)
        {
            item->value.f1 = value.x;
            item->value.f2 = value.y;
            applyValue(item, item->value);
        });
        break;
    }

    case TransitionActionType::Scale:
    case TransitionActionType::Skew:
    {
        item->value.f1 = startValue.f1;
        item->value.f2 = startValue.f2;
        mainAction = ActionVec2::create(item->duration,
            Vec2(startValue.f1, startValue.f2),
            Vec2(endValue.f1, endValue.f2),
            [this, item](const Vec2& value)
        {
            item->value.f1 = value.x;
            item->value.f2 = value.y;
            applyValue(item, item->value);
        });
        break;
    }

    case TransitionActionType::Alpha:
    case TransitionActionType::Rotation:
    {
        item->value.f1 = startValue.f1;
        mainAction = ActionFloat::create(item->duration,
            startValue.f1,
            endValue.f1,
            [this, item](float value)
        {
            item->value.f1 = value;
            applyValue(item, item->value);
        });
        break;
    }

    case TransitionActionType::Color:
    {
        item->value.c = startValue.c;
        mainAction = ActionVec4::create(item->duration,
            Vec4(item->value.c.r, item->value.c.g, item->value.c.b, item->value.c.a),
            Vec4(endValue.c.r, endValue.c.g, endValue.c.b, endValue.c.a),
            [this, item](const Vec4& value)
        {
            item->value.c.r = value.x;
            item->value.c.g = value.y;
            item->value.c.b = value.z;
            item->value.c.a = value.w;
            applyValue(item, item->value);
        });
        break;
    }

    case TransitionActionType::ColorFilter:
    {
        item->value.f1 = startValue.f1;
        item->value.f2 = startValue.f2;
        item->value.f3 = startValue.f3;
        item->value.f4 = startValue.f4;
        mainAction = ActionVec4::create(item->duration,
            Vec4(startValue.f1, startValue.f2, startValue.f3, startValue.f4),
            Vec4(endValue.f1, endValue.f2, endValue.f3, endValue.f4),
            [this, item](const Vec4& value)
        {
            item->value.f1 = value.x;
            item->value.f2 = value.y;
            item->value.f3 = value.z;
            item->value.f4 = value.w;
            applyValue(item, item->value);
        });
        break;
    }
    default:
        break;
    }

    mainAction = createEaseAction(item->easeType, mainAction);
    if (item->repeat != 0)
        mainAction = RepeatYoyo::create(mainAction, item->repeat == -1 ? INT_MAX : (item->repeat + 1), item->yoyo);

    FiniteTimeAction* completeAction = CallFunc::create([this, item]() { tweenComplete(item); });
    if (delay > 0)
    {
        FiniteTimeAction* delayAction = DelayTime::create(delay);
        if (item->hook)
            mainAction = Sequence::create({ delayAction, CallFunc::create(item->hook), mainAction, completeAction });
        else
            mainAction = Sequence::create({ delayAction, mainAction, completeAction });
    }
    else
    {
        applyValue(item, item->value);
        if (item->hook)
            item->hook();

        mainAction = Sequence::createWithTwoActions(mainAction, completeAction);
    }

    mainAction->setTag(ActionTag::TRANSITION_ACTION);
    _owner->displayObject()->runAction(mainAction);
    _totalTasks++;
    item->completed = false;
}

void Transition::tweenComplete(TransitionItem * item)
{
    item->completed = true;
    _totalTasks--;

    if (item->hook2)
        item->hook2();

    checkAllComplete();
}

void Transition::checkAllComplete()
{
    if (_playing && _totalTasks == 0)
    {
        if (_totalTimes < 0)
        {
            internalPlay(0);
        }
        else
        {
            _totalTimes--;
            if (_totalTimes > 0)
                internalPlay(0);
            else
            {
                _playing = false;

                for (auto &item : _items)
                {
                    if (item->target != nullptr)
                    {
                        if (item->displayLockToken != 0)
                        {
                            item->target->releaseDisplayLock(item->displayLockToken);
                            item->displayLockToken = 0;
                        }

                        /*if (item->filterCreated)
                        {
                            item->filterCreated = false;
                            item->target->setFilter(nullptr);
                        }*/
                    }
                }

                if (_onComplete)
                {
                    PlayCompleteCallback func = _onComplete;
                    _onComplete = nullptr;
                    func();
                }
            }
        }
    }
}

void Transition::applyValue(TransitionItem * item, TransitionValue & value)
{
    item->target->_gearLocked = true;

    switch (item->type)
    {
    case TransitionActionType::XY:
        if (item->target == _owner)
        {
            float f1, f2;
            if (!value.b1)
                f1 = item->target->getX();
            else
                f1 = value.f1 + _ownerBaseX;
            if (!value.b2)
                f2 = item->target->getY();
            else
                f2 = value.f2 + _ownerBaseY;
            item->target->setPosition(f1, f2);
        }
        else
        {
            if (!value.b1)
                value.f1 = item->target->getX();
            if (!value.b2)
                value.f2 = item->target->getY();
            item->target->setPosition(value.f1, value.f2);
        }
        break;

    case TransitionActionType::Size:
        if (!value.b1)
            value.f1 = item->target->getWidth();
        if (!value.b2)
            value.f2 = item->target->getHeight();
        item->target->setSize(value.f1, value.f2);
        break;

    case TransitionActionType::Pivot:
        item->target->setPivot(value.f1, value.f2);
        break;

    case TransitionActionType::Alpha:
        item->target->setAlpha(value.f1);
        break;

    case TransitionActionType::Rotation:
        item->target->setRotation(value.f1);
        break;

    case TransitionActionType::Scale:
        item->target->setScale(value.f1, value.f2);
        break;

    case TransitionActionType::Skew:
        item->target->setSkewX(value.f1);
        item->target->setSkewY(value.f2);
        break;

    case TransitionActionType::Color:
    {
        IColorGear* cg = dynamic_cast<IColorGear*>(item->target);
        if (cg)
            cg->cg_setColor(value.c);
        break;
    }

    case TransitionActionType::Animation:
    {
        IAnimationGear* ag = dynamic_cast<IAnimationGear*>(item->target);
        if (ag)
        {
            if (!value.b1)
                value.i = ag->getCurrentFrame();
            ag->setCurrentFrame(value.i);
            ag->setPlaying(value.b);
        }
        break;
    }

    case TransitionActionType::Visible:
        item->target->setVisible(value.b);
        break;

    case TransitionActionType::Transition:
    {
        Transition* trans = item->target->as<GComponent>()->getTransition(value.s);
        if (trans != nullptr)
        {
            if (value.i == 0)
                trans->stop(false, true);
            else if (trans->isPlaying())
                trans->_totalTimes = value.i;
            else
            {
                item->completed = false;
                _totalTasks++;
                if (_reversed)
                    trans->playReverse(value.i, 0, [this, item]() { playTransComplete(item); });
                else
                    trans->play(value.i, 0, [this, item]() { playTransComplete(item); });
            }
        }
        break;
    }

    case TransitionActionType::Sound:
    {
        UIRoot->playSound(value.s, value.f1);
        break;
    }

    case TransitionActionType::Shake:
        item->startValue.f1 = 0; //offsetX
        item->startValue.f2 = 0; //offsetY
        item->startValue.f3 = item->value.f2;//shakePeriod
        Director::getInstance()->getScheduler()->schedule(CC_CALLBACK_1(Transition::shakeItem, this, item), item, 0, false, "-");
        _totalTasks++;
        item->completed = false;
        break;

    case TransitionActionType::ColorFilter:
        break;

    default:
        break;
    }

    item->target->_gearLocked = false;
}

void Transition::playTransComplete(TransitionItem* item)
{
    _totalTasks--;
    item->completed = true;

    checkAllComplete();
}

void Transition::shakeItem(float dt, TransitionItem * item)
{
    float r = ceil(item->value.f1 * item->startValue.f3 / item->value.f2);
    float x1 = (1 - rand_0_1() * 2)*r;
    float y1 = (1 - rand_0_1() * 2)*r;
    x1 = x1 > 0 ? ceil(x1) : floor(x1);
    y1 = y1 > 0 ? ceil(y1) : floor(y1);

    item->target->_gearLocked = true;
    item->target->setPosition(item->target->getX() - item->startValue.f1 + x1, item->target->getY() - item->startValue.f2 + y1);
    item->target->_gearLocked = false;

    item->startValue.f1 = x1;
    item->startValue.f2 = y1;
    item->startValue.f3 -= dt;
    if (item->startValue.f3 <= 0)
    {
        item->target->_gearLocked = true;
        item->target->setPosition(item->target->getX() - item->startValue.f1, item->target->getY() - item->startValue.f2);
        item->target->_gearLocked = false;

        item->completed = true;
        _totalTasks--;
        Director::getInstance()->getScheduler()->unschedule("-", item);

        checkAllComplete();
    }
}

void Transition::decodeValue(TransitionActionType type, const char* pValue, TransitionValue & value)
{
    string str;
    if (pValue)
        str = pValue;
    switch (type)
    {
    case TransitionActionType::XY:
    case TransitionActionType::Size:
    case TransitionActionType::Pivot:
    case TransitionActionType::Skew:
    {
        string s1, s2;
        ToolSet::splitString(str, ',', s1, s2);
        if (s1 == "-")
        {
            value.b1 = false;
        }
        else
        {
            value.f1 = atof(s1.c_str());
            value.b1 = true;
        }
        if (s2 == "-")
        {
            value.b2 = false;
        }
        else
        {
            value.f2 = atof(s2.c_str());
            value.b2 = true;
        }
        break;
    }

    case TransitionActionType::Alpha:
        value.f1 = atof(str.c_str());
        break;

    case TransitionActionType::Rotation:
        value.f1 = atoi(str.c_str());
        break;

    case TransitionActionType::Scale:
    {
        Vec2 v2;
        ToolSet::splitString(str, ',', v2);
        value.f1 = v2.x;
        value.f2 = v2.y;
        break;
    }

    case TransitionActionType::Color:
        value.c = ToolSet::convertFromHtmlColor(str.c_str());
        break;

    case TransitionActionType::Animation:
    {
        string s1, s2;
        ToolSet::splitString(str, ',', s1, s2);
        if (s1 == "-")
        {
            value.b1 = false;
        }
        else
        {
            value.i = atoi(s1.c_str());
            value.b1 = true;
        }
        value.b = s2 == "p";
        break;
    }

    case TransitionActionType::Visible:
        value.b = str == "true";
        break;

    case TransitionActionType::Sound:
    {
        string s;
        ToolSet::splitString(str, ',', value.s, s);
        if (!s.empty())
        {
            int intv = atoi(s.c_str());
            if (intv == 100 || intv == 0)
                value.f1 = 1;
            else
                value.f1 = (float)intv / 100;
        }
        else
            value.f1 = 1;
        break;
    }

    case TransitionActionType::Transition:
    {
        string s;
        ToolSet::splitString(str, ',', value.s, s);
        if (!s.empty())
            value.i = atoi(s.c_str());
        else
            value.i = 1;
        break;
    }

    case TransitionActionType::Shake:
    {
        Vec2 v2;
        ToolSet::splitString(str, ',', v2);
        value.f1 = v2.x;
        value.f2 = v2.y;
        break;
    }

    case TransitionActionType::ColorFilter:
    {
        Vec4 v4;
        ToolSet::splitString(str, ',', v4);
        value.f1 = v4.x;
        value.f2 = v4.y;
        value.f3 = v4.z;
        value.f4 = v4.w;
        break;
    }
    default:
        break;
    }
}

void Transition::setup(TXMLElement * xml)
{
    const char* p;
    name = xml->Attribute("name");
    p = xml->Attribute("options");
    if (p)
        _options = atoi(p);
    _autoPlay = xml->BoolAttribute("autoPlay");
    if (_autoPlay)
    {
        p = xml->Attribute("autoPlayRepeat");
        if (p)
            autoPlayRepeat = atoi(p);
        autoPlayDelay = xml->FloatAttribute("autoPlayDelay");
    }

    TXMLElement* cxml = xml->FirstChildElement("item");
    while (cxml)
    {
        TransitionItem* item = new TransitionItem();
        _items.push_back(item);

        item->time = (float)cxml->IntAttribute("time") / (float)FRAME_RATE;
        p = cxml->Attribute("target");
        if (p)
            item->targetId = p;
        p = cxml->Attribute("type");
        if (p)
            item->type = ToolSet::parseTransitionActionType(p);
        item->tween = cxml->BoolAttribute("tween");
        p = cxml->Attribute("label");
        if (p)
            item->label = p;
        if (item->tween)
        {
            item->duration = (float)cxml->IntAttribute("duration") / FRAME_RATE;
            if (item->time + item->duration > _maxTime)
                _maxTime = item->time + item->duration;

            p = cxml->Attribute("ease");
            if (p)
                item->easeType = ToolSet::parseEaseType(p);

            item->repeat = cxml->IntAttribute("repeat");
            item->yoyo = cxml->BoolAttribute("yoyo");
            p = cxml->Attribute("label2");
            if (p)
                item->label2 = p;

            p = cxml->Attribute("endValue");
            if (p)
            {
                decodeValue(item->type, cxml->Attribute("startValue"), item->startValue);
                decodeValue(item->type, p, item->endValue);
            }
            else
            {
                item->tween = false;
                decodeValue(item->type, cxml->Attribute("startValue"), item->value);
            }
        }
        else
        {
            if (item->time > _maxTime)
                _maxTime = item->time;
            decodeValue(item->type, cxml->Attribute("value"), item->value);
        }

        cxml = cxml->NextSiblingElement("item");
    }
}

NS_FGUI_END
