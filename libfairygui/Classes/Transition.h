#ifndef __TRANSITION_H__
#define __TRANSITION_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class GComponent;

class Transition : public cocos2d::Ref
{
public:
    Transition(GComponent* owner);
    ~Transition();

    GComponent* getOwner() { return _owner; }

private:
    GComponent* _owner;
};

NS_FGUI_END

#endif
