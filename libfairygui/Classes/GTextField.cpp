#include "GTextField.h"
#include "utils/ByteBuffer.h"

NS_FGUI_BEGIN
USING_NS_CC;

GTextField::GTextField()
    :_templateVars(nullptr),
    _ubbEnabled(false),
    _autoSize(AutoSizeType::BOTH)
{
}

GTextField::~GTextField()
{
    CC_SAFE_DELETE(_templateVars);
}

void GTextField::setText(const std::string & value)
{
    _text = value;
    setTextFieldText();
    updateGear(6);
    updateSize();
}

void GTextField::setUBBEnabled(bool value)
{
    if (_ubbEnabled != value)
    {
        _ubbEnabled = value;
        setTextFieldText();
        updateSize();
    }
}

void GTextField::setColor(const cocos2d::Color3B & value)
{
    getTextFormat()->color = value;
    applyTextFormat();
}

void GTextField::setFontSize(float value)
{
    getTextFormat()->fontSize = value;
    applyTextFormat();
}

void GTextField::setOutlineColor(const cocos2d::Color3B & value)
{
    getTextFormat()->outlineColor = value;
    applyTextFormat();
}

void GTextField::setTemplateVars(cocos2d::ValueMap* value)
{
    if (_templateVars == nullptr && value == nullptr)
        return;

    if (value == nullptr)
        CC_SAFE_DELETE(_templateVars);
    else
    {
        if (_templateVars == nullptr)
            _templateVars = new cocos2d::ValueMap();
        *_templateVars = *value;
    }

    flushVars();
}

GTextField* GTextField::setVar(const std::string& name, const cocos2d::Value& value)
{
    if (_templateVars == nullptr)
        _templateVars = new cocos2d::ValueMap();

    (*_templateVars)[name] = value;

    return this;
}

void GTextField::flushVars()
{
    setTextFieldText();
    updateSize();
}

void GTextField::updateSize()
{
}

void GTextField::setup_beforeAdd(ByteBuffer* buffer, int beginPos)
{
    GObject::setup_beforeAdd(buffer, beginPos);

    buffer->Seek(beginPos, 5);

    TextFormat* tf = getTextFormat();

    tf->face = buffer->ReadS();
    tf->fontSize = buffer->ReadShort();
    tf->color = (Color3B)buffer->ReadColor();
    tf->align = (TextHAlignment)buffer->ReadByte();
    tf->verticalAlign = (TextVAlignment)buffer->ReadByte();
    tf->lineSpacing = buffer->ReadShort();
    tf->letterSpacing = buffer->ReadShort();
    _ubbEnabled = buffer->ReadBool();
    setAutoSize((AutoSizeType)buffer->ReadByte());
    tf->underline = buffer->ReadBool();
    tf->italics = buffer->ReadBool();
    tf->bold = buffer->ReadBool();
    if (buffer->ReadBool())
        setSingleLine(true);
    if (buffer->ReadBool())
    {
        tf->outlineColor = (Color3B)buffer->ReadColor();
        tf->outlineSize = buffer->ReadFloat();
        tf->enableEffect(TextFormat::OUTLINE);
    }

    if (buffer->ReadBool())
    {
        tf->shadowColor = (Color3B)buffer->ReadColor();
        float f1 = buffer->ReadFloat();
        float f2 = buffer->ReadFloat();
        tf->shadowOffset = Vec2(f1, -f2);
        tf->enableEffect(TextFormat::SHADOW);
    }

    if (buffer->ReadBool())
        _templateVars = new cocos2d::ValueMap();
}

void GTextField::setup_afterAdd(ByteBuffer* buffer, int beginPos)
{
    GObject::setup_afterAdd(buffer, beginPos);

    applyTextFormat();

    buffer->Seek(beginPos, 6);

    const std::string& str = buffer->ReadS();
    if (!str.empty())
        setText(str);
}

std::string GTextField::parseTemplate(const char* text)
{
    const char* pString = text;

    ssize_t pos;
    ssize_t pos2;
    std::string tag, attr;
    std::string repl;
    std::string out;

    while (*pString != '\0')
    {
        const char* p = strchr(pString, '{');
        if (!p)
        {
            out.append(pString);
            break;
        }

        pos = p - pString;
        if (pos > 0 && *(p - 1) == '\\')
        {
            out.append(pString, pos - 1);
            out.append("{");
            pString += pos + 1;
            continue;
        }

        out.append(pString, pos);
        pString += pos;

        p = strchr(pString, '}');
        if (!p)
        {
            out.append(pString);
            break;
        }

        pos = p - pString;
        if (pos == 1)
        {
            out.append(pString, 0, 2);
            pString += 2;
            continue;
        }

        tag.assign(pString + 1, pos - 1);

        attr.clear();
        repl.clear();
        pos2 = tag.find('=');
        if (pos2 != -1)
        {
            auto it = _templateVars->find(tag.substr(0, pos2));
            if (it != _templateVars->end())
                out.append(it->second.asString());
            else
                out.append(tag.substr(pos2 + 1));
        }
        else
        {
            auto it = _templateVars->find(tag);
            if (it != _templateVars->end())
                out.append(it->second.asString());
        }
        pString += pos + 1;
    }
    return out;
}

//---------------------------

GBasicTextField::GBasicTextField() :
    _label(nullptr),
    _updatingSize(false)
{
    _touchDisabled = true;
}

GBasicTextField::~GBasicTextField()
{
}

void GBasicTextField::handleInit()
{
    _label = FUILabel::create();
    _label->retain();

    _displayObject = _label;
}

void GBasicTextField::applyTextFormat()
{
    _label->applyTextFormat();
    updateGear(4);
    if (!_underConstruct)
        updateSize();
}

void GBasicTextField::setAutoSize(AutoSizeType value)
{
    _autoSize = value;
    switch (value)
    {
    case AutoSizeType::NONE:
        _label->setOverflow(Label::Overflow::CLAMP);
        break;
    case AutoSizeType::BOTH:
        _label->setOverflow(Label::Overflow::NONE);
        break;
    case AutoSizeType::HEIGHT:
        _label->setOverflow(Label::Overflow::RESIZE_HEIGHT);
        break;
    case AutoSizeType::SHRINK:
        _label->setOverflow(Label::Overflow::SHRINK);
        break;
    }

    if (_autoSize == AutoSizeType::BOTH)
        _label->setDimensions(0, 0);
    else if (_autoSize == AutoSizeType::HEIGHT)
        _label->setDimensions(_size.width, 0);
    else
        _label->setDimensions(_size.width, _size.height);

    if (!_underConstruct)
        updateSize();
}

void GBasicTextField::setSingleLine(bool value)
{
    _label->enableWrap(!value);
    if (!_underConstruct)
        updateSize();
}

void GBasicTextField::setTextFieldText()
{
    if (_templateVars != nullptr)
        _label->setText(parseTemplate(_text.c_str()));
    else
        _label->setText(_text);
}

void GBasicTextField::updateSize()
{
    if (_updatingSize)
        return;

    _updatingSize = true;

    Size sz = _label->getContentSize();
    if (_autoSize == AutoSizeType::BOTH)
        setSize(sz.width, sz.height);
    else if (_autoSize == AutoSizeType::HEIGHT)
        setHeight(sz.height);

    _updatingSize = false;
}

void GBasicTextField::handleSizeChanged()
{
    if (_updatingSize)
        return;

    if (_autoSize != AutoSizeType::BOTH)
    {
        _label->setDimensions(_size.width, _size.height);

        if (_autoSize == AutoSizeType::HEIGHT)
        {
            if (!_text.empty())
                setSizeDirectly(_size.width, _label->getContentSize().height);
        }
    }
}

void GBasicTextField::handleGrayedChanged()
{
    GObject::handleGrayedChanged();

    _label->setGrayed(_finalGrayed);
}

NS_FGUI_END
