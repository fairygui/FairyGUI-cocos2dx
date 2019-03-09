#include "FUILabel.h"
#include "UIConfig.h"
#include "UIPackage.h"
#include "BitmapFont.h"

NS_FGUI_BEGIN
USING_NS_CC;

FUILabel::FUILabel() :
    _fontSize(-1),
    _bmFontCanTint(false),
    _textFormat(new TextFormat())
{
}

FUILabel::~FUILabel()
{
    delete _textFormat;
}

void FUILabel::setText(const std::string & value)
{
    if (_fontSize < 0)
        applyTextFormat();

    setString(value);
}

void FUILabel::applyTextFormat()
{
    if (_fontSize < 0 /**first time**/ || _fontName != _textFormat->face)
    {
        _fontName = _textFormat->face;
        Label::LabelType oldType = _currentLabelType;

        if (_fontName.find("ui://") != -1)
        {
            setBMFontFilePath(_fontName);
        }
        else
        {
            bool ttf = false;
            const std::string& fontName = UIConfig::getRealFontName(_fontName, &ttf);
            if (ttf)
            {
                _fontConfig.fontFilePath = fontName;
                _fontConfig.fontSize = _textFormat->fontSize;
                setTTFConfig(_fontConfig);
            }
            else
            {
                setSystemFontName(fontName);
            }

            if (oldType == LabelType::BMFONT)
                setTextColor((Color4B)_textFormat->color);
        }
    }

    if (_fontSize != _textFormat->fontSize)
    {
        _fontSize = _textFormat->fontSize;
        if (_currentLabelType == LabelType::STRING_TEXTURE)
        {
            setSystemFontSize(_fontSize);
        }
        else if (_currentLabelType == LabelType::BMFONT)
        {
            setBMFontSize(_fontSize);
        }
        else
        {
            _fontConfig.fontSize = _fontSize;
            setTTFConfig(_fontConfig);
        }
    }

    if (_currentLabelType != LabelType::BMFONT)
        setTextColor((Color4B)_textFormat->color);
    else if (_bmFontCanTint)
        setColor(_textFormat->color);

    if (_textFormat->underline)
        enableUnderline();
    else
        disableEffect(LabelEffect::UNDERLINE);

    if (_textFormat->italics)
        enableItalics();
    else
        disableEffect(LabelEffect::ITALICS);

    if (_textFormat->bold)
        enableBold();
    else
        disableEffect(LabelEffect::BOLD);

    setLineSpacing(_textFormat->lineSpacing);
    setHorizontalAlignment(_textFormat->align);
    setVerticalAlignment(_textFormat->verticalAlign);

    if (_textFormat->hasEffect(TextFormat::OUTLINE))
        enableOutline((Color4B)_textFormat->outlineColor, _textFormat->outlineSize);
    else
        disableEffect(LabelEffect::OUTLINE);

    if (_textFormat->hasEffect(TextFormat::SHADOW))
        enableShadow((Color4B)_textFormat->shadowColor, _textFormat->shadowOffset);
    else if(!_textFormat->bold)
        disableEffect(LabelEffect::SHADOW);
}

bool FUILabel::setBMFontFilePath(const std::string & bmfontFilePath, const Vec2 & imageOffset, float fontSize)
{
    BitmapFont* bmFont = (BitmapFont*)UIPackage::getItemAssetByURL(bmfontFilePath, PackageItemType::FONT);
    if (bmFont == nullptr)
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
    _bmFontCanTint = bmFont->canTint();

    _currentLabelType = LabelType::BMFONT;
    setFontAtlas(bmFont->createFontAtlas());

    return true;
}

void FUILabel::setGrayed(bool value)
{
    if (_fontAtlas == nullptr)
        return;

    GLProgramState *glState = nullptr;
    if (value)
        glState = GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_GRAYSCALE, _fontAtlas->getTexture(0));
    else
        glState = GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP, _fontAtlas->getTexture(0));

    setGLProgramState(glState);
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