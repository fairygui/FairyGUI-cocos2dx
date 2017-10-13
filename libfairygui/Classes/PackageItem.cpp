#include "PackageItem.h"
#include "UIPackage.h"

NS_FGUI_BEGIN

PackageItem::PackageItem() :
    owner(nullptr),
    width(0),
    height(0),
    decoded(false),
    exported(false),
    spriteFrame(nullptr),
    scale9Grid(nullptr),
    scaleByTile(false),
    tileGridIndice(0),
    texture(nullptr),
    animation(nullptr),
    componentData(nullptr),
    displayList(nullptr),
    extensionCreator(nullptr)
{
}

PackageItem::~PackageItem()
{
    if (scale9Grid)
        CC_SAFE_DELETE(scale9Grid);
    if (displayList)
        CC_SAFE_DELETE(displayList);
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