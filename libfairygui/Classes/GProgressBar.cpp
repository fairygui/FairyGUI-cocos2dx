#include "GProgressBar.h"

NS_FGUI_BEGIN
USING_NS_CC;

GProgressBar::GProgressBar()
{
}

GProgressBar::~GProgressBar()
{

}

bool GProgressBar::init()
{
    if (!GComponent::init())
        return false;

    return true;
}

NS_FGUI_END