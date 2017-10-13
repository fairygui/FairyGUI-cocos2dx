#include "GList.h"

NS_FGUI_BEGIN
USING_NS_CC;

GList::GList()
{
}

GList::~GList()
{

}

bool GList::init()
{
    if (!GComponent::init())
        return false;

    return true;
}

NS_FGUI_END