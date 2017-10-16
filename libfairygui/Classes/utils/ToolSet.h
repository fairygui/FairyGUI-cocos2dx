#ifndef __TOOLSET_H__
#define __TOOLSET_H__

#include "cocos2d.h"
#include "tinyxml2/tinyxml2.h"
#include "FairyGUI.h"

NS_FGUI_BEGIN

class ToolSet
{
public:
    static std::vector<std::string> &splitString(const std::string &s, char delim, std::vector<std::string> &elems);
    static void splitString(const std::string &s, char delim, cocos2d::Vec2& value);
    static void splitString(const std::string &s, char delim, cocos2d::Vec4& value);
    static void splitString(const std::string &s, char delim, std::string& str1, std::string str2);

    static std::string getAttribute(const tinyxml2::XMLElement* ele, const char* name);
    static bool getArrayAttribute(const tinyxml2::XMLElement* ele, const char* name, cocos2d::Vec2& value, bool intType = false);
    static bool getArrayAttribute(const tinyxml2::XMLElement* ele, const char* name, cocos2d::Vec4& value, bool intType = false);
    static ssize_t findInStringArray(const std::vector<std::string>& arr, const std::string str);

    static cocos2d::Color4B convertFromHtmlColor(const char* str);

    static PackageItemType parsePackageItemType(const char * p);
    static cocos2d::TextHAlignment parseAlign(const char *p);
    static cocos2d::TextVAlignment parseVerticalAlign(const char *p);
    static int parseGearIndex(const char* p);
    static GLoader::FillType parseFillType(const char *p);
    static GButton::ButtonMode parseButtonMode(const char*p);
    static GComponent::OverflowType parseOverflowType(const char*p);
    static GComponent::ScrollType parseScrollType(const char*p);
    static GComponent::ScrollBarDisplayType parseScrollBarDisplayType(const char*p);
    static ProgressTitleType parseProgressTitleType(const char*p);
};


NS_FGUI_END

#endif // __TOOLSET_H__
