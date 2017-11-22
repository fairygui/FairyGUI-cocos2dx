#include "GRichTextField.h"

NS_FGUI_BEGIN
USING_NS_CC;

GRichTextField::GRichTextField()
{
}

GRichTextField::~GRichTextField()
{

}

void GRichTextField::handleInit()
{
    _richText = FUIRichText::create();
    _richText->retain();
    _richText->setOpenUrlHandler(CC_CALLBACK_1(GRichTextField::handleURL, this));

    setFontName(UIConfig::defaultFont);
    setColor(Color3B::BLACK);
    setFontSize(12);
    _displayObject = _richText;
}

void GRichTextField::setText(const std::string & value)
{
    _richText->setText(value);
}

void GRichTextField::setFontName(const std::string & value)
{
    bool ttf;
    _richText->setFontFace(UIConfig::getRealFontName(value, ttf));
}

void GRichTextField::setFontSize(int value)
{
    _richText->setFontSize(value);
}

void GRichTextField::setColor(const cocos2d::Color3B & value)
{
    _color = value;
    _richText->setFontColor(_richText->stringWithColor3B(value));
    updateGear(4);
}

void GRichTextField::setAlign(cocos2d::TextHAlignment value)
{
    _richText->setHorizontalAlignment((ui::RichText::HorizontalAlignment)value);
}

void GRichTextField::handleURL(const std::string & url)
{
    bubbleEvent(UIEventType::ClickLink, nullptr, Value(url));
}

void GRichTextField::handleSizeChanged()
{
    _richText->setContentSize(_size);
}

NS_FGUI_END
