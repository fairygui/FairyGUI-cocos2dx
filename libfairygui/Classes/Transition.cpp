#include "Transition.h"
#include "GComponent.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
USING_NS_CC;

Transition::Transition(GComponent* owner)
{
    _owner = owner;
}


Transition::~Transition()
{
}

NS_FGUI_END