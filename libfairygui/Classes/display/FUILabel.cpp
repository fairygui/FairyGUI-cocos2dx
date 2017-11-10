#include "FUILabel.h"
#include "UIConfig.h"
#include "UIPackage.h"
#include "BitmapFont.h"

NS_FGUI_BEGIN
USING_NS_CC;

FUILabel::FUILabel()
{
}

FUILabel::~FUILabel()
{
}

void FUILabel::setFontName(const std::string & value)
{
    _fontName = value;
    Label::LabelType oldType = _currentLabelType;

    if (value.find("ui://") != -1)
    {
        setBMFontFilePath(value);
    }
    else
    {
        bool ttf = false;
        const std::string& fontName = UIConfig::getRealFontName(value, ttf);
        if (ttf)
        {
            _fontConfig.fontFilePath = fontName;
            _fontConfig.fontSize = _fontSize;
            setTTFConfig(_fontConfig);
        }
        else
        {
            setSystemFontName(fontName);
        }

        if (oldType == LabelType::BMFONT)
            setTextColor(_textColor);
    }
}

void FUILabel::setFontSize(int value)
{
    if (_fontSize != value)
    {
        _fontSize = value;
        if (_currentLabelType == LabelType::STRING_TEXTURE)
        {
            setSystemFontSize(value);
        }
        else if (_currentLabelType == LabelType::BMFONT)
        {
            setBMFontSize(value);
        }
        else
        {
            _fontConfig.fontSize = value;
            setTTFConfig(_fontConfig);
        }
    }
}

void FUILabel::setTextColor(const Color4B & color)
{
    if (_currentLabelType == LabelType::BMFONT)
        _textColor = color;
    else
        Label::setTextColor(color);
}

bool FUILabel::setBMFontFilePath(const std::string & bmfontFilePath, const Vec2 & imageOffset, float fontSize)
{
    PackageItem* pi = UIPackage::getItemByURL(bmfontFilePath);
    BitmapFont* bmFont;
    if (!pi || !(bmFont = pi->bitmapFont))
    {
        reset();
        return false;
    }

    //assign the default fontSize
    if (std::abs(fontSize) < FLT_EPSILON) {
        float originalFontSize = bmFont->getOriginalFontSize();
        _bmFontSize = originalFontSize / CC_CONTENT_SCALE_FACTOR();
    }

    if (fontSize > 0.0f && bmFont->isResizable()) {
        _bmFontSize = fontSize;
    }

    _bmFontPath = bmfontFilePath;

    _currentLabelType = LabelType::BMFONT;
    setFontAtlas(bmFont->createFontAtlas());

    return true;
}

void FUILabel::updateBMFontScale()
{
    auto font = _fontAtlas->getFont();
    if (_currentLabelType == LabelType::BMFONT) {
        BitmapFont *bmFont = (BitmapFont*)font;
        float originalFontSize = bmFont->getOriginalFontSize();
        _bmfontScale = _bmFontSize * CC_CONTENT_SCALE_FACTOR() / originalFontSize;
    }
    else {
        _bmfontScale = 1.0f;
    }
}

NS_FGUI_END