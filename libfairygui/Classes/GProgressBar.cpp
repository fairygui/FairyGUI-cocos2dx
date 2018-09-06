#include "GProgressBar.h"
#include "PackageItem.h"
#include "utils/ByteBuffer.h"
#include "tween/GTween.h"

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
    _barStartY(0),
    _tweening(false)
{
}

GProgressBar::~GProgressBar()
{
    if (_tweening)
        GTween::kill(this);
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
    if (_tweening)
    {
        GTween::kill(this, TweenPropType::Progress, true);
        _tweening = false;
    }

    if (_value != value)
    {
        _value = value;
        update(_value);
    }
}

void GProgressBar::tweenValue(double value, float duration)
{
    double oldValule = _value;
    _value = value;

    if (_tweening)
        GTween::kill(this, TweenPropType::Progress, false);
    _tweening = true;
    GTween::toDouble(oldValule, _value, duration)
        ->setEase(EaseType::Linear)
        ->setTarget(this, TweenPropType::Progress)
        ->onComplete([this]() { _tweening = false; });
}

void GProgressBar::update(double newValue)
{
    float percent = _max != 0 ? MIN(newValue / _max, 1) : 0;

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

void GProgressBar::constructExtension(ByteBuffer* buffer)
{
    buffer->Seek(0, 6);

    _titleType = (ProgressTitleType)buffer->ReadByte();
    _reverse = buffer->ReadBool();

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

void GProgressBar::setup_afterAdd(ByteBuffer* buffer, int beginPos)
{
    GComponent::setup_afterAdd(buffer, beginPos);

    if (!buffer->Seek(beginPos, 6))
    {
        update(_value);
        return;
    }

    if ((ObjectType)buffer->ReadByte() != _packageItem->objectType)
    {
        update(_value);
        return;
    }

    _value = buffer->ReadInt();
    _max = buffer->ReadInt();

    update(_value);
}

NS_FGUI_END