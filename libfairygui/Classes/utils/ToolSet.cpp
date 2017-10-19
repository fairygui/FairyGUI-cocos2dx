#include "utils/ToolSet.h"

NS_FGUI_BEGIN
USING_NS_CC;
using namespace std;

vector<string> helperArray;

vector<std::string> &ToolSet::splitString(const string &s, char delim, vector<string> &elems) {
    elems.clear();
    stringstream ss(s);
    string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    if (s.back() == delim)
        elems.push_back(cocos2d::STD_STRING_EMPTY);

    return elems;
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
            value.z = atoi(helperArray[2].c_str());
            value.w = atoi(helperArray[3].c_str());
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
            value.z = atof(helperArray[2].c_str());
            value.w = atof(helperArray[3].c_str());
        }
        else
            value.y = value.z = value.w = value.x;
    }
}

void ToolSet::splitString(const std::string & s, char delim, std::string & str1, std::string str2)
{
    splitString(s, delim, helperArray);
    str1 = helperArray[0];
    if (helperArray.size() > 1)
        str2 = helperArray[1];
    else
        str2 = str1;
}

ssize_t ToolSet::findInStringArray(const std::vector<std::string>& arr, const std::string str)
{
    auto iter = std::find(arr.begin(), arr.end(), str);
    if (iter != arr.end())
        return iter - arr.begin();

    return -1;
}

Color4B ToolSet::convertFromHtmlColor(const char* str)
{
    int len = strlen(str);
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


typedef std::uint64_t hash_t;

constexpr hash_t prime = 0x100000001B3ull;
constexpr hash_t basis = 0xCBF29CE484222325ull;

hash_t hash_(char const* str)
{
    hash_t ret{ basis };

    while (*str) {
        ret ^= *str;
        ret *= prime;
        str++;
    }

    return ret;
}

constexpr hash_t hash_compile_time(char const* str, hash_t last_value = basis)
{
    return *str ? hash_compile_time(str + 1, (*str ^ last_value) * prime) : last_value;
}

constexpr hash_t operator "" _hash(char const* p, size_t)
{
    return hash_compile_time(p);
}

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

FillType ToolSet::parseFillType(const char * p)
{
    if (!p)
        return FillType::FT_NONE;

    switch (hash_(p))
    {
    case "none"_hash:
        return FillType::FT_NONE;
    case "scale"_hash:
        return FillType::FT_SCALE;
    case "scaleMatchHeight"_hash:
        return FillType::FT_SCALE_MATCH_HEIGHT;
    case "scaleMatchWidth"_hash:
        return FillType::FT_SCALE_MATCH_WIDTH;
    case "scaleFree"_hash:
        return FillType::FT_SCALE_FREE;
    default:
        return FillType::FT_NONE;
    }
}

ButtonMode ToolSet::parseButtonMode(const char * p)
{
    if (!p)
        return ButtonMode::BM_COMMON;

    switch (hash_(p))
    {
    case "Check"_hash:
        return ButtonMode::BM_CHECK;
    case "Radio"_hash:
        return ButtonMode::BM_RADIO;
    default:
        return ButtonMode::BM_COMMON;
    }
}

OverflowType ToolSet::parseOverflowType(const char * p)
{
    if (!p)
        return OverflowType::OF_VISIBLE;

    switch (hash_(p))
    {
    case "visible"_hash:
        return OverflowType::OF_VISIBLE;
    case "hidden"_hash:
        return OverflowType::OF_HIDDEN;
    case "scroll"_hash:
        return OverflowType::OF_SCROLL;
    default:
        return OverflowType::OF_VISIBLE;
    }
}

ScrollType ToolSet::parseScrollType(const char * p)
{
    if (!p)
        return ScrollType::ST_HORIZONTAL;

    switch (hash_(p))
    {
    case "horizontal"_hash:
        return ScrollType::ST_HORIZONTAL;
    case "vertical"_hash:
        return ScrollType::ST_VERTICAL;
    case "both"_hash:
        return ScrollType::ST_BOTH;
    default:
        return ScrollType::ST_HORIZONTAL;
    }
}

ScrollBarDisplayType ToolSet::parseScrollBarDisplayType(const char * p)
{
    if (!p)
        return ScrollBarDisplayType::SBD_DEFAULT;

    switch (hash_(p))
    {
    case "default"_hash:
        return ScrollBarDisplayType::SBD_DEFAULT;
    case "visible"_hash:
        return ScrollBarDisplayType::SBD_VISIBLE;
    case "auto"_hash:
        return ScrollBarDisplayType::SBD_AUTO;
    case "hidden"_hash:
        return ScrollBarDisplayType::SDB_HIDDEN;
    default:
        return ScrollBarDisplayType::SBD_DEFAULT;
    }
}

ProgressTitleType ToolSet::parseProgressTitleType(const char * p)
{
    if (!p)
        return ProgressTitleType::PT_PERCENT;

    switch (hash_(p))
    {
    case "percent"_hash:
        return ProgressTitleType::PT_PERCENT;
    case "valueAndmax"_hash:
        return ProgressTitleType::PT_VALUE_MAX;
    case "value"_hash:
        return ProgressTitleType::PT_VALUE;
    case "max"_hash:
        return ProgressTitleType::PT_MAX;
    default:
        return ProgressTitleType::PT_PERCENT;
    }
}

NS_FGUI_END