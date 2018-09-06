#include "PackageItem.h"
#include "UIPackage.h"
#include "display/BitmapFont.h"
#include "utils/ByteBuffer.h"

NS_FGUI_BEGIN

PackageItem::PackageItem() :
    owner(nullptr),
    objectType(ObjectType::COMPONENT),
    width(0),
    height(0),
    rawData(nullptr),
    texture(nullptr),
    spriteFrame(nullptr),
    scale9Grid(nullptr),
    scaleByTile(false),
    tileGridIndice(0),
    animation(nullptr),
    repeatDelay(0),
    swing(false),
    extensionCreator(nullptr),
    translated(false),
    bitmapFont(nullptr)
{
}

PackageItem::~PackageItem()
{
    CC_SAFE_DELETE(scale9Grid);

    CC_SAFE_DELETE(rawData);
    if (bitmapFont) //bitmapfont will be released by fontatlas
        bitmapFont->releaseAtlas();
    bitmapFont = nullptr;
    CC_SAFE_RELEASE(animation);
    CC_SAFE_RELEASE(texture);
    CC_SAFE_RELEASE(spriteFrame);
}

void PackageItem::load()
{
    owner->getItemAsset(this);
}

NS_FGUI_END
