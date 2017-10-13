#ifndef __GCOMPONENT_H__
#define __GCOMPONENT_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GObject.h"
#include "Transition.h"
#include "ScrollPane.h"

NS_FGUI_BEGIN

class Controller;

class GComponent : public GObject
{
public:
    enum ChildrenRenderOrder
    {
        Ascent,
        Descent,
        Arch,
    };

    enum OverflowType
    {
        OverflowVisible,
        OverflowHidden,
        OverflowScroll
    };

    enum ScrollType
    {
        ScrollHorizontal,
        ScrollVertical,
        ScrollBoth
    };

    enum ScrollBarDisplayType
    {
        ScrollBarDisplayDefault,
        ScrollBarDisplayVisible,
        ScrollBarDisplayAuto,
        ScrollBarDisplayHidden
    };

    GComponent();
    ~GComponent();

    CREATE_FUNC(GComponent);

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

    Controller* getController(const std::string& name);
    void removeController(Controller* c);
    void applyController(Controller* c);
    void applyAllControllers();

    void setBoundsChangedFlag();
    void ensureBoundsCorrect();

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

    virtual void visit();

    void setupScroll(const cocos2d::Vec4& scrollBarMargin,
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

    void buildNativeDisplayList();

    cocos2d::Vector<GObject*> _children;
    cocos2d::Vector<Controller*> _controllers;
    cocos2d::Vector<Transition*> _transitions;
    bool _opaque;
    int _sortingChildCount;
    ChildrenRenderOrder _childrenRenderOrder;
    int _apexIndex;
    Controller* _applyingController;
    cocos2d::Vec4 _margin;

    cocos2d::Node* _container;
    cocos2d::Node* _rootContainer;
    ScrollPane* _scrollPane;
};

NS_FGUI_END

#endif // __GCOMPONENT_H__
