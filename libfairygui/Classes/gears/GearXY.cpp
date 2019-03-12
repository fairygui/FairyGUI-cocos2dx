#include "GearXY.h"
#include "GObject.h"
#include "UIPackage.h"
#include "utils/ByteBuffer.h"
#include "tween/GTween.h"

NS_FGUI_BEGIN
USING_NS_CC;

GearXY::GearXY(GObject * owner) : GearBase(owner)
{

}

GearXY::~GearXY()
{
}

void GearXY::init()
{
    _default = Vec2(_owner->getX(), _owner->getY());
    _storage.clear();
}

void GearXY::addStatus(const std::string&  pageId, ByteBuffer* buffer)
{
    Vec2 gv;
    gv.x = buffer->ReadInt();
    gv.y = buffer->ReadInt();

    if (pageId.size() == 0)
        _default = gv;
    else
        _storage[pageId] = gv;
}

void GearXY::apply()
{
    Vec2 gv;
    auto it = _storage.find(_controller->getSelectedPageId());
    if (it != _storage.end())
        gv = it->second;
    else
        gv = _default;

    if (_tweenConfig && UIPackage::_constructing == 0 && !disableAllTweenEffect)
    {
        if (_tweenConfig->_tweener != nullptr)
        {
            if (_tweenConfig->_tweener->endValue.x != gv.x || _tweenConfig->_tweener->endValue.y != gv.y)
            {
                _tweenConfig->_tweener->kill(true);
                _tweenConfig->_tweener = nullptr;
            }
            else
                return;
        }

        if (_owner->getX() != gv.x || _owner->getY() != gv.y)
        {
            if (_owner->checkGearController(0, _controller))
                _tweenConfig->_displayLockToken = _owner->addDisplayLock();

            _tweenConfig->_tweener = GTween::to(_owner->getPosition(), gv, _tweenConfig->duration)
                ->setDelay(_tweenConfig->delay)
                ->setEase(_tweenConfig->easeType)
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
    _owner->setPosition(_tweenConfig->_tweener->value.x, _tweenConfig->_tweener->value.y);
    _owner->_gearLocked = false;
}

void GearXY::onTweenComplete()
{
    if (_tweenConfig->_displayLockToken != 0)
    {
        _owner->releaseDisplayLock(_tweenConfig->_displayLockToken);
        _tweenConfig->_displayLockToken = 0;
    }
    _tweenConfig->_tweener = nullptr;
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