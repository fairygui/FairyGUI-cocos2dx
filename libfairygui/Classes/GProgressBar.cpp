#include "GProgressBar.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
USING_NS_CC;

GProgressBar::GProgressBar() :
    _max(100),
    _value(0),
    _titleType(ProgressTitleType::PERCENT),
    _titleObject(nullptr),
    _barObjectH(nullptr),
    _barObjectV(nullptr),
    _barMaxWidth(0),
    _barMaxHeight(0),
    _barMaxWidthDelta(0),
    _barMaxHeightDelta(0),
    _barStartX(0),
    _barStartY(0)
{
}

GProgressBar::~GProgressBar()
{

}

void GProgressBar::setTitleType(ProgressTitleType value)
{
    if (_titleType != value)
    {
        _titleType = value;
        update(_value);
    }
}

void GProgressBar::setMax(double value)
{
    if (_max != value)
    {
        _max = value;
        update(_value);
    }
}

void GProgressBar::setValue(double value)
{
    if (_value != value)
    {
        _value = value;
        update(_value);
    }
}

void GProgressBar::tweenValue(double value, float duration)
{
    if (_value != value)
    {
        _displayObject->stopActionByTag(ActionTag::PROGRESS_ACTION);

        float oldValue = (float)_value;
        _value = value;
        Action* action = ActionFloat::create(duration, oldValue, value,
            [this](float v) { update(v); });
        action->setTag(ActionTag::PROGRESS_ACTION);
        _displayObject->runAction(action);
    }
}

void GProgressBar::update(double newValue)
{
    float percent = MIN(newValue / _max, 1);

    if (_titleObject != nullptr)
    {
        std::ostringstream oss;
        switch (_titleType)
        {
        case ProgressTitleType::PERCENT:
            oss << round(percent * 100) << "%";
            break;

        case ProgressTitleType::VALUE_MAX:
            oss << round(newValue) << "/" << round(_max);
            break;

        case ProgressTitleType::VALUE:
            oss << newValue;
            break;

        case ProgressTitleType::MAX:
            oss << _max;
            break;
        }
        _titleObject->setText(oss.str());
    }

    float fullWidth = this->getWidth() - _barMaxWidthDelta;
    float fullHeight = this->getHeight() - _barMaxHeightDelta;
    if (!_reverse)
    {
        if (_barObjectH != nullptr)
        {
            /*if (dynamic_cast<GImage*>(_barObjectH) && ((GImage *)_barObjectH)->getFillMethod() != FillMethod::None)
            ((GImage *)_barObjectH).fillAmount = percent;
            else if (dynamic_cast<GLoader*>(_barObjectH) && ((GLoader*)_barObjectH)->getFillMethod() != FillMethod::None)
            ((GLoader *)_barObjectH).fillAmount = percent;
            else*/
            _barObjectH->setWidth(round(fullWidth * percent));
        }
        if (_barObjectV != nullptr)
        {
            /*if (dynamic_cast<GImage*>(_barObjectV) && ((GImage *)_barObjectV)->getFillMethod() != FillMethod::None)
            ((GImage *)_barObjectV).fillAmount = percent;
            else if (dynamic_cast<GLoader*>(_barObjectV) && ((GLoader*)_barObjectV)->getFillMethod() != FillMethod::None)
            ((GLoader *)_barObjectV).fillAmount = percent;
            else*/
            _barObjectV->setHeight(round(fullHeight * percent));
        }
    }
    else
    {
        if (_barObjectH != nullptr)
        {
            /*if (dynamic_cast<GImage*>(_barObjectH) && ((GImage *)_barObjectH)->getFillMethod() != FillMethod::None)
            ((GImage *)_barObjectH).fillAmount = 1 - percent;
            else if (dynamic_cast<GLoader*>(_barObjectH) && ((GLoader*)_barObjectH)->getFillMethod() != FillMethod::None)
            ((GLoader *)_barObjectH).fillAmount = 1 - percent;
            else*/
            {
                _barObjectH->setWidth(round(fullWidth * percent));
                _barObjectH->setX(_barStartX + (fullWidth - _barObjectH->getWidth()));
            }
        }
        if (_barObjectV != nullptr)
        {
            /*if (dynamic_cast<GImage*>(_barObjectV) && ((GImage *)_barObjectV)->getFillMethod() != FillMethod::None)
            ((GImage *)_barObjectV).fillAmount = 1 - percent;
            else if (dynamic_cast<GLoader*>(_barObjectV) && ((GLoader*)_barObjectV)->getFillMethod() != FillMethod::None)
            ((GLoader *)_barObjectV).fillAmount = 1 - percent;
            else*/
            {
                _barObjectV->setHeight(round(fullHeight * percent));
                _barObjectV->setY(_barStartY + (fullHeight - _barObjectV->getHeight()));
            }
        }
    }
}

void GProgressBar::handleSizeChanged()
{
    GComponent::handleSizeChanged();

    if (_barObjectH != nullptr)
        _barMaxWidth = getWidth() - _barMaxWidthDelta;
    if (_barObjectV != nullptr)
        _barMaxHeight = getHeight() - _barMaxHeightDelta;

    if (!_underConstruct)
        update(_value);
}

void GProgressBar::constructFromXML(TXMLElement * xml)
{
    xml = xml->FirstChildElement("ProgressBar");
    CCASSERT(xml, "type mismatch");

    const char *p;
    p = xml->Attribute("titleType");
    if (p)
        _titleType = ToolSet::parseProgressTitleType(p);
    _reverse = xml->BoolAttribute("reverse");

    _titleObject = getChild("title");
    _barObjectH = getChild("bar");
    _barObjectV = getChild("bar_v");

    if (_barObjectH != nullptr)
    {
        _barMaxWidth = _barObjectH->getWidth();
        _barMaxWidthDelta = getWidth() - _barMaxWidth;
        _barStartX = _barObjectH->getX();
    }
    if (_barObjectV != nullptr)
    {
        _barMaxHeight = _barObjectV->getHeight();
        _barMaxHeightDelta = getHeight() - _barMaxHeight;
        _barStartY = _barObjectV->getY();
    }
}

void GProgressBar::setup_AfterAdd(TXMLElement * xml)
{
    GComponent::setup_AfterAdd(xml);

    xml = xml->FirstChildElement("ProgressBar");
    if (xml != nullptr)
    {
        int tmp;
        if (xml->QueryIntAttribute("value", &tmp) == 0)
            _value = tmp;

        if (xml->QueryIntAttribute("max", &tmp) == 0)
            _max = tmp;
    }
    update(_value);
}

NS_FGUI_END