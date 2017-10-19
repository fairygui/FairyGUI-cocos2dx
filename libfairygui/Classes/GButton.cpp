#include "GButton.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
USING_NS_CC;

using namespace tinyxml2;

GButton::GButton() :
    _mode(ButtonMode::BM_COMMON),
    _titleObject(nullptr),
    _iconObject(nullptr),
    _buttonController(nullptr),
    _relatedController(nullptr),
    _selected(false),
    _over(false),
    _down(false),
    _downEffect(0),
    _downScaled(false),
    _downEffectValue(0)
{
}

GButton::~GButton()
{

}

void GButton::setTitle(const std::string & value)
{
    _title = value;
    if (_titleObject != nullptr)
        _titleObject->setText((_selected && _selectedTitle.length() > 0) ? _selectedTitle : _title);
    updateGear(6);
}

void GButton::setIcon(const std::string & value)
{
    _icon = value;
    if (_iconObject != nullptr)
        _iconObject->setIcon((_selected && _selectedIcon.length() > 0) ? _selectedIcon : _icon);
    updateGear(7);
}

void GButton::setSelected(bool value)
{
    if (_mode == ButtonMode::BM_COMMON)
        return;

    if (_selected != value)
    {
        _selected = value;
        setCurrentState();
        if (_selectedTitle.size() > 0 && _titleObject != nullptr)
            _titleObject->setText(_selected ? _selectedTitle : _title);
        if (_selectedIcon.size() > 0)
        {
            const string& str = _selected ? _selectedIcon : _icon;
            if (_iconObject != nullptr)
                _iconObject->setIcon(str);
        }
        if (_relatedController != nullptr
            && getParent() != nullptr
            && !getParent()->_buildingDisplayList)
        {
            if (_selected)
            {
                _relatedController->setSelectedPageId(_relatedPageId);
                if (_relatedController->autoRadioGroupDepth)
                    getParent()->adjustRadioGroupDepth(this, _relatedController);
            }
            else if (_mode == ButtonMode::BM_CHECK && _relatedController->getSelectedPageId().compare(_relatedPageId) == 0)
                _relatedController->setOppositePageId(_relatedPageId);
        }
    }
}

void GButton::setRelatedController(Controller * c)
{
    _relatedController = c;
}

void GButton::constructFromXML(tinyxml2::XMLElement * xml)
{
    GComponent::constructFromXML(xml);

    xml = xml->FirstChildElement("Button");
    if (xml)
    {
        const char*p;
        p = xml->Attribute("mode");
        if (p)
            _mode = ToolSet::parseButtonMode(p);
    }

    _buttonController = getController("button");
    _titleObject = getChild("title");
    _iconObject = getChild("icon");
    if (_titleObject != nullptr)
        _title = _titleObject->getText();
    if (_iconObject != nullptr)
        _icon = _iconObject->getIcon();

    if (_mode == ButtonMode::BM_COMMON)
        setState(UP);

    addEventListener(UIEventType::RollOver, CC_CALLBACK_1(GButton::onRollOver, this));
    addEventListener(UIEventType::RollOut, CC_CALLBACK_1(GButton::onRollOut, this));
    addEventListener(UIEventType::TouchBegin, CC_CALLBACK_1(GButton::onTouchBegin, this));
    addEventListener(UIEventType::TouchEnd, CC_CALLBACK_1(GButton::onTouchEnd, this));
    addEventListener(UIEventType::Click, CC_CALLBACK_1(GButton::onClick, this));
}

void GButton::setup_AfterAdd(tinyxml2::XMLElement * xml)
{
    GComponent::setup_AfterAdd(xml);

    xml = xml->FirstChildElement("Button");
    if (!xml)
        return;

    const char*p;

    p = xml->Attribute("title");
    if (p)
        this->setTitle(p);

    p = xml->Attribute("icon");
    if (p)
        this->setIcon(p);

    p = xml->Attribute("controller");
    if (p)
        _relatedController = getParent()->getController(p);

    p = xml->Attribute("page");
    if (p)
        _relatedPageId = p;

    setSelected(xml->BoolAttribute("checked"));
}

bool GButton::init()
{
    if (!GComponent::init())
        return false;

    return true;
}

void GButton::setState(const std::string& value)
{
    if (_buttonController != nullptr)
        _buttonController->setSelectedPage(value);

    if (_downEffect == 1)
    {

    }
    else if (_downEffect == 2)
    {
        if (value == DOWN || value == SELECTED_OVER || value == SELECTED_DISABLED)
        {
            if (!_downScaled)
            {
                _downScaled = true;
                setScale(getScaleX() * _downEffectValue, getScaleY() * _downEffectValue);
            }
        }
        else
        {
            if (_downScaled)
            {
                _downScaled = false;
                setScale(getScaleX() / _downEffectValue, getScaleY() / _downEffectValue);
            }
        }
    }
}

void GButton::setCurrentState()
{
    if (isGrayed() && _buttonController != nullptr && _buttonController->hasPage(DISABLED))
    {
        if (_selected)
            setState(SELECTED_DISABLED);
        else
            setState(DISABLED);
    }
    else
    {
        if (_selected)
            setState(_over ? SELECTED_OVER : DOWN);
        else
            setState(_over ? OVER : UP);
    }
}

void GButton::handleControllerChanged(Controller* c)
{
    GObject::handleControllerChanged(c);

    if (_relatedController == c)
        setSelected(_relatedPageId.compare(c->getSelectedPageId()) == 0);
}

void GButton::onRollOver(EventContext* context)
{
    if (_buttonController == nullptr || !_buttonController->hasPage(OVER))
        return;

    _over = true;
    if (_down)
        return;

    if (isGrayed() && _buttonController->hasPage(DISABLED))
        return;

    setState(_selected ? SELECTED_OVER : OVER);
}

void GButton::onRollOut(EventContext* context)
{
    if (_buttonController == nullptr || !_buttonController->hasPage(OVER))
        return;

    _over = false;
    if (_down)
        return;

    if (isGrayed() && _buttonController->hasPage(DISABLED))
        return;

    setState(_selected ? DOWN : UP);
}

void GButton::onTouchBegin(EventContext* context)
{
    _down = true;
    context->captureTouch();

    if (_mode == ButtonMode::BM_COMMON)
    {
        if (isGrayed() && _buttonController != nullptr && _buttonController->hasPage(DISABLED))
            setState(SELECTED_DISABLED);
        else
            setState(DOWN);
    }
}

void GButton::onTouchEnd(EventContext* context)
{
    if (_down)
    {
        _down = false;
        if (_mode == ButtonMode::BM_COMMON)
        {
            if (isGrayed() && _buttonController != nullptr && _buttonController->hasPage(DISABLED))
                setState(DISABLED);
            else if (_over)
                setState(OVER);
            else
                setState(UP);
        }
        else
        {
            if (!_over
                && _buttonController != nullptr
                && (_buttonController->getSelectedPage() == OVER
                    || _buttonController->getSelectedPage() == SELECTED_OVER))
            {
                setCurrentState();
            }
        }
    }
}

void GButton::onRemoved()
{
    if (_over)
        onRollOut(nullptr);
}

void GButton::onClick(EventContext* context)
{
    //if (sound != null)
    //	Stage.inst.PlayOneShotSound(sound, soundVolumeScale);

    if (_mode == ButtonMode::BM_CHECK)
    {
        if (changeStateOnClick)
        {
            setSelected(!_selected);
            dispatchEvent(UIEventType::StatusChange);
        }
    }
    else if (_mode == ButtonMode::BM_RADIO)
    {
        if (changeStateOnClick && !_selected)
        {
            setSelected(true);
            dispatchEvent(UIEventType::StatusChange);
        }
    }
    else
    {
        if (_relatedController != nullptr)
            _relatedController->setSelectedPageId(_relatedPageId);
    }
}

NS_FGUI_END
