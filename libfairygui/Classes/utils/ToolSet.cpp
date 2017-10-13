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

void ToolSet::splitString(const string &s, char delim, cocos2d::Vec2& value)
{
    splitString(s, delim, helperArray);
    value.x = atof(helperArray[0].c_str());
    value.y = atof(helperArray[1].c_str());
}

void ToolSet::splitString(const string &s, char delim, cocos2d::Vec4& value)
{
    splitString(s, delim, helperArray);
    value.x = atof(helperArray[0].c_str());
    value.y = atof(helperArray[1].c_str());
    value.z = atof(helperArray[2].c_str());
    value.w = atof(helperArray[3].c_str());
}

void ToolSet::splitString(const std::string & s, char delim, std::string & str1, std::string str2)
{
}

string ToolSet::getAttribute(const tinyxml2::XMLElement* ele, const char* name)
{
    const tinyxml2::XMLAttribute*  a = ele->FindAttribute(name);
    if (!a)
        return "";

    return a->Value();
}

bool ToolSet::getArrayAttribute(const tinyxml2::XMLElement * ele, const char * name, cocos2d::Vec2& value, bool intType)
{
    const tinyxml2::XMLAttribute*  a = ele->FindAttribute(name);
    if (!a)
        return false;

    splitString(a->Value(), ',', helperArray);
    if (intType)
    {
        value.x = atoi(helperArray[0].c_str());
        value.y = atoi(helperArray[1].c_str());
    }
    else
    {
        value.x = atof(helperArray[0].c_str());
        value.y = atof(helperArray[1].c_str());
    }

    return true;
}

bool ToolSet::getArrayAttribute(const tinyxml2::XMLElement * ele, const char * name, cocos2d::Vec4& value, bool intType)
{
    const tinyxml2::XMLAttribute*  a = ele->FindAttribute(name);
    if (!a)
        return false;

    splitString(a->Value(), ',', helperArray);
    if (intType)
    {
        value.x = atoi(helperArray[0].c_str());
        value.y = atoi(helperArray[1].c_str());
        value.z = atoi(helperArray[2].c_str());
        value.w = atoi(helperArray[3].c_str());
    }
    else
    {
        value.x = atof(helperArray[0].c_str());
        value.y = atof(helperArray[1].c_str());
        value.z = atof(helperArray[2].c_str());
        value.w = atof(helperArray[3].c_str());
    }

    return true;
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
        return PackageItemType::Misc;

    switch (hash_(p))
    {
    case "image"_hash:
        return PackageItemType::Image;
    case "movieclip"_hash:
        return PackageItemType::MovieClip;
    case "component"_hash:
        return PackageItemType::Component;
    case "atlas"_hash:
        return PackageItemType::Atlas;
    case "sound"_hash:
        return PackageItemType::Sound;
    case "font"_hash:
        return PackageItemType::Font;
    case "misc"_hash:
        return PackageItemType::Misc;
    default:
        return PackageItemType::Misc;
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

GLoader::FillType ToolSet::parseFillType(const char * p)
{
    if (!p)
        return GLoader::FillType::None;

    switch (hash_(p))
    {
    case "none"_hash:
        return GLoader::FillType::None;
    case "scale"_hash:
        return GLoader::FillType::Scale;
    case "scaleMatchHeight"_hash:
        return GLoader::FillType::ScaleMatchHeight;
    case "scaleMatchWidth"_hash:
        return GLoader::FillType::ScaleMatchWidth;
    case "scaleFree"_hash:
        return GLoader::FillType::ScaleFree;
    default:
        return GLoader::FillType::None;
    }
}

GButton::ButtonMode ToolSet::parseButtonMode(const char * p)
{
    if (!p)
        return GButton::ButtonMode::Common;

    switch (hash_(p))
    {
    case "Check"_hash:
        return GButton::ButtonMode::Check;
    case "Radio"_hash:
        return GButton::ButtonMode::Radio;
    default:
        return GButton::ButtonMode::Common;
    }
}

GComponent::OverflowType ToolSet::parseOverflowType(const char * p)
{
    if (!p)
        return GComponent::OverflowType::OverflowVisible;

    switch (hash_(p))
    {
    case "visible"_hash:
        return GComponent::OverflowType::OverflowVisible;
    case "hidden"_hash:
        return GComponent::OverflowType::OverflowHidden;
    case "scroll"_hash:
        return GComponent::OverflowType::OverflowScroll;
    default:
        return GComponent::OverflowType::OverflowVisible;
    }
}

GComponent::ScrollType ToolSet::parseScrollType(const char * p)
{
    if (!p)
        return GComponent::ScrollType::ScrollHorizontal;

    switch (hash_(p))
    {
    case "horizontal"_hash:
        return GComponent::ScrollType::ScrollHorizontal;
    case "vertical"_hash:
        return GComponent::ScrollType::ScrollVertical;
    case "both"_hash:
        return GComponent::ScrollType::ScrollBoth;
    default:
        return GComponent::ScrollType::ScrollHorizontal;
    }
}

GComponent::ScrollBarDisplayType ToolSet::parseScrollBarDisplayType(const char * p)
{
    if (!p)
        return GComponent::ScrollBarDisplayType::ScrollBarDisplayDefault;

    switch (hash_(p))
    {
    case "default"_hash:
        return GComponent::ScrollBarDisplayType::ScrollBarDisplayDefault;
    case "visible"_hash:
        return GComponent::ScrollBarDisplayType::ScrollBarDisplayVisible;
    case "auto"_hash:
        return GComponent::ScrollBarDisplayType::ScrollBarDisplayAuto;
    case "hidden"_hash:
        return GComponent::ScrollBarDisplayType::ScrollBarDisplayHidden;
    default:
        return GComponent::ScrollBarDisplayType::ScrollBarDisplayDefault;
    }
}

NS_FGUI_END