#include "GearColor.h"
#include "GObject.h"
#include "utils/ToolSet.h"
#include "UIPackage.h"

NS_FGUI_BEGIN
USING_NS_CC;

GearColor::GearColorValue::GearColorValue()
{

}

GearColor::GearColorValue::GearColorValue(const Color4B& color, const Color4B& strokeColor)
{
    this->color = color;
    this->outlineColor = strokeColor;
}

GearColor::GearColor(GObject * owner) :GearBase(owner)
{

}

GearColor::~GearColor()
{
}

void GearColor::init()
{
    IColorGear *cg = dynamic_cast<IColorGear*>(_owner);

    _default = GearColorValue(cg->cg_getColor(), cg->cg_getOutlineColor());
    _storage.clear();
}

void GearColor::addStatus(const std::string&  pageId, const std::string& value)
{
    if (value == "-" || value.length() == 0) //历史遗留处理
        return;

    std::vector<std::string> arr;
    ToolSet::splitString(value, ',', arr);

    GearColorValue gv;
    gv.color = ToolSet::convertFromHtmlColor(arr[0].c_str());
    if (arr.size() == 1)
        gv.outlineColor = Color4B(0, 0, 0, 0);
    else
        gv.outlineColor = ToolSet::convertFromHtmlColor(arr[1].c_str());

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

    if (tween && UIPackage::_constructing == 0 && !disableAllTweenEffect)
    {
        if (gv.outlineColor.a > 0)
        {
            _owner->_gearLocked = true;
            cg->cg_setOutlineColor(gv.outlineColor);
            _owner->_gearLocked = false;
        }

        if (_owner->displayObject()->getActionByTag(ActionTag::COLOR_ACTION) != nullptr)
        {
            if (_tweenTarget.x != gv.color.r || _tweenTarget.y != gv.color.g || _tweenTarget.z != gv.color.b)
            {
                _owner->displayObject()->stopActionByTag(ActionTag::COLOR_ACTION);
                onTweenComplete();
            }
            else
                return;
        }

        if (gv.color != cg->cg_getColor())
        {
            if (_owner->checkGearController(0, _controller))
                _displayLockToken = _owner->addDisplayLock();
            _tweenTarget.set(gv.color.r, gv.color.g, gv.color.b, gv.color.a);
            const Color4B& curColor = cg->cg_getColor();

            ActionInterval* action = ActionVec4::create(tweenTime,
                Vec4(curColor.r, curColor.g, curColor.b, curColor.a),
                _tweenTarget,
                CC_CALLBACK_1(GearColor::onTweenUpdate, this),
                CC_CALLBACK_0(GearColor::onTweenComplete, this));
            action = createEaseAction(easeType, action);
            if (delay > 0)
                action = Sequence::createWithTwoActions(DelayTime::create(delay), action);
            action->setTag(ActionTag::COLOR_ACTION);
            _owner->displayObject()->runAction(action);
        }
    }
    else
    {
        _owner->_gearLocked = true;
        cg->cg_setColor(gv.color);
        if (gv.outlineColor.a > 0)
            cg->cg_setOutlineColor(gv.outlineColor);
        _owner->_gearLocked = false;
    }
}

void GearColor::onTweenUpdate(const Vec4& v)
{
    IColorGear *cg = dynamic_cast<IColorGear*>(_owner);

    _owner->_gearLocked = true;
    cg->cg_setColor(Color4B(v.x, v.y, v.z, v.w));
    _owner->_gearLocked = false;
}

void GearColor::onTweenComplete()
{
    if (_displayLockToken != 0)
    {
        _owner->releaseDisplayLock(_displayLockToken);
        _displayLockToken = 0;
    }
    _owner->dispatchEvent(UIEventType::GearStop);
}

void GearColor::updateState()
{
    IColorGear *cg = dynamic_cast<IColorGear*>(_owner);
    _storage[_controller->getSelectedPageId()] = GearColorValue(cg->cg_getColor(), cg->cg_getOutlineColor());
}

NS_FGUI_END