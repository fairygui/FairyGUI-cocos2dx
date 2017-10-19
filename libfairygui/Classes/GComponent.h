#ifndef __GCOMPONENT_H__
#define __GCOMPONENT_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GObject.h"
#include "Transition.h"
#include "Margin.h"

NS_FGUI_BEGIN

class Controller;
class ScrollPane;
class GGroup;

class GComponent : public GObject
{
public:
    GComponent();
    ~GComponent();

    CREATE_FUNC(GComponent);

    cocos2d::Node* getContainer() { return _container; }

    GObject* addChild(GObject* child);
    GObject* addChildAt(GObject* child, int nIndex);
    GObject * getChildAt(int index);
    GObject * getChild(const std::string& name);
    GObject * removeChild(GObject * child);
    GObject * removeChildAt(int index);
    void removeChildren();
    void removeChildren(int beginIndex, int endIndex);
    GObject * getVisibleChild(const std::string& name);
    GObject * getChildInGroup(GGroup * group, const std::string& name);
    GObject * getChildById(const std::string& id);
    void setChildIndex(GObject* child, int nIndex);
    void swapChildren(GObject* child1, GObject* child2);
    void swapChildrenAt(int index1, int index2);
    int numChildren();
    void addController(Controller* controller);
    int getChildCount() { return _children.size(); }

    const Margin& getMargin() { return _margin; }
    void setMargin(const Margin& value);
    const cocos2d::Vec2& getAlignOffset() { return _alignOffset; }

    Controller* getController(const std::string& name);
    void removeController(Controller* c);
    void applyController(Controller* c);
    void applyAllControllers();

    void setBoundsChangedFlag();
    void ensureBoundsCorrect();

    virtual cocos2d::Vec2 getSnappingPosition(const cocos2d::Vec2& pt);

    virtual bool init() override;
    virtual void constructFromResource() override;
    void constructFromResource(std::vector<GObject*>* objectPool, int poolIndex);
    virtual void constructFromXML(tinyxml2::XMLElement* xml);
    virtual void setup_AfterAdd(tinyxml2::XMLElement* xml) override;
    virtual GObject* hitTest(const cocos2d::Vec2 & pt, const cocos2d::Camera * camera) override;

    void childSortingOrderChanged(GObject* child, int oldValue, int newValue);
    void childStateChanged(GObject * child);

    void adjustRadioGroupDepth(GObject* obj, Controller* c);

    bool _buildingDisplayList;

protected:
    virtual void handleSizeChanged() override;
    virtual void handleGrayedChanged() override;
    virtual void handleControllerChanged(Controller* c) override;

    virtual void updateBounds();
    void setBounds(float ax, float ay, float aw, float ah);

    void setupScroll(const Margin& scrollBarMargin,
        ScrollType scroll, ScrollBarDisplayType scrollBarDisplay, int flags,
        const std::string& vtScrollBarRes, const std::string& hzScrollBarRes,
        const std::string& headerRes, const std::string& footerRes);
    void setupOverflow(OverflowType overflow);

    bool _boundsChanged;
    bool _trackBounds;

private:
    int getInsertPosForSortingChild(GObject * target);
    int getChildIndex(GObject * child);
    int _setChildIndex(GObject* child, int oldIndex, int index);

    CALL_LATER_FUNC(GComponent, doUpdateBounds);
    CALL_LATER_FUNC(GComponent, buildNativeDisplayList);

    cocos2d::Vector<GObject*> _children;
    cocos2d::Vector<Controller*> _controllers;
    cocos2d::Vector<Transition*> _transitions;
    bool _opaque;
    int _sortingChildCount;
    ChildrenRenderOrder _childrenRenderOrder;
    int _apexIndex;
    Controller* _applyingController;
    Margin _margin;
    cocos2d::Vec2 _alignOffset;

    cocos2d::Node* _container;
    cocos2d::Node* _rootContainer;
    ScrollPane* _scrollPane;
};

NS_FGUI_END

#endif // __GCOMPONENT_H__
