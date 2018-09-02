#include "GearSize.h"
#include "GObject.h"
#include "utils/ToolSet.h"
#include "UIPackage.h"

NS_FGUI_BEGIN
USING_NS_CC;

GearSize::GearSize(GObject * owner) :GearBase(owner), _tweener(nullptr)
{
}

GearSize::~GearSize()
{
    if (_tweener != nullptr)
        _tweener->Kill();
}

void GearSize::init()
{
    _default = Vec4(_owner->getWidth(), _owner->getHeight(),
        _owner->getScaleX(), _owner->getScaleY());
    _storage.clear();
}

void GearSize::addStatus(const std::string&  pageId, const std::string& value)
{
    if (value == "-" || value.length() == 0)
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

    if (tween && UIPackage::_constructing == 0 && !disableAllTweenEffect)
    {
        if (_tweener != nullptr)
        {
            if (_tweener->endValue.GetVec4() != gv)
            {
                _tweener->Kill(true);
                _tweener = nullptr;
            }
            else
                return;
        }

        bool a = gv.x != _owner->getWidth() || gv.y != _owner->getHeight();
        bool b = gv.z != _owner->getScaleX() || gv.w != _owner->getScaleY();
        if (a || b)
        {
            if (_owner->checkGearController(0, _controller))
                _displayLockToken = _owner->addDisplayLock();

            _tweener = GTween::To(Vec4(_owner->getWidth(), _owner->getHeight(), _owner->getScaleX(), _owner->getScaleY()), gv, tweenTime)
                ->SetDelay(delay)
                ->SetEase(easeType)
                ->SetTargetAny(this)
                ->SetUserData(Value((a ? 1 : 0) + (b ? 2 : 0)))
                ->OnUpdate(CC_CALLBACK_1(GearSize::onTweenUpdate, this))
                ->OnComplete(CC_CALLBACK_1(GearSize::onTweenComplete, this));
        }
    }
    else
    {
        _owner->_gearLocked = true;
        _owner->setSize(gv.x, gv.y, _owner->checkGearController(1, _controller));
        _owner->setScale(gv.z, gv.w);
        _owner->_gearLocked = false;
    }
}

void GearSize::onTweenUpdate(GTweener* tweener)
{
    int flag = tweener->GetUserData().asInt();
    _owner->_gearLocked = false;
    if ((flag & 1) != 0)
        _owner->setSize(tweener->value.x, tweener->value.y, _owner->checkGearController(1, _controller));
    if ((flag & 2) != 0)
        _owner->setScale(tweener->value.z, tweener->value.w);
    _owner->_gearLocked = false;
}

void GearSize::onTweenComplete(GTweener* tweener)
{
    if (_displayLockToken != 0)
    {
        _owner->releaseDisplayLock(_displayLockToken);
        _displayLockToken = 0;
    }
    _tweener = nullptr;
    _owner->dispatchEvent(UIEventType::GearStop);
}

void GearSize::updateState()
{
    _storage[_controller->getSelectedPageId()] = Vec4(_owner->getWidth(), _owner->getHeight(),
        _owner->getScaleX(), _owner->getScaleY());
}

void GearSize::updateFromRelations(float dx, float dy)
{
    if (_controller != nullptr && !_storage.empty())
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