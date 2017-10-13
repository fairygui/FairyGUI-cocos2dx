#include "GScrollBar.h"

NS_FGUI_BEGIN
USING_NS_CC;

GScrollBar::GScrollBar()
{
}

GScrollBar::~GScrollBar()
{

}

bool GScrollBar::init()
{
    if (!GComponent::init())
        return false;

    return true;
}

NS_FGUI_END