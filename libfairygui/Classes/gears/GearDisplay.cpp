#include "GearDisplay.h"
#include "Controller.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
USING_NS_CC;

GearDisplay::GearDisplay(GObject * owner) :GearBase(owner)
{
    _displayLockToken = 1;
}

GearDisplay::~GearDisplay()
{
}

void GearDisplay::apply()
{
    _displayLockToken++;
    if (_displayLockToken == 0)
        _displayLockToken = 1;

    if (pages.size() == 0
        || ToolSet::findInStringArray(pages, _controller->getSelectedPageId()) != -1)
        _visible = 1;
    else
        _visible = 0;
}

void GearDisplay::updateState()
{
}

void GearDisplay::addStatus(const std::string&  pageId, const std::string& value)
{
}

void GearDisplay::init()
{
    pages.clear();
}

uint32_t GearDisplay::addLock()
{
    _visible++;
    return _displayLockToken;
}

void GearDisplay::releaseLock(uint32_t token)
{
    if (token == _displayLockToken)
        _visible--;
}

bool GearDisplay::isConnected()
{
    return _controller == nullptr || _visible > 0;
}

NS_FGUI_END