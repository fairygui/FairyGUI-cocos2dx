#include "GObject.h"
#include "utils/ToolSet.h"
#include "GGroup.h"
#include "GComponent.h"
#include "GList.h"
#include "GRoot.h"
#include "gears/GearXY.h"
#include "gears/GearSize.h"
#include "gears/GearColor.h"
#include "gears/GearAnimation.h"
#include "gears/GearLook.h"
#include "gears/GearText.h"
#include "gears/GearIcon.h"

NS_FGUI_BEGIN
USING_NS_CC;

using namespace std;
using namespace tinyxml2;

uint32_t GObject::_gInstanceCounter = 0;
GObject* GObject::draggingObject = nullptr;

GObject::GObject()
    : _x(0.0f),
    _y(0.0f),
    _width(0.0f),
    _height(0.0f),
    _rawWidth(0.0f),
    _rawHeight(0.0f),
    _sizePercentInGroup(0.0f),
    _pivotX(0.0f),
    _pivotY(0.0f),
    _pivotAsAnchor(false),
    _alpha(1.0f),
    _rotation(0.0f),
    _visible(true),
    _internalVisible(true),
    _handlingController(false),
    _touchable(true),
    _grayed(false),
    _draggable(false),
    _scaleX(1.0f),
    _scaleY(1.0f),
    _sortingOrder(0),
    _focusable(false),
    _pixelSnapping(false),
    _group(nullptr),
    _parent(nullptr),
    _displayObject(nullptr),
    _sizeImplType(0),
    _underConstruct(false),
    _gearLocked(false),
    _gears{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr },
    name(""),
    packageItem(nullptr),
    data(nullptr),
    sourceWidth(0),
    sourceHeight(0),
    initWidth(0),
    initHeight(0),
    minWidth(0),
    maxWidth(0),
    minHeight(0),
    maxHeight(0)
{
    id = "_n" + Value(_gInstanceCounter++).asString();

    _relations = new Relations(this);

    _eventDispatcher = Director::getInstance()->getEventDispatcher();
    _eventDispatcher->retain();
}

GObject::~GObject()
{
    removeFromParent();
    removeEventListeners();
    if (_displayObject != nullptr)
        CC_SAFE_RELEASE(_displayObject);
    CC_SAFE_RELEASE(_eventDispatcher);
    for (int i = 0; i < 8; i++)
    {
        if (_gears[i] != nullptr)
            delete _gears[i];
    }
    delete _relations;
}

void GObject::setX(float value)
{
    setPosition(value, _y);
}

void GObject::setY(float value)
{
    setPosition(_x, value);
}

void GObject::setPosition(float xv, float yv)
{
    if (_x != xv || _y != yv)
    {
        float dx = xv - _x;
        float dy = yv - _y;
        _x = xv;
        _y = yv;

        handlePositionChanged();

        GGroup* g = dynamic_cast<GGroup*>(this);
        if (g != nullptr)
            g->moveChildren(dx, dy);

        updateGear(1);

        if (_parent != nullptr && dynamic_cast<GList*>(_parent) == nullptr)
        {
            _parent->setBoundsChangedFlag();
            if (_group != nullptr)
                _group->setBoundsChangedFlag();

            dispatchEvent(UIEventType::PositionChange);
        }

        //if (draggingObject == this && !sUpdateInDragging)
        //sGlobalRect = this.LocalToGlobal(new Rect(0, 0, this.width, this.height));
    }
}

void GObject::setPixelSnapping(bool value)
{
    if (_pixelSnapping != value)
    {
        _pixelSnapping = value;
        handlePositionChanged();
    }
}

float GObject::getWidth()
{
    return _width;
}

float GObject::getHeight()
{
    return _height;
}

void GObject::setSize(float wv, float hv, bool ignorePivot /*= false*/)
{
    if (_rawWidth != wv || _rawHeight != hv)
    {
        _rawWidth = wv;
        _rawHeight = hv;
        if (wv < minWidth)
            wv = minWidth;
        else if (maxWidth > 0 && wv > maxWidth)
            wv = maxWidth;
        if (hv < minHeight)
            hv = minHeight;
        else if (maxHeight > 0 && hv > maxHeight)
            hv = maxHeight;
        float dWidth = wv - _width;
        float dHeight = hv - _height;
        _width = wv;
        _height = hv;

        handleSizeChanged();

        if (_pivotX != 0 || _pivotY != 0)
        {
            if (!_pivotAsAnchor)
            {
                if (!ignorePivot)
                    this->setPosition(_x - _pivotX * dWidth, _y - _pivotY * dHeight);
                else
                    this->handlePositionChanged();
            }
            else
                this->handlePositionChanged();
        }

        GGroup* g = dynamic_cast<GGroup*>(this);
        if (g != nullptr)
            g->resizeChildren(dWidth, dHeight);

        updateGear(2);

        if (_parent != nullptr)
        {
            _relations->onOwnerSizeChanged(dWidth, dHeight);
            _parent->setBoundsChangedFlag();
            if (_group != nullptr)
                _group->setBoundsChangedFlag(true);
        }

        dispatchEvent(UIEventType::SizeChange);
    }
}

void GObject::center(bool restraint /*= false*/)
{
    GComponent* r;
    if (_parent != nullptr)
        r = _parent;
    else
        r = GRoot::getInstance();

    setPosition((int)((r->_width - _width) / 2), (int)((r->_height - _height) / 2));
    if (restraint)
    {
        addRelation(r, RelationType::Center_Center);
        addRelation(r, RelationType::Middle_Middle);
    }
}

void GObject::setScale(float xv, float yv)
{
    if (_scaleX != xv || _scaleY != yv)
    {
        _scaleX = xv;
        _scaleY = yv;
        handleScaleChanged();

        updateGear(2);
    }
}

void GObject::setRotation(float value)
{
    _rotation = value;
    if (_displayObject != nullptr)
        _displayObject->setRotation(_rotation);
    updateGear(3);
}

void GObject::setGrayed(bool value)
{
    if (_grayed != value)
    {
        _grayed = value;
        handleGrayedChanged();
    }
}

void GObject::makeFullScreen()
{
    this->setSize(GRoot::getInstance()->getWidth(), GRoot::getInstance()->getHeight());
}

uint32_t GObject::addClickListener(const EventCallback& callback)
{
    return addEventListener(UIEventType::Click, callback, 0);
}

uint32_t GObject::addClickListener(const EventCallback& callback, uint32_t callbackId)
{
    return addEventListener(UIEventType::Click, callback, callbackId);
}

void GObject::removeClickListener(uint32_t callbackId)
{
    removeEventListener(UIEventType::Click, callbackId);
}

GComponent * GObject::asCom()
{
    return dynamic_cast<GComponent*>(this);
}

void GObject::setVisible(bool value)
{
    if (_visible != value)
    {
        _visible = value;
        if (_displayObject)
            _displayObject->setVisible(value);
    }
}

void GObject::setTouchable(bool value)
{
    _touchable = value;
}

void GObject::setSortingOrder(int value)
{
    if (value < 0)
        value = 0;
    if (_sortingOrder != value)
    {
        int old = _sortingOrder;
        _sortingOrder = value;
        if (_parent != nullptr)
            _parent->childSortingOrderChanged(this, old, _sortingOrder);
    }
}

void GObject::setGroup(GGroup * value)
{
    if (_group != value)
    {
        if (_group != nullptr)
            _group->setBoundsChangedFlag(true);
        _group = value;
        if (_group != nullptr)
            _group->setBoundsChangedFlag(true);
    }
}

bool GObject::onStage()
{
    GObject* p = this;
    while (p->_parent != nullptr)
        p = p->_parent;

    return p == GRoot::getInstance();
}

Vec2 GObject::globalToLocal(const Vec2 & pt)
{
    Vec2 pt2 = pt;
    pt2.y = GRoot::getInstance()->getHeight() - pt2.y;
    pt2 = _displayObject->convertToNodeSpace(pt2);
    if (dynamic_cast<GComponent*>(this))
        pt2.y = -pt2.y;
    else
        pt2.y = _displayObject->getContentSize().height - pt2.y;
    return pt2;
}

void GObject::addRelation(GObject * target, RelationType relationType)
{
    _relations->add(target, relationType, false);
}

void GObject::addRelation(GObject * target, RelationType relationType, bool usePercent)
{
    _relations->add(target, relationType, usePercent);
}

void GObject::removeRelation(GObject * target, RelationType relationType)
{
    _relations->remove(target, relationType);
}

GearBase* GObject::getGear(int index)
{
    GearBase* gear = _gears[index];
    if (gear == nullptr)
    {
        switch (index)
        {
        case 0:
            gear = new GearDisplay(this);
            break;
        case 1:
            gear = new GearXY(this);
            break;
        case 2:
            gear = new GearSize(this);
            break;
        case 3:
            gear = new GearLook(this);
            break;
        case 4:
            gear = new GearColor(this);
            break;
        case 5:
            gear = new GearAnimation(this);
            break;
        case 6:
            gear = new GearText(this);
            break;
        case 7:
            gear = new GearIcon(this);
            break;
        }
        _gears[index] = gear;
    }
    return gear;
}

void GObject::updateGear(int index)
{
    if (_underConstruct || _gearLocked)
        return;

    GearBase* gear = _gears[index];
    if (gear != nullptr && gear->getController() != nullptr)
        gear->updateState();
}

bool GObject::checkGearController(int index, Controller* c)
{
    return _gears[index] != nullptr && _gears[index]->getController() == c;
}

void GObject::updateGearFromRelations(int index, float dx, float dy)
{
    if (_gears[index] != nullptr)
        _gears[index]->updateFromRelations(dx, dy);
}

uint32_t GObject::addDisplayLock()
{
    GearDisplay* gearDisplay = (GearDisplay*)_gears[0];
    if (gearDisplay != nullptr && gearDisplay->getController() != nullptr)
    {
        uint32_t ret = gearDisplay->addLock();
        checkGearDisplay();

        return ret;
    }
    else
        return 0;
}

void GObject::releaseDisplayLock(uint32_t token)
{
    GearDisplay* gearDisplay = (GearDisplay*)_gears[0];
    if (gearDisplay != nullptr && gearDisplay->getController() != nullptr)
    {
        gearDisplay->releaseLock(token);
        checkGearDisplay();
    }
}

void GObject::checkGearDisplay()
{
    if (_handlingController)
        return;

    bool connected = _gears[0] == nullptr || ((GearDisplay*)_gears[0])->isConnected();
    if (connected != _internalVisible)
    {
        _internalVisible = connected;
        if (_parent != nullptr)
            _parent->childStateChanged(this);
    }
}

void GObject::setText(const std::string & text)
{
}

const std::string& GObject::getText()
{
    return STD_STRING_EMPTY;
}

void GObject::setIcon(const std::string & text)
{
}

std::string GObject::getResourceURL()
{
    if (packageItem != nullptr)
        return "ui://" + packageItem->owner->getId() + packageItem->id;
    else
        return STD_STRING_EMPTY;
}

const std::string & GObject::getIcon()
{
    return STD_STRING_EMPTY;
}

bool GObject::inContainer()
{
    return _displayObject != nullptr && _displayObject->getParent() != nullptr;
}

void GObject::removeFromParent()
{
    if (_parent != nullptr)
        _parent->removeChild(this);
}

bool GObject::finalVisible()
{
    return _visible && _internalVisible && (_group == nullptr || _group->finalVisible());
}

bool GObject::init()
{
    return false;
}

void GObject::handlePositionChanged()
{
    if (_displayObject)
    {
        float xv = _x;
        float yv = _y;
        if (!_pivotAsAnchor)
        {
            xv += _width * _pivotX;
            yv += _height * _pivotY;
        }
        if (_pixelSnapping)
        {
            xv = (int)xv;
            yv = (int)yv;
        }
        _displayObject->setPosition(Vec2(xv, -yv));
    }
}

void GObject::handleSizeChanged()
{
    if (_displayObject)
    {
        if (_sizeImplType == 0 || sourceWidth == 0 || sourceHeight == 0)
            _displayObject->setContentSize(cocos2d::Size(_width, _height));
        else
            _displayObject->setScale(_scaleX * _width / sourceWidth, _scaleY * _height / sourceHeight);
    }
}

void GObject::handleScaleChanged()
{
    if (_displayObject)
    {
        if (_sizeImplType == 0 || sourceWidth == 0 || sourceHeight == 0)
            _displayObject->setScale(_scaleX, _scaleY);
        else
            _displayObject->setScale(_scaleX * _width / sourceWidth, _scaleY * _height / sourceHeight);
    }
}

void GObject::handleGrayedChanged()
{
    //if (_displayObject)
    //_displayObject->
}

void GObject::handleControllerChanged(Controller * c)
{
    _handlingController = true;
    for (int i = 0; i < 8; i++)
    {
        GearBase* gear = _gears[i];
        if (gear != nullptr && gear->getController() == c)
            gear->apply();
    }
    _handlingController = false;

    checkGearDisplay();
}

void GObject::constructFromResource()
{
}

void GObject::setup_BeforeAdd(tinyxml2::XMLElement * xml)
{
    const char *p;
    Vec2 v2;
    Vec4 v4;

    p = xml->Attribute("id");
    if (p)
        id = p;

    p = xml->Attribute("name");
    if (p)
        name = p;


    p = xml->Attribute("xy");
    if (p)
    {
        ToolSet::splitString(p, ',', v2, true);
        setPosition(v2.x, v2.y);
    }

    p = xml->Attribute("size");
    if (p)
    {
        ToolSet::splitString(p, ',', v2, true);
        initWidth = v2.x;
        initHeight = v2.y;
        setSize(initWidth, initHeight, true);
    }

    p = xml->Attribute("restrictSize");
    if (p)
    {
        ToolSet::splitString(p, ',', v4, true);
        minWidth = v4.x;
        maxWidth = v4.y;
        minHeight = v4.z;
        maxHeight = v4.w;
    }

    p = xml->Attribute("scale");
    if (p)
    {
        ToolSet::splitString(p, ',', v2);
        setScale(v2.x, v2.y);
    }

    p = xml->Attribute("rotation");
    if (p)
        this->setRotation(atoi(p));
}

void GObject::setup_AfterAdd(tinyxml2::XMLElement * xml)
{
    string str;

    const char*p = xml->Attribute("group");
    if (p)
        _group = dynamic_cast<GGroup*>(_parent->getChildById(p));

    XMLElement* exml = xml->FirstChildElement();
    while (exml)
    {
        int gearIndex = ToolSet::parseGearIndex(exml->Name());
        if (gearIndex != -1)
            getGear(gearIndex)->setup(exml);

        exml = exml->NextSiblingElement();
    }
}

GObject* GObject::hitTest(const Vec2 &pt, const Camera* camera)
{
    Rect rect;
    rect.size = _displayObject->getContentSize();
    if (isScreenPointInRect(pt, camera, _displayObject->getWorldToNodeTransform(), rect, nullptr))
        return this;
    else
        return nullptr;
}

NS_FGUI_END