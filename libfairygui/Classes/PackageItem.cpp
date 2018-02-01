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
    bitmapFont(nullptr)
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
    if (bitmapFont) //bitmapfont will be released by fontatlas
        bitmapFont->releaseAtlas();
    CC_SAFE_RELEASE(animation);
    CC_SAFE_RELEASE(texture);
    CC_SAFE_RELEASE(spriteFrame);
}

void PackageItem::load()
{
    owner->loadItem(this);
}

DisplayListItem::DisplayListItem(PackageItem* pi, const std::string& type)
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
