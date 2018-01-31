#include "UIPackage.h"
#include "utils/ByteArray.h"
#include "utils/ToolSet.h"
#include "UIObjectFactory.h"
#include "display/BitmapFont.h"
#include "event/HitTest.h"

NS_FGUI_BEGIN
USING_NS_CC;

using namespace std;

const string UIPackage::URL_PREFIX = "ui://";
int UIPackage::_constructing = 0;

const unsigned char* emptyTextureData = new unsigned char[16]{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

std::unordered_map<std::string, UIPackage*> UIPackage::_packageInstById;
std::unordered_map<std::string, UIPackage*> UIPackage::_packageInstByName;
std::vector<UIPackage*> UIPackage::_packageList;
ValueMap UIPackage::_stringsSource;
Texture2D* UIPackage::_emptyTexture;

struct AtlasSprite
{
    std::string atlas;
    Rect rect;
    bool rotated;
};

UIPackage::UIPackage() :
    _loadingPackage(false)
{
}

UIPackage::~UIPackage()
{
    for (auto &it : _items)
        delete it;
    for (auto &it : _hitTestDatas)
        delete it.second;
    for (auto &it : _sprites)
        delete it.second;
    for (auto &it : _descPack)
        delete it.second;
}

UIPackage * UIPackage::getById(const string& id)
{
    auto it = _packageInstById.find(id);
    if (it != _packageInstById.end())
        return it->second;
    else
        return nullptr;
}

UIPackage * UIPackage::getByName(const string& name)
{
    auto it = _packageInstByName.find(name);
    if (it != _packageInstByName.end())
        return it->second;
    else
        return nullptr;
}

UIPackage * UIPackage::addPackage(const string& assetPath)
{
    auto it = _packageInstById.find(assetPath);
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
    {
        auto it = std::find(_packageList.cbegin(), _packageList.cend(), pkg);
        if (it != _packageList.cend())
            _packageList.erase(it);

        _packageInstById.erase(pkg->getId());
        _packageInstById.erase(pkg->_assetPath);
        _packageInstByName.erase(pkg->getName());

        delete pkg;
    }
    else
        CCLOG("FairyGUI: invalid package name or id: %s", packageIdOrName.c_str());
}

void UIPackage::removeAllPackages()
{
    for (auto &it : _packageList)
        delete it;

    _packageInstById.clear();
    _packageInstByName.clear();
    _packageList.clear();
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

    ssize_t pos1 = url.find('/');
    if (pos1 == -1)
        return nullptr;

    ssize_t pos2 = url.find('/', pos1 + 2);
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

    ssize_t pos1 = url.find('/');
    if (pos1 == -1)
        return url;

    ssize_t pos2 = url.find('/', pos1 + 2);
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
    auto it = _itemsById.find(itemId);
    if (it != _itemsById.end())
        return it->second;
    else
        return nullptr;
}

PackageItem * UIPackage::getItemByName(const string & itemName)
{
    auto it = _itemsByName.find(itemName);
    if (it != _itemsByName.end())
        return it->second;
    else
        return nullptr;
}

void UIPackage::loadItem(const string& resName)
{
    PackageItem* pi = getItemByName(resName);
    if (pi)
        return loadItem(pi);
}

void UIPackage::loadItem(PackageItem * item)
{
    switch (item->type)
    {
    case PackageItemType::IMAGE:
        if (!item->decoded)
        {
            item->decoded = true;
            auto it = _sprites.find(item->id);
            if (it != _sprites.end())
            {
                AtlasSprite * sprite = it->second;
                item->spriteFrame = createSpriteTexture(sprite);
            }
            else
                item->spriteFrame = SpriteFrame::createWithTexture(_emptyTexture, Rect());
            if (item->scaleByTile)
            {
                Texture2D::TexParams tp = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT };
                item->spriteFrame->getTexture()->setTexParameters(tp);
            }
        }
        break;

    case PackageItemType::ATLAS:
        if (!item->decoded)
        {
            item->decoded = true;
            loadAtlas(item);
        }
        break;

    case PackageItemType::SOUND:
        if (!item->decoded)
        {
            item->decoded = true;
            item->file = _assetNamePrefix + item->file;
        }
        break;

    case PackageItemType::FONT:
        if (!item->decoded)
        {
            item->decoded = true;
            loadFont(item);
        }
        break;

    case PackageItemType::MOVIECLIP:
        if (!item->decoded)
        {
            item->decoded = true;
            loadMovieClip(item);
        }
        break;

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
        break;
    default:
        break;
    }
}

PixelHitTestData * UIPackage::getPixelHitTestData(const std::string & itemId)
{
    auto it = _hitTestDatas.find(itemId);
    if (it != _hitTestDatas.end())
        return it->second;
    else
        return nullptr;
}

void UIPackage::create(const string& assetPath)
{
    Data data;

    if (FileUtils::getInstance()->getContents(assetPath + ".bytes", &data) != FileUtils::Status::OK)
    {
        CCLOGERROR("FairyGUI: cannot load package from '%s'", assetPath.c_str());
        return;
    }

    if (_emptyTexture == nullptr)
    {
        Image* emptyImage = new Image();
        emptyImage->initWithRawData(emptyTextureData, 16, 2, 2, 4, false);
        _emptyTexture = new Texture2D();
        _emptyTexture->initWithImage(emptyImage);
        _emptyTexture->retain();
        _emptyTexture->autorelease();
        delete emptyImage;
    }

    _assetPath = assetPath;
    _assetNamePrefix = assetPath + "@";
    decodeDesc(data);

    loadPackage();
}

void UIPackage::decodeDesc(Data& buffer)
{
    ByteArray* ba = ByteArray::createWithBuffer((char *)buffer.getBytes(), buffer.getSize());

    size_t pos = ba->getLength() - 22;
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
                Data* data = new Data();
                data->copy(buffer.getBytes() + offset, size);
                _descPack[entryName] = data;
            }
        }

        pos += 46 + len + len2;
    }

    CC_SAFE_DELETE(ba);
}

void UIPackage::loadPackage()
{
    string str;

    if (FileUtils::getInstance()->getContents(_assetNamePrefix + "sprites.bytes", &str) != FileUtils::Status::OK)
    {
        CCLOGERROR("FairyGUI: cannot load package from '%s'", _assetNamePrefix.c_str());
        return;
    }

    _loadingPackage = true;

    vector<string> lines;
    vector<string> arr;

    ToolSet::splitString(str, '\n', lines);
    size_t cnt = lines.size();
    for (size_t i = 1; i < cnt; i++)
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
            size_t pos = itemId.find_first_of("_");
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

    string hitTestDataFilePath = _assetNamePrefix + "hittest.bytes";
    bool tmp = FileUtils::getInstance()->isPopupNotify();
    FileUtils::getInstance()->setPopupNotify(false);
    bool hasHitTestData = FileUtils::getInstance()->isFileExist(hitTestDataFilePath);
    FileUtils::getInstance()->setPopupNotify(tmp);

    if (hasHitTestData)
    {
        Data data = FileUtils::getInstance()->getDataFromFile(hitTestDataFilePath);
        ByteArray* ba = ByteArray::createWithBuffer((char*)data.getBytes(), data.getSize(), false);
        ba->setEndian(ByteArray::ENDIAN_BIG);
        while (ba->getBytesAvailable())
        {
            PixelHitTestData* pht = new PixelHitTestData();
            _hitTestDatas[ba->readString()] = pht;
            pht->load(*ba);
        }
        CC_SAFE_DELETE(ba);
    }

    Data* xmlData = _descPack["package.xml"];

    TXMLDocument* xml = new TXMLDocument();
    xml->Parse((const char*)xmlData->getBytes(), xmlData->getSize());

    TXMLElement* root = xml->RootElement();

    _id = root->Attribute("id");
    _name = root->Attribute("name");

    TXMLElement* rxml = root->FirstChildElement("resources");
    if (rxml == nullptr)
    {
        CCLOGERROR("FairyGUI: invalid package xml '%s'", _assetNamePrefix.c_str());
        return;
    }

    PackageItem* pi;
    TXMLElement* cxml = rxml->FirstChildElement();
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

        case PackageItemType::COMPONENT:
        {
            UIObjectFactory::resolvePackageItemExtension(pi);
            break;
        }

        default:
            break;
        }
        _items.push_back(pi);
        _itemsById[pi->id] = pi;
        if (!pi->name.empty())
            _itemsByName[pi->name] = pi;

        cxml = cxml->NextSiblingElement();
    }

    delete xml;

    for (auto &iter : _items)
        loadItem(iter);

    for (auto &iter : _descPack)
        delete iter.second;
    _descPack.clear();

    for (auto &iter : _sprites)
        delete iter.second;
    _sprites.clear();

    _loadingPackage = false;
}

//note: SpriteFrame.ref=1£¬not autorelease.
SpriteFrame* UIPackage::createSpriteTexture(AtlasSprite * sprite)
{
    PackageItem* atlasItem = getItem(sprite->atlas);
    Texture2D* atlasTexture;
    if (atlasItem)
    {
        loadItem(atlasItem);
        atlasTexture = atlasItem->texture;
    }
    else
    {
        atlasTexture = _emptyTexture;
        CCLOG("FairyGUI: %s not found in %s", sprite->atlas.c_str(), _name.c_str());
    }

    //not using createWithTexture for saving a autorelease call.
    SpriteFrame* spriteFrame = new SpriteFrame();
    spriteFrame->initWithTexture(atlasTexture, sprite->rect, sprite->rotated, Vec2(0, 0), sprite->rect.size);

    return spriteFrame;
}

void UIPackage::loadAtlas(PackageItem * item)
{
    Image* image = new Image();
    string filePath = _assetNamePrefix + (!item->file.empty() ? item->file : item->id + ".png");
    Image::setPNGPremultipliedAlphaEnabled(false);
    if (!image->initWithImageFile(filePath))
    {
        item->texture = _emptyTexture;
        delete image;
        Image::setPNGPremultipliedAlphaEnabled(true);
        CCLOGWARN("FairyGUI: texture '%s' not found in %s", filePath.c_str(), _name.c_str());
        return;
    }
    Image::setPNGPremultipliedAlphaEnabled(true);

    Texture2D* tex = new Texture2D();
    tex->initWithImage(image);
    item->texture = tex;
    tex->retain();
    tex->autorelease();
    delete image;

    string ext = FileUtils::getInstance()->getFileExtension(filePath);
    size_t pos = filePath.find_last_of('.');
    if (pos != -1)
        filePath = filePath.substr(0, pos) + "!a" + ext;
    else
        filePath = filePath + "!a" + ext;

    bool tmp = FileUtils::getInstance()->isPopupNotify();
    FileUtils::getInstance()->setPopupNotify(false);
    bool hasAlphaTexture = FileUtils::getInstance()->isFileExist(filePath);
    FileUtils::getInstance()->setPopupNotify(tmp);
    if (hasAlphaTexture)
    {
        image = new Image();
        if (!image->initWithImageFile(filePath))
        {
            delete image;
            return;
        }

        tex = new Texture2D();
        tex->initWithImage(image);
        item->texture->setAlphaTexture(tex);
        tex->autorelease();
        delete image;
    }
}

void UIPackage::loadMovieClip(PackageItem * item)
{
    Data* xmlData = _descPack[item->id + ".xml"];
    TXMLDocument* xml = new TXMLDocument();
    xml->Parse((const char*)xmlData->getBytes(), xmlData->getSize());

    TXMLElement* root = xml->RootElement();
    vector<string> arr;

    item->animation = Animation::create();
    item->animation->retain();

    float interval = root->FloatAttribute("interval") / 1000.0f;
    item->repeatDelay = root->FloatAttribute("repeatDelay") / 1000.0f;
    bool swing = root->BoolAttribute("swing");

    int frameCount = root->IntAttribute("frameCount");
    Vector<AnimationFrame*> frames(frameCount);

    int i = 0;
    string spriteId;
    AtlasSprite* sprite;
    SpriteFrame* spriteFrame = nullptr;
    const char* p;

    Size mcSizeInPixels = Size(item->width, item->height);
    Size mcSize = CC_SIZE_PIXELS_TO_POINTS(mcSizeInPixels);

    TXMLElement* framesEle = root->FirstChildElement("frames");
    TXMLElement* frameEle = framesEle->FirstChildElement("frame");
    while (frameEle)
    {
        ToolSet::splitString(frameEle->Attribute("rect"), ',', arr);
        Rect rect = Rect(atoi(arr[0].c_str()), atoi(arr[1].c_str()), atoi(arr[2].c_str()), atoi(arr[3].c_str()));
        float addDelay = frameEle->IntAttribute("addDelay") / 1000.0f;

        p = frameEle->Attribute("sprite");
        if (p)
            spriteId = item->id + "_" + string(p);
        else if (rect.size.width != 0)
            spriteId = item->id + "_" + Value(i).asString();
        else
            spriteId.clear();

        spriteFrame = nullptr;
        if (!spriteId.empty())
        {
            auto it = _sprites.find(spriteId);
            if (it != _sprites.end())
            {
                sprite = it->second;
                PackageItem* atlasItem = getItem(sprite->atlas);
                if (atlasItem)
                {
                    spriteFrame = createSpriteTexture(sprite);
                    spriteFrame->setOriginalSizeInPixels(mcSizeInPixels);
                    spriteFrame->setOriginalSize(mcSize);
                }
            }
        }

        if (spriteFrame == nullptr)
            spriteFrame = SpriteFrame::createWithTexture(_emptyTexture, Rect(Vec2::ZERO, Size::ZERO));
        spriteFrame->setOffset(Vec2(rect.origin.x - (mcSize.width - rect.size.width) / 2, -(rect.origin.y - (mcSize.height - rect.size.height) / 2)));
        AnimationFrame* frame = AnimationFrame::create(spriteFrame, addDelay / interval + 1, ValueMapNull);
        frames.pushBack(frame);
        //tranfer to AnimationFrame
        spriteFrame->release();

        i++;
        frameEle = frameEle->NextSiblingElement("frame");
    }
    delete xml;

    if (swing)
    {
        int cnt = (int)frames.size();
        for (int i = cnt - 1; i >= 1; i++)
        {
            AnimationFrame* frame = frames.at(i);
            frames.pushBack(frame);
        }
    }

    item->animation->initWithAnimationFrames(frames, interval, 1);
}

void UIPackage::loadFont(PackageItem * item)
{
    Data* fntData = _descPack[item->id + ".fnt"];
    FastSplitter lines, props, fields;
    lines.start((char*)fntData->getBytes(), fntData->getSize(), '\n');
    bool ttf = false;
    int size = 0;
    int xadvance = 0;
    bool resizable = false;
    bool canTint = false;
    int lineHeight = 0;
    Texture2D* mainTexture = nullptr;
    AtlasSprite* mainSprite = nullptr;
    char keyBuf[30];
    char valueBuf[50];

    item->bitmapFont = BitmapFont::create(); //fontAtlas retain

    FontAtlas* fontAtlas = new FontAtlas(*item->bitmapFont);
    item->bitmapFont->_fontAtlas = fontAtlas;

    while (lines.next())
    {
        size_t len = lines.getTextLength();
        const char* line = lines.getText();
        if (len > 4 && memcmp(line, "info", 4) == 0)
        {
            props.start(line, len, ' ');
            while (props.next())
            {
                props.getKeyValuePair(keyBuf, sizeof(keyBuf), valueBuf, sizeof(valueBuf));

                if (strcmp(keyBuf, "face") == 0)
                {
                    ttf = true;

                    auto it = _sprites.find(item->id);
                    if (it != _sprites.end())
                    {
                        mainSprite = it->second;
                        PackageItem* atlasItem = getItem(mainSprite->atlas);
                        loadItem(atlasItem);
                        mainTexture = atlasItem->texture;
                    }
                }
                else if (strcmp(keyBuf, "size") == 0)
                    sscanf(valueBuf, "%d", &size);
                else if (strcmp(keyBuf, "resizable") == 0)
                    resizable = strcmp(valueBuf, "true") == 0;
                else if (strcmp(keyBuf, "colored") == 0)
                    canTint = strcmp(valueBuf, "true") == 0;
            }

            if (size == 0)
                size = lineHeight;
            else if (lineHeight == 0)
                lineHeight = size;
        }
        else if (len > 6 && memcmp(line, "common", 6) == 0)
        {
            props.start(line, len, ' ');
            while (props.next())
            {
                props.getKeyValuePair(keyBuf, sizeof(keyBuf), valueBuf, sizeof(valueBuf));

                if (strcmp(keyBuf, "lineHeight") == 0)
                    sscanf(valueBuf, "%d", &lineHeight);

                if (strcmp(keyBuf, "xadvance") == 0)
                    sscanf(valueBuf, "%d", &xadvance);
            }
        }
        else if (len > 4 && memcmp(line, "char", 4) == 0)
        {
            FontLetterDefinition def;
            memset(&def, 0, sizeof(def));

            int bx = 0, by = 0, charId = 0;
            int bw = 0, bh = 0;
            PackageItem* charImg = nullptr;

            props.start(line, len, ' ');
            while (props.next())
            {
                props.getKeyValuePair(keyBuf, sizeof(keyBuf), valueBuf, sizeof(valueBuf));

                if (strcmp(keyBuf, "id") == 0)
                    sscanf(valueBuf, "%d", &charId);
                else if (strcmp(keyBuf, "x") == 0)
                    sscanf(valueBuf, "%d", &bx);
                else if (strcmp(keyBuf, "y") == 0)
                    sscanf(valueBuf, "%d", &by);
                else if (strcmp(keyBuf, "xoffset") == 0)
                    sscanf(valueBuf, "%f", &def.offsetX);
                else if (strcmp(keyBuf, "yoffset") == 0)
                    sscanf(valueBuf, "%f", &def.offsetY);
                else if (strcmp(keyBuf, "width") == 0)
                    sscanf(valueBuf, "%d", &bw);
                else if (strcmp(keyBuf, "height") == 0)
                    sscanf(valueBuf, "%d", &bh);
                else if (strcmp(keyBuf, "xadvance") == 0)
                    sscanf(valueBuf, "%d", &def.xAdvance);
                else if (!ttf && strcmp(keyBuf, "img") == 0)
                    charImg = getItem(valueBuf);
            }

            if (ttf)
            {
                Rect tempRect = Rect(bx + mainSprite->rect.origin.x, by + mainSprite->rect.origin.y, bw, bh);
                tempRect = CC_RECT_PIXELS_TO_POINTS(tempRect);
                def.U = tempRect.origin.x;
                def.V = tempRect.origin.y;
                def.width = tempRect.size.width;
                def.height = tempRect.size.height;
                def.validDefinition = true;
            }
            else if (charImg)
            {
                loadItem(charImg);

                Rect tempRect = charImg->spriteFrame->getRectInPixels();
                bw = tempRect.size.width;
                bh = tempRect.size.height;
                tempRect = CC_RECT_PIXELS_TO_POINTS(tempRect);
                def.U = tempRect.origin.x;
                def.V = tempRect.origin.y;
                def.width = tempRect.size.width;
                def.height = tempRect.size.height;
                if (mainTexture == nullptr)
                    mainTexture = charImg->spriteFrame->getTexture();
                def.validDefinition = true;
            }
            fontAtlas->addLetterDefinition(charId, def);

            if (size == 0)
                size = bh;
            if (!ttf && lineHeight < size)
                lineHeight = size;
        }
    }

    fontAtlas->addTexture(mainTexture, 0);
    fontAtlas->setLineHeight(lineHeight);
    item->bitmapFont->_originalFontSize = size;
    item->bitmapFont->_resizable = resizable;
    item->bitmapFont->_canTint = canTint;
}

void UIPackage::loadComponent(PackageItem * item)
{
    Data* xmlData = _descPack[item->id + ".xml"];
    TXMLDocument* doc = new TXMLDocument();
    doc->Parse((const char*)xmlData->getBytes(), xmlData->getSize());
    item->componentData = doc;
}

void UIPackage::loadComponentChildren(PackageItem * item)
{
    TXMLElement* listNode = item->componentData->RootElement()->FirstChildElement("displayList");
    item->displayList = new vector<DisplayListItem*>();
    const char *p;
    if (listNode)
    {
        TXMLElement* cxml = listNode->FirstChildElement();
        DisplayListItem* di;
        while (cxml)
        {
            p = cxml->Attribute("src");
            if (p)
            {
                string src = p;
                string pkgId = (p = cxml->Attribute("pkg")) ? p : "";
                UIPackage* pkg;
                if (!pkgId.empty() && pkgId.compare(item->owner->getId()) != 0)
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
                if (strcmp(cxml->Name(), "text") == 0 && cxml->BoolAttribute("input"))
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
    PackageItem* pi = getItemByName(resName);
    CCASSERT(pi, StringUtils::format("FairyGUI: resource not found - %s in  %s",
        resName.c_str(), _name.c_str()).c_str());

    return createObject(pi);
}

GObject * UIPackage::createObject(PackageItem * item)
{
    loadItem(item);

    GObject* g = UIObjectFactory::newObject(item);
    if (g == nullptr)
        return nullptr;

    _constructing++;
    g->constructFromResource();
    _constructing--;
    return g;
}

NS_FGUI_END
