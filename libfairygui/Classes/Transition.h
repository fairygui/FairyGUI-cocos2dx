#ifndef __TRANSITION_H__
#define __TRANSITION_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class GObject;
class GComponent;
class TransitionItem;
class TransitionValue;

enum class TransitionActionType
{
    XY,
    Size,
    Scale,
    Pivot,
    Alpha,
    Rotation,
    Color,
    Animation,
    Visible,
    Sound,
    Transition,
    Shake,
    ColorFilter,
    Skew,
    Unknown
};

class Transition : public cocos2d::Ref
{
public:
    typedef std::function<void()> PlayCompleteCallback;
    typedef std::function<void()> TransitionHook;

    Transition(GComponent* owner, int index);
    virtual ~Transition();

    GComponent* getOwner() { return _owner; }
    bool isAutoPlay() { return _autoPlay; }
    void setAutoPlay(bool value);
    bool isPlaying() { return _playing; }

    void play(PlayCompleteCallback callback = nullptr);
    void play(int times, float delay, PlayCompleteCallback callback = nullptr);
    void playReverse(PlayCompleteCallback callback = nullptr);
    void playReverse(int times, float delay, PlayCompleteCallback callback = nullptr);
    void changeRepeat(int value);
    void stop();
    void stop(bool setToComplete, bool processCallback);

    void setValue(const std::string& label, const cocos2d::ValueVector& values);
    void setHook(const std::string& label, TransitionHook callback);
    void clearHooks();
    void setTarget(const std::string& label, GObject* newTarget);
    void setDuration(const std::string& label, float value);

    void updateFromRelations(const std::string& targetId, float dx, float dy);
    void OnOwnerRemovedFromStage();

    void setup(TXMLElement* xml);

    std::string name;
    int autoPlayRepeat;
    float autoPlayDelay;

private:
    void play(int times, float delay, PlayCompleteCallback onComplete, bool reverse);
    void stopItem(TransitionItem* item, bool setToComplete);
    void internalPlay(float delay);
    void startTween(TransitionItem* item, float delay);
    void tweenComplete(TransitionItem* item);
    void checkAllComplete();
    void applyValue(TransitionItem* item, TransitionValue& value);
    void playTransComplete(TransitionItem* item);
    void shakeItem(float dt, TransitionItem* item);

    void decodeValue(TransitionActionType type, const char* pValue, TransitionValue& value);

    GComponent* _owner;
    std::vector<TransitionItem*> _items;
    int _totalTimes;
    int _totalTasks;
    bool _playing;
    float _ownerBaseX;
    float _ownerBaseY;
    PlayCompleteCallback _onComplete;
    int _options;
    bool _reversed;
    float _maxTime;
    bool _autoPlay;
    float _timeScale;
    int _actionTag;
};

NS_FGUI_END

#endif
