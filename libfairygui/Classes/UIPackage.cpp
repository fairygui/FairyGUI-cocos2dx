#include "UIPackage.h"
#include "utils/ByteArray.h"
#include "utils/ToolSet.h"
#include "UIObjectFactory.h"
#include "tinyxml2/tinyxml2.h"

NS_FGUI_BEGIN

using namespace tinyxml2;

const string UIPackage::URL_PREFIX = "ui://";
int UIPackage::_constructing = 0;

const unsigned char* whiteTextureData = new unsigned char[116]{ 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF };

UIPackage::PackageMap UIPackage::_packageInstById;
UIPackage::PackageMap UIPackage::_packageInstByName;
UIPackage::PackageCollection UIPackage::_packageList;
ValueMap UIPackage::_stringsSource;
Texture2D* UIPackage::_emptyTexture;

UIPackage::UIPackage()
{
}

UIPackage::~UIPackage()
{
    CC_SAFE_RELEASE(_emptyTexture);

    for_each(_items.begin(), _items.end(), [](PackageItem* pi) {
        CC_SAFE_DELETE(pi->scale9Grid);
        CC_SAFE_DELETE(pi->componentData);
        if (pi->displayList)
        {
            for_each(pi->displayList->begin(), pi->displayList->end(), [](DisplayListItem* di) {
                CC_SAFE_DELETE(di);
            });
            CC_SAFE_DELETE(pi->displayList);
        }

        CC_SAFE_RELEASE(pi->texture);
        CC_SAFE_RELEASE(pi->spriteFrame);
        CC_SAFE_RELEASE(pi->animation);

        CC_SAFE_DELETE(pi);
    });
}

UIPackage * UIPackage::getById(const string& id)
{
    return _packageInstById[id];
}

UIPackage * UIPackage::getByName(const string& name)
{
    return _packageInstByName[name];
}

UIPackage * UIPackage::addPackage(const string& assetPath)
{
    PackageMap::iterator it = _packageInstById.find(assetPath);
    if (it != _packageInstById.end())
        return it->second;

    UIPackage* pkg = new UIPackage();
    pkg->create(assetPath);

    _packageInstById[pkg->getId()] = pkg;
    _packageInstByName[pkg->getName()] = pkg;
    _packageInstById[assetPath] = pkg;
    _packageList.push_back(pkg);

    return pkg;
}

void UIPackage::removePackage(const string& packageIdOrName)
{
    UIPackage* pkg = UIPackage::getByName(packageIdOrName);
    if (!pkg)
        pkg = getById(packageIdOrName);

    if (pkg)
        delete pkg;
    else
        CCLOG("FairyGUI: invalid package name or id: %s", packageIdOrName.c_str());
}

GObject* UIPackage::createObject(const string& pkgName, const string& resName)
{
    UIPackage* pkg = UIPackage::getByName(pkgName);
    if (pkg)
        return pkg->createObject(resName);
    else
        return nullptr;
}

GObject* UIPackage::createObjectFromURL(const string& url)
{
    PackageItem* pi = UIPackage::getItemByURL(url);
    if (pi)
        return pi->owner->createObject(pi);
    else
        return nullptr;
}

void * UIPackage::getItemAsset(const string& pkgName, const string& resName)
{
    UIPackage* pkg = UIPackage::getByName(pkgName);
    if (pkg)
        return pkg->getItemAsset(resName);
    else
        return nullptr;
}

void * UIPackage::getItemAssetByURL(const string& url)
{
    PackageItem* pi = UIPackage::getItemByURL(url);
    if (pi)
        return pi->owner->getItemAsset(pi);
    else
        return nullptr;
}

string UIPackage::getItemURL(const string& pkgName, const string& resName)
{
    UIPackage* pkg = UIPackage::getByName(pkgName);
    if (pkg)
    {
        PackageItem* pi = pkg->getItemByName(resName);
        if (pi)
            return URL_PREFIX + pkg->getId() + pi->id;
    }
    return string();
}

PackageItem * UIPackage::getItemByURL(const string& url)
{
    if (url.size() == 0)
        return nullptr;

    int pos1 = url.find('/');
    if (pos1 == -1)
        return nullptr;

    int pos2 = url.find('/', pos1 + 2);
    if (pos2 == -1)
    {
        if (url.size() > 13)
        {
            string pkgId = url.substr(5, 8);
            UIPackage* pkg = getById(pkgId);
            if (pkg != nullptr)
            {
                string srcId = url.substr(13);
                return pkg->getItem(srcId);
            }
        }
    }
    else
    {
        string pkgName = url.substr(pos1 + 2, pos2 - pos1 - 2);
        UIPackage* pkg = getByName(pkgName);
        if (pkg != nullptr)
        {
            string srcName = url.substr(pos2 + 1);
            return pkg->getItemByName(srcName);
        }
    }

    return nullptr;
}

string UIPackage::normalizeURL(const string& url)
{
    if (url.size() == 0)
        return url;

    int pos1 = url.find('/');
    if (pos1 == -1)
        return url;

    int pos2 = url.find('/', pos1 + 2);
    if (pos2 == -1)
        return url;
    else
    {
        string pkgName = url.substr(pos1 + 2, pos2 - pos1 - 2);
        string srcName = url.substr(pos2 + 1);
        return getItemURL(pkgName, srcName);
    }
}

PackageItem * UIPackage::getItem(const string & itemId)
{
    return _itemsById[itemId];
}

PackageItem * UIPackage::getItemByName(const string & itemName)
{
    return _itemsByName[itemName];
}

void * UIPackage::getItemAsset(const string& resName)
{
    PackageItem* pi = _itemsByName[resName];
    if (!pi)
        return nullptr;

    return  getItemAsset(pi);
}

void * UIPackage::getItemAsset(PackageItem * item)
{
    switch (item->type)
    {
    case PackageItemType::IMAGE:
        if (!item->decoded)
        {
            item->decoded = true;
            AtlasSprite* sprite = _sprites[item->id];
            if (sprite)
                item->spriteFrame = createSpriteTexture(sprite);
            else
                item->spriteFrame = SpriteFrame::createWithTexture(_emptyTexture, Rect());
        }
        return item->spriteFrame;

    case PackageItemType::ATLAS:
        if (!item->decoded)
        {
            item->decoded = true;
            loadAtlas(item);

        }
        return item->texture;

    case PackageItemType::SOUND:
        if (!item->decoded)
        {
            item->decoded = true;
            //loadSound(item);
        }
        return nullptr;// item.audioClip;

    case PackageItemType::FONT:
        if (!item->decoded)
        {
            item->decoded = true;
            //loadFont(item);
        }
        return nullptr;// item.bitmapFont;

    case PackageItemType::MOVIECLIP:
        if (!item->decoded)
        {
            item->decoded = true;
            loadMovieClip(item);
        }
        return item->animation;

    case PackageItemType::COMPONENT:
        if (!item->decoded)
        {
            item->decoded = true;
            loadComponent(item);
        }
        if (!_loadingPackage && !item->displayList)
        {
            loadComponentChildren(item);
            translateComponent(item);
        }
        return item->componentData;

    default:
        return nullptr;
    }
}

void UIPackage::create(const string& assetPath)
{
    Data data;

    if (CCFileUtils::getInstance()->getContents(assetPath + ".bytes", &data) != CCFileUtils::Status::OK)
    {
        CCLOGERROR("FairyGUI: cannot load package from '%s'", assetPath.c_str());
        return;
    }

    if (_emptyTexture == nullptr)
    {
        cocos2d::Image* emptyImage = new cocos2d::Image();
        emptyImage->initWithRawData(whiteTextureData, 16, 2, 2, 4, false);
        _emptyTexture = new Texture2D();
        _emptyTexture->initWithImage(emptyImage);
        _emptyTexture->retain();
        delete emptyImage;
    }
    else
        _emptyTexture->retain();

    _assetNamePrefix = assetPath + "@";
    decodeDesc(data);

    loadPackage();
}

void UIPackage::decodeDesc(cocos2d::Data& buffer)
{
    ByteArray* ba = ByteArray::createWithBuffer((char *)buffer.getBytes(), buffer.getSize());

    int pos = ba->getLength() - 22;
    ba->setPosition(pos + 10);
    int entryCount = ba->readShort();
    ba->setPosition(pos + 16);
    pos = ba->readInt();

    for (int i = 0; i < entryCount; i++)
    {
        ba->setPosition(pos + 28);
        int len = ba->readUnsignedShort();
        int len2 = ba->readUnsignedShort() + ba->readUnsignedShort();

        ba->setPosition(pos + 46);
        string entryName = ba->readString(len);

        if (entryName[entryName.size() - 1] != '/' && entryName[entryName.size() - 1] != '\\') //not directory
        {
            ba->setPosition(pos + 20);
            int size = ba->readInt();
            ba->setPosition(pos + 42);
            int offset = ba->readInt() + 30 + len;

            if (size > 0)
            {
                char* data = new char[size];
                memcpy(data, buffer.getBytes() + offset, size);

                _descPack[entryName] = string(data);
            }
        }

        pos += 46 + len + len2;
    }

    CC_SAFE_DELETE(ba);
}

void UIPackage::loadPackage()
{
    string str;

    if (CCFileUtils::getInstance()->getContents(_assetNamePrefix + "sprites.bytes", &str) != CCFileUtils::Status::OK)
    {
        CCLOGERROR("FairyGUI: cannot load package from '%s'", _assetNamePrefix.c_str());
        return;
    }

    _loadingPackage = true;

    vector<string> lines;
    vector<string> arr;

    ToolSet::splitString(str, '\n', lines);
    int cnt = lines.size();
    for (int i = 1; i < cnt; i++)
    {
        str = lines[i];
        if (str.size() == 0)
            continue;

        ToolSet::splitString(str, ' ', arr);
        AtlasSprite* sprite = new AtlasSprite();
        string itemId = arr[0];
        int binIndex = atoi(arr[1].c_str());
        if (binIndex >= 0)
            sprite->atlas = "atlas" + Value(binIndex).asString();
        else
        {
            int pos = itemId.find_first_of("_");
            if (pos == -1)
                sprite->atlas = "atlas_" + itemId;
            else
                sprite->atlas = "atlas_" + itemId.substr(0, pos);
        }
        sprite->rect.origin.x = atoi(arr[2].c_str());
        sprite->rect.origin.y = atoi(arr[3].c_str());
        sprite->rect.size.width = atoi(arr[4].c_str());
        sprite->rect.size.height = atoi(arr[5].c_str());
        sprite->rotated = arr[6] == "1";
        _sprites[itemId] = sprite;
    }

    str = _descPack["package.xml"].asString();

    tinyxml2::XMLDocument* xml = new tinyxml2::XMLDocument();
    xml->Parse(str.c_str());

    tinyxml2::XMLElement* root = xml->RootElement();

    _id = root->Attribute("id");
    _name = root->Attribute("name");

    XMLElement* rxml = root->FirstChildElement("resources");
    if (rxml == nullptr)
    {
        CCLOGERROR("FairyGUI: invalid package xml '%s'", _assetNamePrefix.c_str());
        return;
    }

    PackageItem* pi;
    XMLElement* cxml = rxml->FirstChildElement();
    Vec2 v2;
    Vec4 v4;
    const char *p;
    while (cxml)
    {
        pi = new PackageItem();
        pi->owner = this;
        pi->type = ToolSet::parsePackageItemType(cxml->Name());
        pi->id = cxml->Attribute("id");

        p = cxml->Attribute("name");
        if (p)
            pi->name = p;

        pi->exported = cxml->BoolAttribute("exported");

        p = cxml->Attribute("file");
        if (p)
            pi->file = p;

        p = cxml->Attribute("size");
        if (p)
        {
            ToolSet::splitString(p, ',', v2, true);
            pi->width = v2.x;
            pi->height = v2.y;
        }
        switch (pi->type)
        {
        case PackageItemType::IMAGE:
        {
            p = cxml->Attribute("scale");
            if (p)
            {
                if (strcmp(p, "9grid") == 0)
                {
                    p = cxml->Attribute("scale9grid");
                    if (p)
                    {
                        ToolSet::splitString(p, ',', v4, true);
                        pi->scale9Grid = new Rect();
                        pi->scale9Grid->origin.x = v4.x;
                        pi->scale9Grid->origin.y = v4.y;
                        pi->scale9Grid->size.width = v4.z;
                        pi->scale9Grid->size.height = v4.w;

                        pi->tileGridIndice = cxml->IntAttribute("gridTile");
                    }
                }
                else if (strcmp(p, "tile") == 0)
                    pi->scaleByTile = true;
            }
            break;
        }

        case PackageItemType::FONT:
        {
            //pi.bitmapFont = new BitmapFont(pi);
            //FontManager.RegisterFont(pi.bitmapFont, null);
            break;
        }

        case PackageItemType::COMPONENT:
        {
            UIObjectFactory::resolvePackageItemExtension(pi);
            break;
        }
        }
        _items.push_back(pi);
        _itemsById[pi->id] = pi;
        if (pi->name.size() > 0)
            _itemsByName[pi->name] = pi;

        cxml = cxml->NextSiblingElement();
    }

    delete xml;

    cnt = _items.size();
    for (int i = 0; i < cnt; i++)
        getItemAsset(_items[i]);

    _descPack.clear();

    for (SpriteMap::iterator iter = _sprites.begin(); iter != _sprites.end(); ++iter)
        delete iter->second;
    _sprites.clear();

    _loadingPackage = false;
}

cocos2d::SpriteFrame* UIPackage::createSpriteTexture(AtlasSprite * sprite)
{
    PackageItem* atlasItem = _itemsById[sprite->atlas];
    Texture2D* atlasTexture;
    if (atlasItem)
        atlasTexture = (Texture2D*)getItemAsset(atlasItem);
    else
    {
        atlasTexture = _emptyTexture;
        CCLOG("FairyGUI: %s not found in %s", sprite->atlas.c_str(), _name.c_str());
    }

    SpriteFrame* spriteFrame = new SpriteFrame();
    spriteFrame->initWithTexture(atlasTexture, sprite->rect, sprite->rotated, Vec2(0, 0), sprite->rect.size);
    spriteFrame->retain();

    return spriteFrame;
}

void UIPackage::loadAtlas(PackageItem * item)
{
    cocos2d::Image* image = new cocos2d::Image();
    string fileName = _assetNamePrefix + (item->file.size() > 0 ? item->file : item->id + ".png");
    if (!image->initWithImageFile(fileName))
    {
        item->texture = _emptyTexture;
        CCLOG("FairyGUI: texture '%s' not found in %s", fileName.c_str(), _name.c_str());
        return;
    }

    Texture2D* tex = new Texture2D();
    tex->initWithImage(image);
    item->texture = tex;
    tex->retain();

    delete image;
}

void UIPackage::loadMovieClip(PackageItem * item)
{
    string str = _descPack[item->id + ".xml"].asString();
    tinyxml2::XMLDocument* xml = new tinyxml2::XMLDocument();
    xml->Parse(str.c_str());

    tinyxml2::XMLElement* root = xml->RootElement();
    vector<string> arr;

    item->animation = Animation::create();
    item->animation->retain();

    float interval = root->FloatAttribute("interval") / 1000.0f;
    //bool swing = root->BoolAttribute("swing");
    //float repeatDelay = root->FloatAttribute("repeatDelay") / 1000.0f;

    int frameCount = root->IntAttribute("frameCount");
    Vector<AnimationFrame*> frames(frameCount);

    int i = 0;
    string spriteId;
    AtlasSprite* sprite;
    const char* p;

    tinyxml2::XMLElement* framesEle = root->FirstChildElement("frames");
    tinyxml2::XMLElement* frameEle = framesEle->FirstChildElement("frame");
    while (frameEle)
    {
        ToolSet::splitString(frameEle->Attribute("rect"), ',', arr);
        Rect rect = Rect(atoi(arr[0].c_str()), atoi(arr[1].c_str()), atoi(arr[2].c_str()), atoi(arr[3].c_str()));
        float addDelay = frameEle->IntAttribute("addDelay");

        p = frameEle->Attribute("sprite");
        if (p)
            spriteId = item->id + "_" + string(p);
        else if (rect.size.width != 0)
            spriteId = item->id + "_" + Value(i).asString();
        else
            spriteId.clear();

        if (spriteId.size() && (sprite = _sprites[spriteId]) != nullptr)
        {
            PackageItem* atlasItem = _itemsById[sprite->atlas];
            if (atlasItem)
            {
                if (item->texture == nullptr)
                    item->texture = (Texture2D*)getItemAsset(atlasItem);
                SpriteFrame* spriteFrame = createSpriteTexture(sprite);
                spriteFrame->setOffset(Vec2(rect.origin.x, -rect.origin.y));
                AnimationFrame* frame = AnimationFrame::create(spriteFrame, addDelay / interval + 1, ValueMapNull);
                frames.pushBack(frame);
            }
        }
        i++;
        frameEle = frameEle->NextSiblingElement("frame");
    }

    delete xml;
    item->animation->initWithAnimationFrames(frames, interval, 1);
}

void UIPackage::loadComponent(PackageItem * item)
{
    string str = _descPack[item->id + ".xml"].asString();
    tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
    doc->Parse(str.c_str());
    item->componentData = doc;
}

void UIPackage::loadComponentChildren(PackageItem * item)
{
    tinyxml2::XMLElement* listNode = item->componentData->RootElement()->FirstChildElement("displayList");
    item->displayList = new vector<DisplayListItem*>();
    const char *p;
    if (listNode)
    {
        XMLElement* cxml = listNode->FirstChildElement();
        DisplayListItem* di;
        while (cxml)
        {
            p = cxml->Attribute("src");
            if (p)
            {
                string src = p;
                string pkgId = (p = cxml->Attribute("pkg")) ? p : "";
                UIPackage* pkg;
                if (pkgId.size() > 0 && pkgId.compare(item->owner->getId()) == 0)
                    pkg = UIPackage::getById(pkgId);
                else
                    pkg = item->owner;

                PackageItem* pi = pkg ? pkg->getItem(src) : nullptr;
                if (pi)
                    di = new DisplayListItem(pi, "");
                else
                    di = new DisplayListItem(nullptr, cxml->Name());
            }
            else
            {
                if (cxml->Name() == "text" && cxml->BoolAttribute("input"))
                    di = new DisplayListItem(nullptr, "inputtext");
                else
                    di = new DisplayListItem(nullptr, cxml->Name());
            }

            di->desc = cxml;
            item->displayList->push_back(di);

            cxml = cxml->NextSiblingElement();
        }
    }
}

void UIPackage::translateComponent(PackageItem * item)
{
}

GObject * UIPackage::createObject(const string & resName)
{
    PackageItem* pi = _itemsByName[resName];
    CCASSERT(pi, StringUtils::format("FairyGUI: resource not found - %s in  %s",
        resName.c_str(), _name.c_str()).c_str());

    return createObject(pi);
}

GObject * UIPackage::createObject(PackageItem * item)
{
    getItemAsset(item);

    GObject* g = UIObjectFactory::newObject(item);
    if (g == nullptr)
        return nullptr;

    _constructing++;
    g->packageItem = item;
    g->constructFromResource();
    _constructing--;
    return g;
}

NS_FGUI_END
