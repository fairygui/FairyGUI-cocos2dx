#include "TextFormat.h"

NS_FGUI_BEGIN
USING_NS_CC;

TextFormat::TextFormat() :
    fontSize(12),
    color(Color3B::BLACK),
    bold(false),
    italics(false),
    underline(false),
    lineSpacing(3),
    letterSpacing(0),
    align(TextHAlignment::LEFT),
    verticalAlign(TextVAlignment::TOP),
    effect(0),
    outlineSize(1),
    shadowBlurRadius(0),
    _hasColor(false)
{
}

void TextFormat::setFormat(const TextFormat & format)
{
    *this = format;
}

NS_FGUI_END