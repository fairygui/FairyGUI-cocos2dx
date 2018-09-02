#ifndef __TWEENMANAGER_H__
#define __TWEENMANAGER_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "TweenPropType.h"

NS_FGUI_BEGIN

class GTweener;

class TweenManager
{
public:
    static GTweener* CreateTween();
    static bool IsTweening(cocos2d::Ref* target, TweenPropType propType);
    static bool KillTweens(cocos2d::Ref* target, TweenPropType propType, bool completed);
    static GTweener* GetTween(cocos2d::Ref* target, TweenPropType propType);
    static void Update(float dt);
    static void Clean();
    static void Init();

private:
    static GTweener** _activeTweens;
    static std::vector<GTweener*> _tweenerPool;
    static int _totalActiveTweens;
    static int _arrayLength;
    static bool _inited;
};

NS_FGUI_END

#endif
