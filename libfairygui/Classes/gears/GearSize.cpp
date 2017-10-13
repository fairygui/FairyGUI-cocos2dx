#include "GearSize.h"
#include "GObject.h"
#include "utils/ToolSet.h"
#include "UIPackage.h"

NS_FGUI_BEGIN
USING_NS_CC;

GearSize::GearSize(GObject * owner) :GearBase(owner)
{

}

GearSize::~GearSize()
{
}

void GearSize::init()
{
    _default = Vec4(_owner->getWidth(), _owner->getHeight(),
        _owner->getScaleX(), _owner->getScaleY());
    _storage.clear();
}

void GearSize::addStatus(const std::string&  pageId, const std::string& value)
{
    if (value == "-" || value.length() == 0) //历史遗留处理
        return;

    Vec4 v4;
    ToolSet::splitString(value, ',', v4);

    if (pageId.size() == 0)
        _default = v4;
    else
        _storage[pageId] = v4;
}

void GearSize::apply()
{
    Vec4 gv;
    auto it = _storage.find(_controller->getSelectedPageId());
    if (it != _storage.end())
        gv = it->second;
    else
        gv = _default;

    /*if (tween && UIPackage::_constructing == 0 && !disableAllTweenEffect)
    {

    }
    else*/
    {
        _owner->_gearLocked = true;
        _owner->setSize(gv.x, gv.y, _owner->checkGearController(1, _controller));
        _owner->setScale(gv.z, gv.w);
        _owner->_gearLocked = false;
    }
}

void GearSize::updateState()
{
    _storage[_controller->getSelectedPageId()] = Vec4(_owner->getWidth(), _owner->getHeight(),
        _owner->getScaleX(), _owner->getScaleY());
}

void GearSize::updateFromRelations(float dx, float dy)
{
    if (_controller != nullptr && _storage.size() > 0)
    {
        for (auto it = _storage.begin(); it != _storage.end(); ++it)
        {
            it->second = Vec4(it->second.x + dx, it->second.y + dy,
                it->second.z, it->second.w);
        }
        _default.x += dx;
        _default.y += dy;

        updateState();
    }
}


NS_FGUI_END