#ifndef __UIPACKAGE_H__
#define __UIPACKAGE_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "PackageItem.h"
#include "GObject.h"

NS_FGUI_BEGIN

USING_NS_CC;
using namespace std;

class UIPackage
{
public:
    UIPackage();
    virtual ~UIPackage();

    class AtlasSprite
    {
    public:
        string atlas;
        Rect rect;
        bool rotated;
    };

    typedef std::unordered_map<string, UIPackage*> PackageMap;
    typedef std::unordered_map<string, PackageItem*> PackageItemMap;
    typedef std::vector<UIPackage*> PackageCollection;
    typedef std::vector<PackageItem*> PackageItemCollection;
    typedef std::unordered_map<string, AtlasSprite*> SpriteMap;

public:
    static int _constructing;
    static const string URL_PREFIX;

    static UIPackage* getById(const string& id);
    static UIPackage* getByName(const string& name);
    static UIPackage* addPackage(const string& descFilePath);
    static void removePackage(const string& packageIdOrName);
    static GObject* createObject(const string& pkgName, const string& resName);
    static GObject* createObjectFromURL(const string& url);
    static void* getItemAsset(const string& pkgName, const string& resName);
    static void* getItemAssetByURL(const string& url);
    static string getItemURL(const string& pkgName, const string& resName);
    static PackageItem* getItemByURL(const string& url);
    static string normalizeURL(const string& url);

    string getId() { return _id; }
    string getName() { return _name; }

    PackageItem* getItem(const string& itemId);
    PackageItem* getItemByName(const string& itemName);
    void* getItemAsset(const string& resName);
    void* getItemAsset(PackageItem* item);

private:
    void create(const string& assetPath);
    void decodeDesc(cocos2d::Data& data);
    void loadPackage();
    cocos2d::SpriteFrame* createSpriteTexture(AtlasSprite* sprite);
    void loadAtlas(PackageItem* item);
    void loadMovieClip(PackageItem* item);
    void loadComponent(PackageItem* item);
    void loadComponentChildren(PackageItem* item);
    void translateComponent(PackageItem* item);
    GObject* createObject(const string& resName);
    GObject* createObject(PackageItem* item);

private:
    string _id;
    string _name;
    string _assetPath;

    PackageItemCollection _items;
    PackageItemMap _itemsById;
    PackageItemMap _itemsByName;
    ValueMap _descPack;
    //ValueMap _hitTestDatas;
    string _assetNamePrefix;
    string _customId;
    bool _fromBundle;
    bool _loadingPackage;

    SpriteMap _sprites;

    static PackageMap _packageInstById;
    static PackageMap _packageInstByName;
    static PackageCollection _packageList;
    static ValueMap _stringsSource;
    static Texture2D* _emptyTexture;
};

NS_FGUI_END

#endif // __UIPACKAGE_H__
