#ifndef __PACKAGEITEM_H__
#define __PACKAGEITEM_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"

NS_FGUI_BEGIN

class UIPackage;
class UIObjectFactory;
class GComponent;
class BitmapFont;
class PixelHitTestData;
class ByteBuffer;

class PackageItem : public cocos2d::Ref
{
public:
    PackageItem();
    virtual ~PackageItem();

    void load();

public:
    UIPackage* owner;

    PackageItemType type;
    ObjectType objectType;
    std::string id;
    std::string name;
    int width;
    int height;
    std::string file;
    ByteBuffer* rawData;

    //atlas
    cocos2d::Texture2D* texture;

    //image
    cocos2d::Rect* scale9Grid;
    bool scaleByTile;
    int tileGridIndice;
    cocos2d::SpriteFrame* spriteFrame;
    PixelHitTestData* pixelHitTestData;

    //movieclip
    cocos2d::Animation* animation;
    float delayPerUnit;
    float repeatDelay;
    bool swing;

    //component
    std::function<GComponent*()> extensionCreator;
    bool translated;

    //font
    BitmapFont* bitmapFont;
};

NS_FGUI_END

#endif
