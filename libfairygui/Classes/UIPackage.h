#ifndef __UIPACKAGE_H__
#define __UIPACKAGE_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "PackageItem.h"
#include "GObject.h"

NS_FGUI_BEGIN

USING_NS_CC;

class UIPackage
{
public:
    UIPackage();
    ~UIPackage();

    class AtlasSprite
    {
    public:
        std::string atlas;
        Rect rect;
        bool rotated;
    };

    typedef std::unordered_map<std::string, UIPackage*> PackageMap;
    typedef std::unordered_map<std::string, PackageItem*> PackageItemMap;
    typedef std::vector<UIPackage*> PackageCollection;
    typedef std::vector<PackageItem*> PackageItemCollection;
    typedef std::unordered_map<std::string, AtlasSprite*> SpriteMap;

    static UIPackage* getById(const std::string& id);
    static UIPackage* getByName(const std::string& name);
    static UIPackage* addPackage(const std::string& descFilePath);
    static void removePackage(const std::string& packageIdOrName);
    static GObject* createObject(const std::string& pkgName, const std::string& resName);
    static GObject* createObjectFromURL(const std::string& url);
    static void* getItemAsset(const std::string& pkgName, const std::string& resName);
    static void* getItemAssetByURL(const std::string& url);
    static std::string getItemURL(const std::string& pkgName, const std::string& resName);
    static PackageItem* getItemByURL(const std::string& url);
    static std::string normalizeURL(const std::string& url);

    static Texture2D* getEmptyTexture() { return _emptyTexture; }

    const std::string& getId() const { return _id; }
    const std::string& getName() const { return _name; }

    PackageItem* getItem(const std::string& itemId);
    PackageItem* getItemByName(const std::string& itemName);
    void* getItemAsset(const std::string& resName);
    void* getItemAsset(PackageItem* item);

    static int _constructing;
    static const std::string URL_PREFIX;

private:
    void create(const std::string& assetPath);
    void decodeDesc(cocos2d::Data& data);
    void loadPackage();
    cocos2d::SpriteFrame* createSpriteTexture(AtlasSprite* sprite);
    void loadAtlas(PackageItem* item);
    void loadMovieClip(PackageItem* item);
    void loadComponent(PackageItem* item);
    void loadComponentChildren(PackageItem* item);
    void translateComponent(PackageItem* item);
    GObject* createObject(const std::string& resName);
    GObject* createObject(PackageItem* item);

private:
    std::string _id;
    std::string _name;
    std::string _assetPath;

    PackageItemCollection _items;
    PackageItemMap _itemsById;
    PackageItemMap _itemsByName;
    ValueMap _descPack;
    //ValueMap _hitTestDatas;
    std::string _assetNamePrefix;
    std::string _customId;
    bool _loadingPackage;

    SpriteMap _sprites;

    static PackageMap _packageInstById;
    static PackageMap _packageInstByName;
    static PackageCollection _packageList;
    static ValueMap _stringsSource;
    static Texture2D* _emptyTexture;
};

NS_FGUI_END

#endif
