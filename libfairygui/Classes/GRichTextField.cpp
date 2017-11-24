#include "GRichTextField.h"

NS_FGUI_BEGIN
USING_NS_CC;

GRichTextField::GRichTextField() :
    _richText(nullptr),
    _updatingSize(false),
    _autoSize(TextAutoSize::BOTH)
{
}

GRichTextField::~GRichTextField()
{
}

void GRichTextField::handleInit()
{
    _richText = FUIRichText::create();
    _richText->retain();
    _richText->setCascadeOpacityEnabled(true);

    _displayObject = _richText;
}

void GRichTextField::setText(const std::string & value)
{
    _richText->setText(value);
    if (!_underConstruct)
        updateSize();
}

void GRichTextField::applyTextFormat()
{
    _richText->applyTextFormat();
    updateGear(4);
    if (!_underConstruct)
        updateSize();
}

void GRichTextField::setAutoSize(TextAutoSize value)
{
    _autoSize = value;
    switch (value)
    {
    case TextAutoSize::NONE:
        _richText->setOverflow(Label::Overflow::CLAMP);
        break;
    case TextAutoSize::BOTH:
        _richText->setOverflow(Label::Overflow::NONE);
        break;
    case TextAutoSize::HEIGHT:
        _richText->setOverflow(Label::Overflow::RESIZE_HEIGHT);
        break;
    case TextAutoSize::SHRINK:
        _richText->setOverflow(Label::Overflow::SHRINK);
        break;
    }

    _richText->setDimensions(_size.width, _size.height);
    if (!_underConstruct)
        updateSize();
}

void GRichTextField::setSingleLine(bool value)
{
}

void GRichTextField::updateSize()
{
    if (_updatingSize)
        return;

    _updatingSize = true;

    Size sz = _richText->getContentSize();
    if (_autoSize == TextAutoSize::BOTH)
        setSize(sz.width, sz.height);
    else if (_autoSize == TextAutoSize::HEIGHT)
        setHeight(sz.height);

    _updatingSize = false;
}

void GRichTextField::handleSizeChanged()
{
    if (_updatingSize)
        return;

    if (_autoSize != TextAutoSize::BOTH)
    {
        _richText->setDimensions(_size.width, _size.height);

        if (_autoSize == TextAutoSize::HEIGHT)
        {
            if (_richText->getText().length() > 0)
                setSizeDirectly(_size.width, _richText->getContentSize().height);
        }
    }
}

void GRichTextField::setup_AfterAdd(TXMLElement* xml)
{
    GTextField::setup_AfterAdd(xml);

    updateSize();
}

NS_FGUI_END
