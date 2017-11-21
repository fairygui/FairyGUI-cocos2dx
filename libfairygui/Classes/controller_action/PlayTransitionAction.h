#ifndef __PLAYTRNASITIONACTION_H__
#define __PLAYTRNASITIONACTION_H__

#include "ControllerAction.h"

NS_FGUI_BEGIN

class Transition;

class PlayTransitionAction : public ControllerAction
{
public:
    PlayTransitionAction();
    virtual void setup(TXMLElement * xml) override;

    std::string transitionName;
    int repeat;
    float delay;
    bool stopOnExit;

protected:
    virtual void enter(GController* controller) override;
    virtual void leave(GController* controller) override;

    Transition* _currentTransition;
};

NS_FGUI_END

#endif
