#include "GearAnimation.h"
#include "GObject.h"
#include "utils/ToolSet.h"
#include "UIPackage.h"

NS_FGUI_BEGIN
USING_NS_CC;

GearAnimation::GearAnimation(GObject * owner) :GearBase(owner)
{

}

GearAnimation::~GearAnimation()
{
}

void GearAnimation::init()
{
}

void GearAnimation::addStatus(const std::string&  pageId, const std::string& value)
{
    if (value == "-" || value.length() == 0) //历史遗留处理
        return;
}

void GearAnimation::apply()
{

}

void GearAnimation::updateState()
{

}

NS_FGUI_END