#include "GearColor.h"
#include "GObject.h"
#include "utils/ToolSet.h"
#include "UIPackage.h"

NS_FGUI_BEGIN
USING_NS_CC;

GearColor::GearColorValue::GearColorValue()
{

}

GearColor::GearColorValue::GearColorValue(const Color3B& color, const Color3B& strokeColor)
{
    this->color = color;
    this->outlineColor = strokeColor;
}

GearColor::GearColor(GObject * owner) :GearBase(owner), _tweener(nullptr)
{

}

GearColor::~GearColor()
{
    if (_tweener != nullptr)
        _tweener->kill();
}

void GearColor::init()
{
    IColorGear *cg = dynamic_cast<IColorGear*>(_owner);
    GTextField* tf = dynamic_cast<GTextField*>(_owner);

    _default = GearColorValue(cg->getColor(), tf != nullptr ? tf->getOutlineColor() : Color3B::BLACK);
    _storage.clear();
}

void GearColor::addStatus(const std::string&  pageId, const std::string& value)
{
    if (value == "-" || value.length() == 0)
        return;

    std::vector<std::string> arr;
    ToolSet::splitString(value, ',', arr);

    GearColorValue gv;
    gv.color = (Color3B)ToolSet::convertFromHtmlColor(arr[0].c_str());
    if (arr.size() == 1)
        gv.outlineColor = _default.outlineColor;
    else
        gv.outlineColor = (Color3B)ToolSet::convertFromHtmlColor(arr[1].c_str());

    if (pageId.size() == 0)
        _default = gv;
    else
        _storage[pageId] = gv;
}

void GearColor::apply()
{
    GearColorValue gv;
    auto it = _storage.find(_controller->getSelectedPageId());
    if (it != _storage.end())
        gv = it->second;
    else
        gv = _default;

    IColorGear *cg = dynamic_cast<IColorGear*>(_owner);
    GTextField* tf = dynamic_cast<GTextField*>(_owner);

    if (tween && UIPackage::_constructing == 0 && !disableAllTweenEffect)
    {
        if (tf != nullptr && gv.outlineColor != tf->getOutlineColor())
        {
            _owner->_gearLocked = true;
            tf->setOutlineColor(gv.outlineColor);
            _owner->_gearLocked = false;
        }

        if (_tweener != nullptr)
        {
            if (_tweener->endValue.getColor() != gv.color)
            {
                _tweener->kill(true);
                _tweener = nullptr;
            }
            else
                return;
        }

        Color3B curColor = cg->getColor();
        if (gv.color != curColor)
        {
            if (_owner->checkGearController(0, _controller))
                _displayLockToken = _owner->addDisplayLock();

            _tweener = GTween::to((Color4B)curColor, (Color4B)gv.color, tweenTime)
                ->setDelay(delay)
                ->setEase(easeType)
                ->setTargetAny(this)
                ->onUpdate(CC_CALLBACK_1(GearColor::onTweenUpdate, this))
                ->onComplete(CC_CALLBACK_0(GearColor::onTweenComplete, this));
        }
    }
    else
    {
        _owner->_gearLocked = true;
        cg->setColor(gv.color);
        if (tf != nullptr  && gv.outlineColor != tf->getOutlineColor())
            tf->setOutlineColor(gv.outlineColor);
        _owner->_gearLocked = false;
    }
}

void GearColor::onTweenUpdate(GTweener* tweener)
{
    IColorGear *cg = dynamic_cast<IColorGear*>(_owner);

    _owner->_gearLocked = true;
    cg->setColor((Color3B)_tweener->value.getColor());
    _owner->_gearLocked = false;
}

void GearColor::onTweenComplete()
{
    if (_displayLockToken != 0)
    {
        _owner->releaseDisplayLock(_displayLockToken);
        _displayLockToken = 0;
    }
    _tweener = nullptr;
    _owner->dispatchEvent(UIEventType::GearStop);
}

void GearColor::updateState()
{
    IColorGear *cg = dynamic_cast<IColorGear*>(_owner);
    GTextField* tf = dynamic_cast<GTextField*>(_owner);

    _storage[_controller->getSelectedPageId()] = GearColorValue(cg->getColor(), tf != nullptr ? tf->getOutlineColor() : _default.outlineColor);
}

NS_FGUI_END