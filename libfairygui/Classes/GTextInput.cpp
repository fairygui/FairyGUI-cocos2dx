#include "GTextInput.h"

NS_FGUI_BEGIN
USING_NS_CC;

GTextInput::GTextInput()
{
}

GTextInput::~GTextInput()
{

}

bool GTextInput::init()
{
    if (!GTextField::init())
        return false;

    return true;
}

NS_FGUI_END