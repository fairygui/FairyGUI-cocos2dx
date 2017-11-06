#include "FUIRichText.h"
#include "utils/UBBParser.h"
#include "utils/SwitchHelper.h"
#include "utils/ToolSet.h"
#include "UIPackage.h"

NS_FGUI_BEGIN
USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

class FUIXMLVisitor : public SAXDelegator
{
public:
    enum class StyleLine {
        NONE,
        UNDERLINE,          /*!< underline */
        STRIKETHROUGH       /*!< a typographical presentation of words with a horizontal line through their center */
    };

    enum class StyleEffect {
        NONE,
        OUTLINE,            /*!< outline effect enabled */
        SHADOW,             /*!< shadow effect enabled */
        GLOW                /*!< glow effect enabled @discussion Limiting use to only when the Label created with true type font. */
    };

    struct TextFormat
    {
        std::string face;                       /*!< font name */
        std::string url;                        /*!< url is a attribute of a anchor tag */
        float fontSize;                         /*!< font size */
        Color3B color;                          /*!< font color */
        bool hasColor;                          /*!< or color is specified? */
        bool bold;                              /*!< bold text */
        bool italics;                           /*!< italic text */
        StyleLine line;                         /*!< underline or strikethrough */
        StyleEffect effect;                     /*!< outline, shadow or glow */
        Color3B outlineColor;                   /*!< the color of the outline */
        int outlineSize;                        /*!< the outline effect size value */
        Color3B shadowColor;                    /*!< the shadow effect color value */
        cocos2d::Size shadowOffset;             /*!< shadow effect offset value */
        int shadowBlurRadius;                   /*!< the shadow effect blur radius */
        Color3B glowColor;                      /*!< the glow effect color value */

        TextFormat()
            : fontSize(-1)
            , hasColor(false)
            , bold(false)
            , italics(false)
            , line(StyleLine::NONE)
            , effect(StyleEffect::NONE)
        {
        }
    };

    explicit FUIXMLVisitor(RichText* richText);
    virtual ~FUIXMLVisitor();

    void startElement(void *ctx, const char *name, const char **atts) override;
    void endElement(void *ctx, const char *name) override;
    void textHandler(void *ctx, const char *s, size_t len) override;

private:
    ValueMap tagAttrMapWithXMLElement(const char ** attrs);

    void pushTextFormat();
    void popTextFormat();
    void addNewLine(bool check);
    void finishTextBlock();

    RichText* _richText;
    std::vector<TextFormat> _textFormatStack;
    TextFormat _format;
    size_t _textFormatStackTop;
    int _skipText;
    bool _ignoreWhiteSpace;
    std::string _textBlock;
    RichElement* _lastElement;
};

FUIXMLVisitor::FUIXMLVisitor(RichText* richText)
    : _richText(richText),
    _textFormatStackTop(0),
    _skipText(0),
    _ignoreWhiteSpace(false),
    _lastElement(nullptr)
{
    _format.face = _richText->getFontFace();
    _format.color = _richText->getFontColor3B();
    _format.fontSize = _richText->getFontSize();
}

FUIXMLVisitor::~FUIXMLVisitor()
{
}

void FUIXMLVisitor::pushTextFormat()
{
    if (_textFormatStack.size() <= _textFormatStackTop)
        _textFormatStack.push_back(_format);
    else
        _textFormatStack[_textFormatStackTop] = _format;
    _textFormatStackTop++;
}

void FUIXMLVisitor::popTextFormat()
{
    if (_textFormatStackTop > 0)
    {
        _format = _textFormatStack[_textFormatStackTop - 1];
        _textFormatStackTop--;
    }
}

void FUIXMLVisitor::addNewLine(bool check)
{
    if (check)
    {
        if (dynamic_cast<RichElementNewLine*>(_lastElement))
            return;
    }
    RichElementNewLine* element = RichElementNewLine::create(0, _format.color, 255);
    _richText->pushBackElement(element);
}

void FUIXMLVisitor::finishTextBlock()
{
    if (!_textBlock.empty())
    {
        RichElementText* element = RichElementText::create(0, _format.color, 255, _textBlock,
            _format.face, _format.fontSize, _format.url.empty() ? 0 : RichElementText::URL_FLAG, _format.url, _format.outlineColor, _format.outlineSize,
            _format.shadowColor, _format.shadowOffset, _format.shadowBlurRadius, _format.glowColor);
        _richText->pushBackElement(element);
        _textBlock.clear();
    }
}

#pragma warning(once:4307)
void FUIXMLVisitor::startElement(void* /*ctx*/, const char *elementName, const char **atts)
{
    finishTextBlock();

    switch (hash_(elementName))
    {
    case "b"_hash:
        pushTextFormat();
        _format.bold = true;
        break;
    case "i"_hash:
        pushTextFormat();
        _format.italics = true;
        break;

    case "u"_hash:
        pushTextFormat();
        _format.line = FUIXMLVisitor::StyleLine::UNDERLINE;
        break;

    case "sub"_hash:
        break;

    case "sup"_hash:
        break;

    case "font"_hash:
    {
        pushTextFormat();
        ValueMap&& tagAttrValueMap = tagAttrMapWithXMLElement(atts);
        auto it = tagAttrValueMap.find("size");
        if (it != tagAttrValueMap.end())
            _format.fontSize = it->second.asInt();

        it = tagAttrValueMap.find("color");
        if (it != tagAttrValueMap.end())
        {
            _format.color = _richText->color3BWithString(it->second.asString());
            _format.hasColor = true;
        }
        break;
    }

    case "br"_hash:
    {
        addNewLine(false);
        break;
    }

    case "img"_hash:
    {
        std::string src;
        ValueMap&& tagAttrValueMap = tagAttrMapWithXMLElement(atts);

        int height = -1;
        int width = -1;
        Widget::TextureResType resType = Widget::TextureResType::LOCAL;

        auto it = tagAttrValueMap.find("height");
        if (it != tagAttrValueMap.end()) {
            height = it->second.asInt();
        }
        it = tagAttrValueMap.find("width");
        if (it != tagAttrValueMap.end()) {
            width = it->second.asInt();
        }
        it = tagAttrValueMap.find("type");
        if (it != tagAttrValueMap.end()) {
            // texture type
            // 0: normal file path
            // 1: sprite frame name
            int type = it->second.asInt();
            resType = type == 0 ? Widget::TextureResType::LOCAL : Widget::TextureResType::PLIST;
        }
        it = tagAttrValueMap.find("src");
        if (it != tagAttrValueMap.end()) {
            src = it->second.asString();
            if (src.find("ui://") != -1)
            {
                src = UIPackage::normalizeURL(src);
                PackageItem* pi = UIPackage::getItemByURL(src);
                if (pi && pi->spriteFrame && !pi->addedToCache)
                {
                    SpriteFrameCache::getInstance()->addSpriteFrame(pi->spriteFrame, src);
                    if (width < 0)
                        width = pi->width;
                    if (height < 0)
                        height = pi->height;
                }
                else
                    src.clear();
            }
        }

        if (!src.empty()) {
            RichElementImage* element = RichElementImage::create(0, Color3B::WHITE, 255, src, "", resType);
            if (0 <= height) element->setHeight(height);
            if (0 <= width)  element->setWidth(width);
            _richText->pushBackElement(element);
        }
        break;
    }

    case "a"_hash:
    {
        pushTextFormat();

        std::string href;
        ValueMap&& tagAttrValueMap = tagAttrMapWithXMLElement(atts);
        auto it = tagAttrValueMap.find("href");
        if (it != tagAttrValueMap.end())
            href = it->second.asString();
        else
            href = "";

        if (_richText->isAnchorTextUnderlineEnabled())
            _format.line = StyleLine::UNDERLINE;
        if (!_format.hasColor)
            _format.color = _richText->getAnchorFontColor3B();
        _format.url = href;
        break;
    }

    case "p"_hash:
    case "ui"_hash:
    case "div"_hash:
    case "li"_hash:
        addNewLine(true);
        break;

    case "html"_hash:
    case "body"_hash:
        //full html
        _ignoreWhiteSpace = true;
        break;

    case "head"_hash:
    case "style"_hash:
    case "script"_hash:
    case "form"_hash:
        _skipText++;
        break;
    }
}

void FUIXMLVisitor::endElement(void* /*ctx*/, const char *elementName)
{
    finishTextBlock();

    switch (hash_(elementName))
    {
    case "b"_hash:
    case "i"_hash:
    case "u"_hash:
    case "font"_hash:
    case "a"_hash:
        popTextFormat();
        break;

    case "head"_hash:
    case "style"_hash:
    case "script"_hash:
    case "form"_hash:
        _skipText--;
        break;
    }
}
#pragma warning(default:4307)

void FUIXMLVisitor::textHandler(void* /*ctx*/, const char *str, size_t len)
{
    if (_skipText == 0)
    {
        if (!_ignoreWhiteSpace)
        {
            FastSplitter fs;
            fs.start(str, len, '\n');
            bool first = true;
            while (fs.next())
            {
                if (!first)
                {
                    finishTextBlock();

                    RichElementNewLine* element = RichElementNewLine::create(0, _format.color, 255);
                    _richText->pushBackElement(element);
                    continue;
                }
                _textBlock += string(fs.getText(), fs.getTextLength());
                first = false;
            }
        }
        else
            _textBlock += string(str, len);
    }
}

ValueMap FUIXMLVisitor::tagAttrMapWithXMLElement(const char ** attrs)
{
    ValueMap tagAttrValueMap;
    for (const char** attr = attrs; *attr != nullptr; attr = (attrs += 2)) {
        if (attr[0] && attr[1]) {
            tagAttrValueMap[attr[0]] = attr[1];
        }
    }
    return tagAttrValueMap;
}

FUIRichText::FUIRichText() :_ubbEnabled(false)
{
}

FUIRichText::~FUIRichText()
{
}

bool FUIRichText::init()
{
    if (!RichText::init())
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

    string parsedText;
    if (_ubbEnabled)
        parsedText = UBBParser::getInstance()->parse(_text.c_str());
    else
        parsedText = _text;
    parsedText = "<dummy>" + parsedText + "</dummy>";
    FUIXMLVisitor visitor(this);
    SAXParser parser;
    parser.setDelegator(&visitor);
    parser.parseIntrusive(&parsedText.front(), parsedText.length());
}

NS_FGUI_END