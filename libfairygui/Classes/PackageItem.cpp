#include "PackageItem.h"
#include "UIPackage.h"
#include "display/BitmapFont.h"

NS_FGUI_BEGIN

PackageItem::PackageItem() :
    owner(nullptr),
    width(0),
    height(0),
    decoded(false),
    exported(false),
    texture(nullptr),
    spriteFrame(nullptr),
    scale9Grid(nullptr),
    scaleByTile(false),
    tileGridIndice(0),
    animation(nullptr),
    repeatDelay(0),
    componentData(nullptr),
    displayList(nullptr),
    extensionCreator(nullptr),
    bitmapFont(nullptr),
    addedToCache(false)
{
}

PackageItem::~PackageItem()
{
    CC_SAFE_DELETE(scale9Grid);
    if (displayList)
    {
        for (auto &it : *displayList)
            CC_SAFE_DELETE(it);
        delete displayList;
    }
    CC_SAFE_DELETE(componentData);
    if (bitmapFont) //bitmapfont的引用被fontatlas保存，所以这里不能直接释放bitmapFont
        bitmapFont->releaseAtlas();
    CC_SAFE_RELEASE(animation);
    CC_SAFE_RELEASE(texture);
    if (addedToCache)
        SpriteFrameCache::getInstance()->removeSpriteFrameByName("ui://" + owner->getId() + id);
    CC_SAFE_RELEASE(spriteFrame);
}

void* PackageItem::load()
{
    return owner->getItemAsset(this);
}

DisplayListItem::DisplayListItem(PackageItem* pi, std::string type)
    :packageItem(pi),
    type(type),
    desc(nullptr),
    listItemCount(0)
{
}

DisplayListItem::~DisplayListItem()
{
}

NS_FGUI_END