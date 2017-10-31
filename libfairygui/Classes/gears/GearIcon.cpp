#include "GearIcon.h"
#include "GObject.h"
#include "utils/ToolSet.h"
#include "UIPackage.h"

NS_FGUI_BEGIN
USING_NS_CC;

GearIcon::GearIcon(GObject * owner) :GearBase(owner)
{

}

GearIcon::~GearIcon()
{
}

void GearIcon::init()
{
    _default = _owner->getIcon();
    _storage.clear();
}

void GearIcon::addStatus(const std::string&  pageId, const std::string& value)
{
    if (pageId.length() == 0)
        _default = value;
    else
        _storage[pageId] = value;
}

void GearIcon::apply()
{
    _owner->_gearLocked = true;

    auto it = _storage.find(_controller->getSelectedPageId());
    if (it != _storage.end())
        _owner->setIcon(it->second);
    else
        _owner->setIcon(_default);

    _owner->_gearLocked = false;
}

void GearIcon::updateState()
{
    _storage[_controller->getSelectedPageId()] = _owner->getIcon();
}

NS_FGUI_END