#ifndef __GOBJECT_H__
#define __GOBJECT_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "PackageItem.h"
#include "UIConfig.h"
#include "tinyxml2/tinyxml2.h"
#include "event/UIEventDispatcher.h"
#include "Controller.h"
#include "Relations.h"
#include "gears/GearBase.h"
#include "gears/GearDisplay.h"

NS_FGUI_BEGIN

class GComponent;
class GGroup;

class GObject : public UIEventDispatcher
{
public:
    GObject();
    ~GObject();

    CREATE_FUNC(GObject);

    std::string id;
    std::string name;
    void * data;
    int sourceWidth;
    int sourceHeight;
    int initWidth;
    int initHeight;
    int minWidth;
    int maxWidth;
    int minHeight;
    int maxHeight;

    PackageItem* packageItem;

    GComponent* getParent() { return _parent; }
    const GComponent* getParent() const { return _parent; }
    cocos2d::Node* displayObject() { return _displayObject; }

    virtual void constructFromResource();
    virtual void setup_BeforeAdd(tinyxml2::XMLElement* xml);
    virtual void setup_AfterAdd(tinyxml2::XMLElement* xml);

    void removeFromParent();

    float getX() { return _x; };
    void setX(float value);
    float getY() { return _y; };
    void setY(float value);
    void setPosition(float xv, float yv);
    cocos2d::Vec2 getPosition() { return cocos2d::Vec2(_x, _y); }

    bool getPixelSnapping() { return _pixelSnapping; }
    void setPixelSnapping(bool value);

    float getWidth();
    void setWidth(float value) { setSize(value, _rawHeight); }
    float getHeight();
    void setHeight(float value) { setSize(_rawWidth, value); }
    void setSize(float wv, float hv, bool ignorePivot = false);
    void center(bool restraint = false);
    float getActualWidth() { return this->_width * _scaleX; }
    float getActualHeight() { return this->_height * _scaleY; }

    float getScaleX() { return _scaleX; }
    void setScaleX(float value) { setScale(value, _scaleY); }
    float getScaleY() { return _scaleY; }
    void setScaleY(float value) { setScale(_scaleX, value); }
    void setScale(float xv, float yv);

    void setRotation(float value);
    float getRotation() { return _rotation; }

    void setGrayed(bool value);
    bool isGrayed() { return _grayed; }

    bool isVisible() { return _visible; }
    void setVisible(bool value);
    bool isTouchable() { return _touchable; }
    void setTouchable(bool value);

    int getSortingOrder() { return this->_sortingOrder; }
    void setSortingOrder(int value);

    GGroup* getGroup() { return this->_group; }
    void setGroup(GGroup* value);

    bool onStage();

    cocos2d::Vec2 globalToLocal(const cocos2d::Vec2& pt);

    Relations* relations() { return _relations; }
    void addRelation(GObject* target, RelationType relationType);
    void addRelation(GObject* target, RelationType relationType, bool usePercent);
    void removeRelation(GObject* target, RelationType relationType);

    GearBase* getGear(int index);
    bool checkGearController(int index, Controller* c);
    void updateGearFromRelations(int index, float dx, float dy);
    uint32_t addDisplayLock();
    void releaseDisplayLock(uint32_t token);

    virtual const std::string& getText();
    virtual void setText(const std::string& text);

    virtual const std::string& getIcon();
    virtual void setIcon(const std::string& text);

    std::string getResourceURL();

    bool inContainer();
    void makeFullScreen();

    cocos2d::EventDispatcher* getCCEventDispatcher() { return _eventDispatcher; }

    uint32_t addClickListener(const EventCallback& callback);
    uint32_t addClickListener(const EventCallback& callback, uint32_t callbackId);
    void removeClickListener(uint32_t callbackId);

    GComponent* asCom();

    virtual GObject* hitTest(const cocos2d::Vec2 & pt, const cocos2d::Camera * camera);

    bool _underConstruct;
    bool _gearLocked;

protected:
    GComponent* _parent;
    cocos2d::Node* _displayObject;
    cocos2d::EventDispatcher* _eventDispatcher;

    //Size的实现方式，有两种，0-GObject的w/h等于DisplayObject的w/h。1-GObject的sourceWidth/sourceHeight等于DisplayObject的w/h，剩余部分由scale实现
    int _sizeImplType;

    virtual bool init();

    virtual void handleSizeChanged();
    virtual void handleScaleChanged();
    virtual void handleGrayedChanged();
    virtual void handlePositionChanged();
    virtual void handleControllerChanged(Controller* c);

    void updateGear(int index);
    void checkGearDisplay();

private:
    void setParent(GComponent* value) { _parent = value; }
    bool finalVisible();

    float _x;
    float _y;
    float _width;
    float _height;
    float _rawWidth;
    float _rawHeight;
    float _sizePercentInGroup;
    float _pivotX;
    float _pivotY;
    bool _pivotAsAnchor;
    float _alpha;
    float _rotation;
    bool _visible;
    bool _internalVisible;
    bool _handlingController;
    bool _touchable;
    bool _grayed;
    bool _draggable;
    float _scaleX;
    float _scaleY;
    int _sortingOrder;
    bool _focusable;
    std::string _tooltips;
    bool _pixelSnapping;
    GGroup* _group;
    Relations* _relations;
    GearBase* _gears[8];

    static uint32_t _gInstanceCounter;

    friend class GComponent;
    friend class RelationItem;
};

NS_FGUI_END

#endif // __GOBJECT_H__
