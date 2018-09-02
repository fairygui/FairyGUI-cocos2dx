#include "GearXY.h"
#include "GObject.h"
#include "utils/ToolSet.h"
#include "UIPackage.h"

NS_FGUI_BEGIN
USING_NS_CC;

GearXY::GearXY(GObject * owner) : GearBase(owner),_tweener(nullptr)
{

}

GearXY::~GearXY()
{
    if (_tweener != nullptr)
        _tweener->kill();
}

void GearXY::init()
{
    _default = Vec2(_owner->getX(), _owner->getY());
    _storage.clear();
}

void GearXY::addStatus(const std::string&  pageId, const std::string& value)
{
    if (value == "-" || value.length() == 0)
        return;

    Vec2 v2;
    ToolSet::splitString(value, ',', v2);

    if (pageId.size() == 0)
        _default = v2;
    else
        _storage[pageId] = v2;
}

void GearXY::apply()
{
    Vec2 gv;
    auto it = _storage.find(_controller->getSelectedPageId());
    if (it != _storage.end())
        gv = it->second;
    else
        gv = _default;

    if (tween && UIPackage::_constructing == 0 && !disableAllTweenEffect)
    {
        if (_tweener != nullptr)
        {
            if (_tweener->endValue.x != gv.x || _tweener->endValue.y != gv.y)
            {
                _tweener->kill(true);
                _tweener = nullptr;
            }
            else
                return;
        }

        if (_owner->getX() != gv.x || _owner->getY() != gv.y)
        {
            if (_owner->checkGearController(0, _controller))
                _displayLockToken = _owner->addDisplayLock();

            _tweener = GTween::to(_owner->getPosition(), gv, tweenTime)
                ->setDelay(delay)
                ->setEase(easeType)
                ->setTargetAny(this)
                ->onUpdate(CC_CALLBACK_1(GearXY::onTweenUpdate, this))
                ->onComplete(CC_CALLBACK_0(GearXY::onTweenComplete, this));
        }
    }
    else
    {
        _owner->_gearLocked = true;
        _owner->setPosition(gv.x, gv.y);
        _owner->_gearLocked = false;
    }
}

void GearXY::onTweenUpdate(GTweener* tweener)
{
    _owner->_gearLocked = true;
    _owner->setPosition(_tweener->value.x, _tweener->value.y);
    _owner->_gearLocked = false;
}

void GearXY::onTweenComplete()
{
    if (_displayLockToken != 0)
    {
        _owner->releaseDisplayLock(_displayLockToken);
        _displayLockToken = 0;
    }
    _tweener = nullptr;
    _owner->dispatchEvent(UIEventType::GearStop);
}

void GearXY::updateState()
{
    _storage[_controller->getSelectedPageId()] = Vec2(_owner->getX(), _owner->getY());
}

void GearXY::updateFromRelations(float dx, float dy)
{
    if (_controller != nullptr && !_storage.empty())
    {
        for (auto it = _storage.begin(); it != _storage.end(); ++it)
        {
            it->second = Vec2(it->second.x + dx, it->second.y + dy);
        }
        _default.x += dx;
        _default.y += dy;

        updateState();
    }
}

NS_FGUI_END