#include "FUIRichText.h"

NS_FGUI_BEGIN
USING_NS_CC;

FUIRichText::FUIRichText()
{
}

FUIRichText::~FUIRichText()
{
}

bool FUIRichText::init()
{
    if(!RichText::init())
        return false;

    this->ignoreContentAdaptWithSize(false);
    return true;
}

void FUIRichText::setText(const std::string & value)
{
    _richElements.clear();
    _text = value;

    if (value.empty())
        return;

    ui::RichElementText* element = ui::RichElementText::create(0, getFontColor3B(), 255, _text,
        getFontFace(), getFontSize());
    pushBackElement(element);
}

NS_FGUI_END