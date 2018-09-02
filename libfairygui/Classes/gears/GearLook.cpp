#include "GearLook.h"
#include "GObject.h"
#include "utils/ToolSet.h"
#include "UIPackage.h"

NS_FGUI_BEGIN
USING_NS_CC;

GearLook::GearLookValue::GearLookValue() :alpha(0), rotation(0), grayed(false), touchable(false)
{

}

GearLook::GearLookValue::GearLookValue(float alpha, float rotation, bool grayed, bool touchable)
{
    this->alpha = alpha;
    this->rotation = rotation;
    this->grayed = grayed;
    this->touchable = touchable;
}

GearLook::GearLook(GObject * owner) :GearBase(owner), _tweener(nullptr)
{

}

GearLook::~GearLook()
{
    if (_tweener != nullptr)
        _tweener->Kill();
}

void GearLook::init()
{
    _default = GearLookValue(_owner->getAlpha(), _owner->getRotation(),
        _owner->isGrayed(), _owner->isTouchable());
    _storage.clear();
}

void GearLook::addStatus(const std::string&  pageId, const std::string& value)
{
    if (value == "-" || value.length() == 0)
        return;

    std::vector<std::string> arr;
    ToolSet::splitString(value, ',', arr);

    GearLookValue gv;
    gv.alpha = atof(arr[0].c_str());
    gv.rotation = atof(arr[1].c_str());
    gv.grayed = arr[2] == "1";
    if (arr.size() > 3)
        gv.touchable = arr[3] == "1";

    if (pageId.size() == 0)
        _default = gv;
    else
        _storage[pageId] = gv;
}

void GearLook::apply()
{
    GearLookValue gv;
    auto it = _storage.find(_controller->getSelectedPageId());
    if (it != _storage.end())
        gv = it->second;
    else
        gv = _default;

    if (tween && UIPackage::_constructing == 0 && !disableAllTweenEffect)
    {
        if (_tweener != nullptr)
        {
            if (_tweener->endValue.x != gv.alpha || _tweener->endValue.y != gv.rotation)
            {
                _tweener->Kill(true);
                _tweener = nullptr;
            }
            else
                return;
        }

        bool a = gv.alpha != _owner->getAlpha();
        bool b = gv.rotation != _owner->getRotation();
        if (a || b)
        {
            if (_owner->checkGearController(0, _controller))
                _displayLockToken = _owner->addDisplayLock();

            _tweener = GTween::To(Vec2(_owner->getAlpha(), _owner->getRotation()), Vec2(gv.alpha, gv.rotation), tweenTime)
                ->SetDelay(delay)
                ->SetEase(easeType)
                ->SetTargetAny(this)
                ->SetUserData(Value((a ? 1 : 0) + (b ? 2 : 0)))
                ->OnUpdate(CC_CALLBACK_1(GearLook::onTweenUpdate, this))
                ->OnComplete(CC_CALLBACK_1(GearLook::onTweenComplete, this));
        }
    }
    else
    {
        _owner->_gearLocked = true;
        _owner->setAlpha(gv.alpha);
        _owner->setRotation(gv.rotation);
        _owner->setGrayed(gv.grayed);
        _owner->setTouchable(gv.touchable);
        _owner->_gearLocked = false;
    }
}

void GearLook::onTweenUpdate(GTweener* tweener)
{
    int flag = _tweener->GetUserData().asInt();
    _owner->_gearLocked = false;
    if ((flag & 1) != 0)
        _owner->setAlpha(_tweener->value.x);
    if ((flag & 2) != 0)
        _owner->setRotation(_tweener->value.y);
    _owner->_gearLocked = false;
}

void GearLook::onTweenComplete(GTweener* tweener)
{
    if (_displayLockToken != 0)
    {
        _owner->releaseDisplayLock(_displayLockToken);
        _displayLockToken = 0;
    }
    _tweener = nullptr;
    _owner->dispatchEvent(UIEventType::GearStop);
}

void GearLook::updateState()
{
    _storage[_controller->getSelectedPageId()] = GearLookValue(_owner->getAlpha(), _owner->getRotation(),
        _owner->isGrayed(), _owner->isTouchable());
}


NS_FGUI_END