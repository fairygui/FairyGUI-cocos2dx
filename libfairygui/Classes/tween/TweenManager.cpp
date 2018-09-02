#include "TweenManager.h"
#include "GTweener.h"

NS_FGUI_BEGIN
USING_NS_CC;

GTweener** TweenManager::_activeTweens = nullptr;
std::vector<GTweener*> TweenManager::_tweenerPool;
int TweenManager::_totalActiveTweens = 0;
int TweenManager::_arrayLength = 0;
bool TweenManager::_inited = false;

class TweenEngine
{
public:
    void update(float dt)
    {
        TweenManager::Update(dt);
    }
};
static TweenEngine tweenEngine;

GTweener* TweenManager::CreateTween()
{
    if (!_inited)
        Init();

    GTweener* tweener;
    int cnt = _tweenerPool.size();
    if (cnt > 0)
    {
        tweener = _tweenerPool[cnt - 1];
        _tweenerPool.pop_back();
    }
    else
        tweener = new GTweener();
    tweener->_Init();
    _activeTweens[_totalActiveTweens++] = tweener;

    if (_totalActiveTweens == _arrayLength)
    {
        int newLen = _arrayLength + ceil(_arrayLength * 0.5f);
        GTweener** newArray = new GTweener*[newLen];
        memcpy(newArray, _activeTweens, _arrayLength * sizeof(GTweener*));
        delete _activeTweens;
        _activeTweens = newArray;
        _arrayLength = newLen;
    }

    return tweener;
}

bool TweenManager::IsTweening(cocos2d::Ref * target, TweenPropType propType)
{
    if (target == nullptr)
        return false;

    bool anyType = propType == TweenPropType::None;
    for (int i = 0; i < _totalActiveTweens; i++)
    {
        GTweener* tweener = _activeTweens[i];
        if (tweener != nullptr && tweener->_target == target && !tweener->_killed
            && (anyType || tweener->_propType == propType))
            return true;
    }

    return false;
}

bool TweenManager::KillTweens(cocos2d::Ref * target, TweenPropType propType, bool completed)
{
    if (target == nullptr)
        return false;

    bool flag = false;
    int cnt = _totalActiveTweens;
    bool anyType = propType == TweenPropType::None;
    for (int i = 0; i < cnt; i++)
    {
        GTweener* tweener = _activeTweens[i];
        if (tweener != nullptr && tweener->_target == target && !tweener->_killed
            && (anyType || tweener->_propType == propType))
        {
            tweener->Kill(completed);
            flag = true;
        }
    }

    return flag;
}

GTweener * TweenManager::GetTween(cocos2d::Ref * target, TweenPropType propType)
{
    if (target == nullptr)
        return nullptr;

    int cnt = _totalActiveTweens;
    bool anyType = propType == TweenPropType::None;
    for (int i = 0; i < cnt; i++)
    {
        GTweener* tweener = _activeTweens[i];
        if (tweener != nullptr && tweener->_target == target && !tweener->_killed
            && (anyType || tweener->_propType == propType))
        {
            return tweener;
        }
    }

    return nullptr;
}

void TweenManager::Update(float dt)
{
    int cnt = _totalActiveTweens;
    int freePosStart = -1;
    int freePosCount = 0;
    for (int i = 0; i < cnt; i++)
    {
        GTweener* tweener = _activeTweens[i];
        if (tweener == nullptr)
        {
            if (freePosStart == -1)
                freePosStart = i;
            freePosCount++;
        }
        else if (tweener->_killed)
        {
            tweener->_Reset();
            _tweenerPool.push_back(tweener);
            _activeTweens[i] = nullptr;

            if (freePosStart == -1)
                freePosStart = i;
            freePosCount++;
        }
        else
        {
            if (!tweener->_paused)
                tweener->_Update(dt);

            if (freePosStart != -1)
            {
                _activeTweens[freePosStart] = tweener;
                _activeTweens[i] = nullptr;
                freePosStart++;
            }
        }
    }

    if (freePosStart >= 0)
    {
        if (_totalActiveTweens != cnt) //new tweens added
        {
            int j = cnt;
            cnt = _totalActiveTweens - cnt;
            for (int i = 0; i < cnt; i++)
                _activeTweens[freePosStart++] = _activeTweens[j++];
        }
        _totalActiveTweens = freePosStart;
    }
}

void TweenManager::Clean()
{
    for (auto it = _tweenerPool.begin(); it != _tweenerPool.end(); it++)
        delete *it;
    _tweenerPool.clear();
}

void TweenManager::Init()
{
    _inited = true;

    _arrayLength = 30;
    _activeTweens = new GTweener*[_arrayLength];

    cocos2d::Director::getInstance()->getScheduler()->scheduleUpdate(&tweenEngine, INT_MIN + 10, false);
}

NS_FGUI_END