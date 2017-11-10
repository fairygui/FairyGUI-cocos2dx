#ifndef __PACKAGEITEM_H__
#define __PACKAGEITEM_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"

NS_FGUI_BEGIN

class UIPackage;
class DisplayListItem;
class UIObjectFactory;
class GComponent;
class BitmapFont;

class PackageItem
{
public:
    PackageItem();
    virtual ~PackageItem();

    void load();

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

    //atlas
    cocos2d::Texture2D* texture;

    //image
    cocos2d::Rect* scale9Grid;
    bool scaleByTile;
    int tileGridIndice;
    cocos2d::SpriteFrame* spriteFrame;
    bool addedToCache;

    //movieclip
    cocos2d::Animation* animation;
    float delayPerUnit;
    float repeatDelay;

    //component
    TXMLDocument* componentData;
    std::vector<DisplayListItem*>* displayList;
    std::function<GComponent*()> extensionCreator;

    //font
    BitmapFont* bitmapFont;
};

class DisplayListItem
{
public:
    PackageItem* packageItem;
    std::string type;
    TXMLElement* desc;
    int listItemCount;

    DisplayListItem(PackageItem* pi, std::string type);
    virtual ~DisplayListItem();
};

NS_FGUI_END

#endif
