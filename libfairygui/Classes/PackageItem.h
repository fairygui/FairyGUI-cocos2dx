#ifndef __PACKAGEITEM_H__
#define __PACKAGEITEM_H__

#include "cocos2d.h"
#include "tinyxml2/tinyxml2.h"
#include "FairyGUIMacros.h"

NS_FGUI_BEGIN

class UIPackage;
class DisplayListItem;
class UIObjectFactory;
class GComponent;

class PackageItem
{
public:
    PackageItem();
    virtual ~PackageItem();

    void* load();

public:
    UIPackage* owner;

    PackageItemType type;
    std::string id;
    std::string name;
    int width;
    int height;
    std::string file;
    bool decoded;
    bool exported;

    //image
    cocos2d::Rect* scale9Grid;
    bool scaleByTile;
    int tileGridIndice;
    cocos2d::Texture2D* texture;
    cocos2d::SpriteFrame* spriteFrame;

    //movieclip
    cocos2d::Animation* animation;

    //component
    tinyxml2::XMLDocument* componentData;
    std::vector<DisplayListItem*>* displayList;
    std::function<GComponent*()> extensionCreator;

    //font
    //BitmapFont bitmapFont;

    //sound
    //AudioClip audioClip;
};

class DisplayListItem
{
public:
    PackageItem* packageItem;
    std::string type;
    tinyxml2::XMLElement* desc;
    int listItemCount;

    DisplayListItem(PackageItem* pi, std::string type);
    virtual ~DisplayListItem();
};

NS_FGUI_END

#endif // __PACKAGEITEM_H__
