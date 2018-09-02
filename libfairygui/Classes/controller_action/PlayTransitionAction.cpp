#include "PlayTransitionAction.h"
#include "Controller.h"
#include "GComponent.h"

NS_FGUI_BEGIN
USING_NS_CC;

PlayTransitionAction::PlayTransitionAction() :
    repeat(1), delay(0), stopOnExit(false), _currentTransition(nullptr)
{
}

void PlayTransitionAction::setup(TXMLElement * xml)
{
    ControllerAction::setup(xml);

    const char *p;
    p = xml->Attribute("transition");
    if (p)
        transitionName = p;

    p = xml->Attribute("repeat");
    if (p)
        repeat = atoi(p);

    p = xml->Attribute("delay");
    if (p)
        delay = atof(p);

    stopOnExit = xml->BoolAttribute("stopOnExit");
}

void PlayTransitionAction::enter(GController * controller)
{
    Transition* trans = controller->getParent()->getTransition(transitionName);
    if (trans != nullptr)
    {
        if (_currentTransition != nullptr && _currentTransition->isPlaying())
            trans->changePlayTimes(repeat);
        else
            trans->play(repeat, delay, nullptr);
        _currentTransition = trans;
    }
}

void PlayTransitionAction::leave(GController * controller)
{
    if (stopOnExit && _currentTransition != nullptr)
    {
        _currentTransition->stop();
        _currentTransition = nullptr;
    }
}

NS_FGUI_END