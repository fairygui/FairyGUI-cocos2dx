#include "utils/ToolSet.h"
#include "utils/SwitchHelper.h"

NS_FGUI_BEGIN
USING_NS_CC;
using namespace std;

static vector<string> helperArray;

void ToolSet::splitString(const string &s, char delim, vector<string> &elems)
{
    elems.clear();
    if (s.empty())
        return;

    stringstream ss(s);
    string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    if (s.back() == delim)
        elems.push_back(cocos2d::STD_STRING_EMPTY);
}

void ToolSet::splitString(const string &s, char delim, cocos2d::Vec2& value, bool intType)
{
    splitString(s, delim, helperArray);
    if (intType)
    {
        value.x = atoi(helperArray[0].c_str());
        if (helperArray.size() > 1)
            value.y = atoi(helperArray[1].c_str());
        else
            value.y = value.x;
    }
    else
    {
        value.x = atof(helperArray[0].c_str());
        if (helperArray.size() > 1)
            value.y = atof(helperArray[1].c_str());
        else
            value.y = value.x;
    }
}

void ToolSet::splitString(const string &s, char delim, cocos2d::Vec4& value, bool intType)
{
    splitString(s, delim, helperArray);
    if (intType)
    {
        value.x = atoi(helperArray[0].c_str());
        if (helperArray.size() > 1)
        {
            value.y = atoi(helperArray[1].c_str());
            if (helperArray.size() > 2)
            {
                value.z = atoi(helperArray[2].c_str());
                value.w = atoi(helperArray[3].c_str());
            }
        }
        else
            value.y = value.z = value.w = value.x;
    }
    else
    {
        value.x = atof(helperArray[0].c_str());
        if (helperArray.size() > 1)
        {
            value.y = atof(helperArray[1].c_str());
            if (helperArray.size() > 2)
            {
                value.z = atof(helperArray[2].c_str());
                value.w = atof(helperArray[3].c_str());
            }
        }
        else
            value.y = value.z = value.w = value.x;
    }
}

void ToolSet::splitString(const std::string & s, char delim, std::string & str1, std::string& str2)
{
    splitString(s, delim, helperArray);
    str1 = helperArray[0];
    if (helperArray.size() > 1)
        str2 = helperArray[1];
    else
        str2 = "";
}

int ToolSet::findInStringArray(const std::vector<std::string>& arr, const std::string& str)
{
    auto iter = std::find(arr.begin(), arr.end(), str);
    if (iter != arr.end())
        return (int)(iter - arr.begin());

    return -1;
}

Color4B ToolSet::convertFromHtmlColor(const char* str)
{
    ssize_t len = strlen(str);
    if (len < 7 || str[0] != '#')
        return Color4B::BLACK;

    char temp[3];

    if (len == 9)
    {
        return Color4B(strtol(strncpy(temp, str + 3, 2), NULL, 16),
            strtol(strncpy(temp, str + 5, 2), NULL, 16),
            strtol(strncpy(temp, str + 7, 2), NULL, 16),
            strtol(strncpy(temp, str + 1, 2), NULL, 16));
    }
    else
    {
        return Color4B(strtol(strncpy(temp, str + 1, 2), NULL, 16),
            strtol(strncpy(temp, str + 3, 2), NULL, 16),
            strtol(strncpy(temp, str + 5, 2), NULL, 16),
            255);
    }
}

#pragma warning(once:4307) 

PackageItemType ToolSet::parsePackageItemType(const char* p)
{
    if (!p)
        return PackageItemType::MISC;

    switch (hash_(p))
    {
    case "image"_hash:
        return PackageItemType::IMAGE;
    case "movieclip"_hash:
        return PackageItemType::MOVIECLIP;
    case "component"_hash:
        return PackageItemType::COMPONENT;
    case "atlas"_hash:
        return PackageItemType::ATLAS;
    case "sound"_hash:
        return PackageItemType::SOUND;
    case "font"_hash:
        return PackageItemType::FONT;
    case "misc"_hash:
        return PackageItemType::MISC;
    default:
        return PackageItemType::MISC;
    }
}

TextHAlignment ToolSet::parseAlign(const char *p)
{
    if (!p)
        return TextHAlignment::LEFT;

    switch (hash_(p))
    {
    case "left"_hash:
        return TextHAlignment::LEFT;
    case "center"_hash:
        return TextHAlignment::CENTER;
    case "right"_hash:
        return TextHAlignment::RIGHT;
    default:
        return TextHAlignment::LEFT;
    }
}

cocos2d::TextVAlignment ToolSet::parseVerticalAlign(const char * p)
{
    if (!p)
        return TextVAlignment::TOP;

    switch (hash_(p))
    {
    case "top"_hash:
        return TextVAlignment::TOP;
    case "middle"_hash:
        return TextVAlignment::CENTER;
    case "bottom"_hash:
        return TextVAlignment::BOTTOM;
    default:
        return TextVAlignment::TOP;
    }
}

int ToolSet::parseGearIndex(const char* p)
{
    if (!p || *p != 'g')
        return -1;

    switch (hash_(p))
    {
    case "gearDisplay"_hash:
        return 0;
    case "gearXY"_hash:
        return 1;
    case "gearSize"_hash:
        return 2;
    case "gearLook"_hash:
        return 3;
    case "gearColor"_hash:
        return 4;
    case "gearAni"_hash:
        return 5;
    case "gearText"_hash:
        return 6;
    case "gearIcon"_hash:
        return 7;
    default:
        return -1;
    }
}

LoaderFillType ToolSet::parseFillType(const char * p)
{
    if (!p)
        return LoaderFillType::NONE;

    switch (hash_(p))
    {
    case "none"_hash:
        return LoaderFillType::NONE;
    case "scale"_hash:
        return LoaderFillType::SCALE;
    case "scaleMatchHeight"_hash:
        return LoaderFillType::SCALE_MATCH_HEIGHT;
    case "scaleMatchWidth"_hash:
        return LoaderFillType::SCALE_MATCH_WIDTH;
    case "scaleFree"_hash:
        return LoaderFillType::SCALE_FREE;
    default:
        return LoaderFillType::NONE;
    }
}

ButtonMode ToolSet::parseButtonMode(const char * p)
{
    if (!p)
        return ButtonMode::COMMON;

    switch (hash_(p))
    {
    case "Check"_hash:
        return ButtonMode::CHECK;
    case "Radio"_hash:
        return ButtonMode::RADIO;
    default:
        return ButtonMode::COMMON;
    }
}

OverflowType ToolSet::parseOverflowType(const char * p)
{
    if (!p)
        return OverflowType::VISIBLE;

    switch (hash_(p))
    {
    case "visible"_hash:
        return OverflowType::VISIBLE;
    case "hidden"_hash:
        return OverflowType::HIDDEN;
    case "scroll"_hash:
        return OverflowType::SCROLL;
    default:
        return OverflowType::VISIBLE;
    }
}

ScrollType ToolSet::parseScrollType(const char * p)
{
    if (!p)
        return ScrollType::HORIZONTAL;

    switch (hash_(p))
    {
    case "horizontal"_hash:
        return ScrollType::HORIZONTAL;
    case "vertical"_hash:
        return ScrollType::VERTICAL;
    case "both"_hash:
        return ScrollType::BOTH;
    default:
        return ScrollType::HORIZONTAL;
    }
}

ScrollBarDisplayType ToolSet::parseScrollBarDisplayType(const char * p)
{
    if (!p)
        return ScrollBarDisplayType::DEFAULT;

    switch (hash_(p))
    {
    case "default"_hash:
        return ScrollBarDisplayType::DEFAULT;
    case "visible"_hash:
        return ScrollBarDisplayType::VISIBLE;
    case "auto"_hash:
        return ScrollBarDisplayType::AUTO;
    case "hidden"_hash:
        return ScrollBarDisplayType::HIDDEN;
    default:
        return ScrollBarDisplayType::DEFAULT;
    }
}

ProgressTitleType ToolSet::parseProgressTitleType(const char * p)
{
    if (!p)
        return ProgressTitleType::PERCENT;

    switch (hash_(p))
    {
    case "percent"_hash:
        return ProgressTitleType::PERCENT;
    case "valueAndmax"_hash:
        return ProgressTitleType::VALUE_MAX;
    case "value"_hash:
        return ProgressTitleType::VALUE;
    case "max"_hash:
        return ProgressTitleType::MAX;
    default:
        return ProgressTitleType::PERCENT;
    }
}

ListLayoutType ToolSet::parseListLayoutType(const char * p)
{
    if (!p)
        return ListLayoutType::SINGLE_COLUMN;

    switch (hash_(p))
    {
    case "column"_hash:
        return ListLayoutType::SINGLE_COLUMN;
    case "row"_hash:
        return ListLayoutType::SINGLE_ROW;
    case "flow_hz"_hash:
        return ListLayoutType::FLOW_HORIZONTAL;
    case "flow_vt"_hash:
        return ListLayoutType::FLOW_VERTICAL;
    case "pagination"_hash:
        return ListLayoutType::PAGINATION;
    default:
        return ListLayoutType::SINGLE_COLUMN;
    }
}

ListSelectionMode ToolSet::parseListSelectionMode(const char * p)
{
    if (!p)
        return ListSelectionMode::SINGLE;

    switch (hash_(p))
    {
    case "single"_hash:
        return ListSelectionMode::SINGLE;
    case "multiple"_hash:
        return ListSelectionMode::MULTIPLE;
    case "multipleSingleClick"_hash:
        return ListSelectionMode::MULTIPLE_SINGLECLICK;
    case "none"_hash:
        return ListSelectionMode::NONE;
    default:
        return ListSelectionMode::SINGLE;
    }
}

ChildrenRenderOrder ToolSet::parseChildrenRenderOrder(const char * p)
{
    if (!p)
        return ChildrenRenderOrder::ASCENT;

    switch (hash_(p))
    {
    case "ascent"_hash:
        return ChildrenRenderOrder::ASCENT;
    case "descent"_hash:
        return ChildrenRenderOrder::DESCENT;
    case "arch"_hash:
        return ChildrenRenderOrder::ARCH;
    default:
        return ChildrenRenderOrder::ASCENT;
    }
}

GroupLayoutType ToolSet::parseGroupLayoutType(const char * p)
{
    if (!p)
        return GroupLayoutType::NONE;

    switch (hash_(p))
    {
    case "hz"_hash:
        return GroupLayoutType::HORIZONTAL;
    case "vt"_hash:
        return GroupLayoutType::VERTICAL;
    default:
        return GroupLayoutType::NONE;
    }
}

PopupDirection ToolSet::parsePopupDirection(const char * p)
{
    if (!p)
        return PopupDirection::AUTO;

    switch (hash_(p))
    {
    case "auto"_hash:
        return PopupDirection::AUTO;
    case "up"_hash:
        return PopupDirection::UP;
    case "down"_hash:
        return PopupDirection::DOWN;
    default:
        return PopupDirection::AUTO;
    }
}

TextAutoSize ToolSet::parseTextAutoSize(const char * p)
{
    if (!p)
        return TextAutoSize::NONE;

    switch (hash_(p))
    {
    case "none"_hash:
        return TextAutoSize::NONE;
    case "both"_hash:
        return TextAutoSize::BOTH;
    case "height"_hash:
        return TextAutoSize::HEIGHT;
    case "shrink"_hash:
        return TextAutoSize::SHRINK;
    default:
        return TextAutoSize::NONE;
    }
}

FlipType ToolSet::parseFlipType(const char * p)
{
    if (!p)
        return FlipType::NONE;

    switch (hash_(p))
    {
    case "both"_hash:
        return FlipType::BOTH;
    case "hz"_hash:
        return FlipType::HORIZONTAL;
    case "vt"_hash:
        return FlipType::VERTICAL;
    default:
        return FlipType::NONE;
    }
}

TransitionActionType ToolSet::parseTransitionActionType(const char * p)
{
    if (!p)
        return TransitionActionType::Unknown;

    switch (hash_(p))
    {
    case "XY"_hash:
        return TransitionActionType::XY;
    case "Size"_hash:
        return TransitionActionType::Size;
    case "Scale"_hash:
        return TransitionActionType::Scale;
    case "Pivot"_hash:
        return TransitionActionType::Pivot;
    case "Alpha"_hash:
        return TransitionActionType::Alpha;
    case "Rotation"_hash:
        return TransitionActionType::Rotation;
    case "Color"_hash:
        return TransitionActionType::Color;
    case "Animation"_hash:
        return TransitionActionType::Animation;
    case "Visible"_hash:
        return TransitionActionType::Visible;
    case "Sound"_hash:
        return TransitionActionType::Sound;
    case "Transition"_hash:
        return TransitionActionType::Transition;
    case "Shake"_hash:
        return TransitionActionType::Shake;
    case "ColorFilter"_hash:
        return TransitionActionType::ColorFilter;
    case "Skew"_hash:
        return TransitionActionType::Skew;
    default:
        return TransitionActionType::Unknown;
    }
}

cocos2d::tweenfunc::TweenType ToolSet::parseEaseType(const char * p)
{
    if (!p)
        return tweenfunc::Expo_EaseOut;

    switch (hash_(p))
    {
    case "Linear"_hash:
        return tweenfunc::Linear;

    case "Elastic.In"_hash:
        return tweenfunc::Elastic_EaseIn;
    case "Elastic.Out"_hash:
        return tweenfunc::Elastic_EaseOut;
    case "Elastic.InOut"_hash:
        return tweenfunc::Elastic_EaseInOut;

    case "Quad.In"_hash:
        return tweenfunc::Quad_EaseIn;
    case "Quad.Out"_hash:
        return tweenfunc::Quad_EaseOut;
    case "Quad.InOut"_hash:
        return tweenfunc::Quad_EaseInOut;

    case "Cube.In"_hash:
        return tweenfunc::Cubic_EaseIn;
    case "Cube.Out"_hash:
        return tweenfunc::Cubic_EaseOut;
    case "Cube.InOut"_hash:
        return tweenfunc::Cubic_EaseInOut;

    case "Quart.In"_hash:
        return tweenfunc::Quart_EaseIn;
    case "Quart.Out"_hash:
        return tweenfunc::Quart_EaseOut;
    case "Quart.InOut"_hash:
        return tweenfunc::Quart_EaseInOut;

    case "Sine.In"_hash:
        return tweenfunc::Sine_EaseIn;
    case "Sine.Out"_hash:
        return tweenfunc::Sine_EaseOut;
    case "Sine.InOut"_hash:
        return tweenfunc::Sine_EaseInOut;

    case "Bounce.In"_hash:
        return tweenfunc::Bounce_EaseIn;
    case "Bounce.Out"_hash:
        return tweenfunc::Bounce_EaseOut;
    case "Bounce.InOut"_hash:
        return tweenfunc::Bounce_EaseInOut;

    case "Circ.In"_hash:
        return tweenfunc::Circ_EaseIn;
    case "Circ.Out"_hash:
        return tweenfunc::Circ_EaseOut;
    case "Circ.InOut"_hash:
        return tweenfunc::Circ_EaseInOut;

    case "Expo.In"_hash:
        return tweenfunc::Expo_EaseIn;
    case "Expo.Out"_hash:
        return tweenfunc::Expo_EaseOut;
    case "Expo.InOut"_hash:
        return tweenfunc::Expo_EaseInOut;

    case "Back.In"_hash:
        return tweenfunc::Back_EaseIn;
    case "Back.Out"_hash:
        return tweenfunc::Back_EaseOut;
    case "Back.InOut"_hash:
        return tweenfunc::Back_EaseInOut;

    default:
        return tweenfunc::Expo_EaseOut;
    }
}

#pragma warning(default:4307)

FastSplitter::FastSplitter() :data(nullptr), dataLength(-1), delimiter('\0')
{
}

void FastSplitter::start(const char * data, ssize_t dataLength, char delimiter)
{
    this->data = data;
    this->dataLength = dataLength;
    this->delimiter = delimiter;
    this->textLength = -1;
}

bool FastSplitter::next()
{
    if (dataLength < 0)
        return false;

    if (dataLength == 0)
    {
        dataLength = -1;
        textLength = 0;
        return true;
    }

    data += textLength + 1;
    char* found = (char*)memchr(data, (int)delimiter, dataLength);
    if (found)
        textLength = found - data;
    else
        textLength = dataLength;
    dataLength -= (textLength + 1);

    return true;
}

const char * FastSplitter::getText()
{
    if (textLength > 0)
        return data;
    else
        return nullptr;
}

ssize_t FastSplitter::getTextLength()
{
    return textLength;
}

void FastSplitter::getKeyValuePair(char* keyBuf, ssize_t keyBufSize, char* valueBuf, ssize_t valueBufSize)
{
    if (textLength == 0)
    {
        keyBuf[0] = '\0';
        valueBuf[0] = '\0';
    }
    else
    {
        char* found = (char*)memchr(data, (int)'=', textLength);
        if (found)
        {
            ssize_t len = MIN(keyBufSize - 1, found - data);
            memcpy(keyBuf, data, len);
            keyBuf[len] = '\0';

            len = MIN(valueBufSize - 1, textLength - (found - data) - 1);
            memcpy(valueBuf, found + 1, len);
            valueBuf[len] = '\0';
        }
        else
        {
            ssize_t len = MIN(valueBufSize - 1, textLength);
            memcpy(keyBuf, data, len);
            keyBuf[len] = '\0';
            valueBuf[0] = '\0';
        }
    }
}

NS_FGUI_END
