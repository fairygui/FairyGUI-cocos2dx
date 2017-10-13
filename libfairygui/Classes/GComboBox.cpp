#include "GComboBox.h"

NS_FGUI_BEGIN
USING_NS_CC;

GComboBox::GComboBox()
{
}

GComboBox::~GComboBox()
{

}

bool GComboBox::init()
{
    if (!GComponent::init())
        return false;

    return true;
}

NS_FGUI_END