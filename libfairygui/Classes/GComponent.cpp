#include "GComponent.h"
#include "Controller.h"
#include "utils/ToolSet.h"
#include "UIObjectFactory.h"
#include "Relations.h"
#include "GGroup.h"
#include "display/ScissorClipNode.h"

NS_FGUI_BEGIN
USING_NS_CC;

using namespace std;
using namespace tinyxml2;

GComponent::GComponent()
    :_sortingChildCount(0),
    _opaque(false),
    _container(nullptr),
    _rootContainer(nullptr),
    _childrenRenderOrder(ChildrenRenderOrder::Ascent),
    _apexIndex(0),
    _applyingController(nullptr),
    _buildingDisplayList(false),
    _scrollPane(nullptr),
    _boundsChanged(false),
    _trackBounds(false)
{
}

GComponent::~GComponent()
{
    _children.clear();
    _controllers.clear();
    _transitions.clear();
    if (_container != _rootContainer)
        CC_SAFE_RELEASE(_container);
}

GObject* GComponent::addChild(GObject* child)
{
    addChildAt(child, _children.size());
    return child;
}

GObject* GComponent::addChildAt(GObject* child, int index)
{
    CCASSERT(index >= 0 && index <= _children.size(), "Invalid child index");
    CCASSERT(child != nullptr, "Argument must be non-nil");

    if (child->getParent() == this)
    {
        setChildIndex(child, index);
    }
    else
    {
        child->removeFromParent();
        child->setParent(this);

        int cnt = _children.size();
        if (child->getSortingOrder() != 0)
        {
            _sortingChildCount++;
            index = getInsertPosForSortingChild(child);
        }
        else if (_sortingChildCount > 0)
        {
            if (index > (cnt - _sortingChildCount))
                index = cnt - _sortingChildCount;
        }

        if (index == cnt)
            _children.pushBack(child);
        else
            _children.insert(index, child);

        childStateChanged(child);
        setBoundsChangedFlag();
        if (child->getGroup() != nullptr)
            child->getGroup()->setBoundsChangedFlag(true);
    }
    return child;
}

int GComponent::getInsertPosForSortingChild(GObject* target)
{
    int cnt = _children.size();
    int i;
    for (i = 0; i < cnt; i++)
    {
        GObject* child = _children.at(i);
        if (child == target)
            continue;

        if (target->getSortingOrder() < child->getSortingOrder())
            break;
    }
    return i;
}

GObject* GComponent::removeChild(GObject* child)
{
    CCASSERT(child != nullptr, "Argument must be non-nil");

    int childIndex = _children.getIndex(child);
    if (childIndex != -1)
        removeChildAt(childIndex);
    return child;
}

GObject* GComponent::removeChildAt(int index)
{
    CCASSERT(index >= 0 && index <= _children.size(), "Invalid child index");

    GObject* child = _children.at(index);

    child->setParent(nullptr);

    if (child->getSortingOrder() != 0)
        _sortingChildCount--;

    child->setGroup(nullptr);
    if (child->inContainer())
    {
        _container->removeChild(child->displayObject());
        if (_childrenRenderOrder == ChildrenRenderOrder::Arch)
        {

        }
    }

    _children.erase(index);
    setBoundsChangedFlag();
    return child;
}

void GComponent::removeChildren()
{
    removeChildren(0, -1);
}

void GComponent::removeChildren(int beginIndex, int endIndex)
{
    if (endIndex < 0 || endIndex >= _children.size())
        endIndex = _children.size() - 1;

    for (int i = beginIndex; i <= endIndex; ++i)
        removeChildAt(beginIndex);
}

GObject* GComponent::getChildAt(int index)
{
    CCASSERT(index >= 0 && index <= _children.size(), "Invalid child index");

    return _children.at(index);
}

GObject* GComponent::getChild(const std::string& name)
{
    for (auto it = _children.cbegin(); it != _children.cend(); ++it)
    {
        if ((*it)->name.compare(name) == 0)
            return *it;
    }

    return nullptr;
}

GObject* GComponent::getVisibleChild(const std::string&)
{
    for (auto it = _children.cbegin(); it != _children.cend(); ++it)
    {
        if ((*it)->finalVisible() && (*it)->name.compare(name) == 0)
            return (*it);
    }

    return nullptr;
}

GObject* GComponent::getChildInGroup(GGroup* group, const std::string&)
{
    CCASSERT(group != nullptr, "Argument must be non-nil");

    for (auto it = _children.cbegin(); it != _children.cend(); ++it)
    {
        if ((*it)->getGroup() == group && (*it)->name.compare(name) == 0)
            return (*it);
    }

    return nullptr;
}

GObject* GComponent::getChildById(const std::string& id)
{
    for (auto it = _children.cbegin(); it != _children.cend(); ++it)
    {
        if ((*it)->id.compare(id) == 0)
            return (*it);
    }

    return nullptr;
}

int GComponent::getChildIndex(GObject* child)
{
    CCASSERT(child != nullptr, "Argument must be non-nil");

    return _children.getIndex(child);
}

void GComponent::setChildIndex(GObject* child, int nIndex)
{
    CCASSERT(child != nullptr, "Argument must be non-nil");

    int oldIndex = _children.getIndex(child);
    CCASSERT(oldIndex != -1, "Not a child of this container");

    if (child->getSortingOrder() != 0) //no effect
        return;

    int cnt = _children.size();
    if (_sortingChildCount > 0)
    {
        if (nIndex > (cnt - _sortingChildCount - 1))
            nIndex = cnt - _sortingChildCount - 1;
    }

    _setChildIndex(child, oldIndex, nIndex);
}

int GComponent::_setChildIndex(GObject* child, int oldIndex, int index)
{
    int cnt = _children.size();
    if (index > cnt)
        index = cnt;

    if (oldIndex == index)
        return oldIndex;

    _children.erase(oldIndex);
    if (index >= cnt)
        _children.pushBack(child);
    else
        _children.insert(index, child);

    if (child->inContainer())
    {
        if (_childrenRenderOrder == ChildrenRenderOrder::Ascent)
        {
            int fromIndex = min(index, oldIndex);
            int toIndex = min(max(index, oldIndex), cnt);
            for (int i = fromIndex; i <= toIndex; i++)
            {
                GObject* g = _children.at(i);
                if (g->inContainer())
                    g->displayObject()->setLocalZOrder(i);
            }
        }
        else if (_childrenRenderOrder == ChildrenRenderOrder::Descent)
        {
            int fromIndex = min(index, oldIndex);
            int toIndex = min(max(index, oldIndex), cnt);
            for (int i = fromIndex; i <= toIndex; i++)
            {
                GObject* g = _children.at(i);
                if (g->inContainer())
                    g->displayObject()->setLocalZOrder(cnt - 1 - i);
            }
        }
        else
        {
        }

        setBoundsChangedFlag();
    }

    return index;
}

void GComponent::swapChildren(GObject* child1, GObject* child2)
{

}

void GComponent::swapChildrenAt(int index1, int index2)
{

}

int GComponent::numChildren()
{
    return _children.size();
}

void GComponent::addController(Controller* controller)
{
    _controllers.pushBack(controller);
}

Controller* GComponent::getController(const std::string& name)
{
    int cnt = _controllers.size();
    for (int i = 0; i < cnt; ++i)
    {
        Controller* c = _controllers.at(i);
        if (c->name.compare(name) == 0)
            return c;
    }

    return nullptr;
}

void GComponent::removeController(Controller* c)
{
    int index = _controllers.getIndex(c);
    CCASSERT(index != -1, StringUtils::format("controller not exists: %s", c->name.c_str()).c_str());

    c->setParent(nullptr);
    applyController(c);
    _controllers.erase(index);
}

void GComponent::applyController(Controller * c)
{
    _applyingController = c;

    int cnt = _children.size();
    for (int i = 0; i < cnt; ++i)
    {
        GObject* child = _children.at(i);
        child->handleControllerChanged(c);
    }
    _applyingController = nullptr;
}

void GComponent::applyAllControllers()
{
    for (auto it = _controllers.cbegin(); it != _controllers.cend(); ++it)
        applyController(*it);
}

void GComponent::adjustRadioGroupDepth(GObject* obj, Controller* c)
{
    int cnt = _children.size();
    int i;
    GObject* child;
    int myIndex = -1, maxIndex = -1;
    for (i = 0; i < cnt; i++)
    {
        child = _children.at(i);
        if (child == obj)
        {
            myIndex = i;
        }
        else if (dynamic_cast<GButton*>(child)
            && ((GButton *)child)->getRelatedController() == c)
        {
            if (i > maxIndex)
                maxIndex = i;
        }
    }
    if (myIndex < maxIndex)
    {
        if (_applyingController != nullptr)
            _children.at(maxIndex)->handleControllerChanged(_applyingController);
        swapChildrenAt(myIndex, maxIndex);
    }
}

void GComponent::setupScroll(const Vec4& scrollBarMargin,
    ScrollType scroll, ScrollBarDisplayType scrollBarDisplay, int flags,
    const std::string& vtScrollBarRes, const std::string& hzScrollBarRes,
    const std::string& headerRes, const std::string& footerRes)
{
    if (_rootContainer == _container)
    {
        _container = Node::create();
        _container->retain();
        _rootContainer->addChild(_container);
        ((ScissorClipNode*)_rootContainer)->setClippingEnabled(true);
    }

    //_scrollPane = new ScrollPane(this, scroll, scrollBarMargin, scrollBarDisplay, flags, vtScrollBarRes, hzScrollBarRes, headerRes, footerRes);
}

void GComponent::setupOverflow(OverflowType overflow)
{
    if (overflow == OverflowType::OverflowHidden)
    {
        if (_rootContainer == _container)
        {
            _container = Node::create();
            _container->retain();
            _rootContainer->addChild(_container);
            ((ScissorClipNode*)_rootContainer)->setClippingEnabled(true);
        }

        _container->setPosition(_margin.x, -_margin.y);
    }
    else if (_margin.x != 0 || _margin.y != 0)
    {
        if (_rootContainer == _container)
        {
            _container = Node::create();
            _container->retain();
            _rootContainer->addChild(_container);
        }

        _container->setPosition(_margin.x, -_margin.y);
    }
}

void GComponent::handleSizeChanged()
{
    GObject::handleSizeChanged();

    if (_scrollPane != nullptr)
        ;// scrollPane.OnOwnerSizeChanged();

    /*if (rootContainer.hitArea is PixelHitTest)
    {
        PixelHitTest test = (PixelHitTest)rootContainer.hitArea;
        if (sourceWidth != 0)
            test.scaleX = this.width / this.sourceWidth;
        if (sourceHeight != 0)
            test.scaleY = this.height / this.sourceHeight;
    }*/
}

void GComponent::handleGrayedChanged()
{
    Controller* cc = getController("grayed");
    if (cc != nullptr)
        cc->setSelectedIndex(isGrayed() ? 1 : 0);
    else
        GObject::handleGrayedChanged();
}

void GComponent::handleControllerChanged(Controller* c)
{
    GObject::handleControllerChanged(c);

    if (_scrollPane != nullptr)
        ;// scrollPane.HandleControllerChanged(c);
}

void GComponent::childStateChanged(GObject* child)
{
    if (_buildingDisplayList)
        return;

    int cnt = _children.size();

    if (dynamic_cast<GGroup*>(child) != nullptr)
    {
        for (int i = 0; i < cnt; ++i)
        {
            GObject* g = _children.at(i);
            if (g->getGroup() == child)
                childStateChanged(g);
        }
        return;
    }

    if (child->displayObject() == nullptr)
        return;

    if (child->finalVisible())
    {
        if (child->displayObject()->getParent() == nullptr)
        {
            if (_childrenRenderOrder == ChildrenRenderOrder::Ascent)
            {
                _container->addChild(child->displayObject(), _children.getIndex(child));
            }
            else if (_childrenRenderOrder == ChildrenRenderOrder::Descent)
            {
                _container->addChild(child->displayObject(), cnt - 1 - _children.getIndex(child));
            }
            else
            {
            }
        }
    }
    else
    {
        if (child->displayObject()->getParent() != nullptr)
        {
            _container->removeChild(child->displayObject());
            if (_childrenRenderOrder == ChildrenRenderOrder::Arch)
            {

            }
        }
    }
}

void GComponent::childSortingOrderChanged(GObject* child, int oldValue, int newValue)
{
    if (newValue == 0)
    {
        _sortingChildCount--;
        setChildIndex(child, _children.size());
    }
    else
    {
        if (oldValue == 0)
            _sortingChildCount++;

        int oldIndex = _children.getIndex(child);
        int index = getInsertPosForSortingChild(child);
        if (oldIndex < index)
            _setChildIndex(child, oldIndex, index - 1);
        else
            _setChildIndex(child, oldIndex, index);
    }
}

void GComponent::buildNativeDisplayList()
{
    if (_displayObject == nullptr)
        return;

    int cnt = _children.size();
    if (cnt == 0)
        return;

    switch (_childrenRenderOrder)
    {
    case ChildrenRenderOrder::Ascent:
    {
        for (int i = 0; i < cnt; i++)
        {
            GObject* child = _children.at(i);
            if (child->displayObject() != nullptr && child->finalVisible())
                _container->addChild(child->displayObject(), i);
        }
    }
    break;
    case ChildrenRenderOrder::Descent:
    {
        for (int i = 0; i < cnt; i++)
        {
            GObject* child = _children.at(i);
            if (child->displayObject() != nullptr && child->finalVisible())
                _container->addChild(child->displayObject(), cnt - 1 - i);
        }
    }
    break;

    case ChildrenRenderOrder::Arch:
    {
        for (int i = 0; i < _apexIndex; i++)
        {
            GObject* child = _children.at(i);
            if (child->displayObject() != nullptr && child->finalVisible())
                _container->addChild(child->displayObject(), i);
        }
        for (int i = cnt - 1; i >= _apexIndex; i--)
        {
            GObject* child = _children.at(i);
            if (child->displayObject() != nullptr && child->finalVisible())
                _container->addChild(child->displayObject(), _apexIndex + cnt - 1 - i);
        }
    }
    break;
    }
}

void GComponent::setBoundsChangedFlag()
{
    if (_scrollPane == nullptr && !_trackBounds)
        return;

    _boundsChanged = true;
}

void GComponent::ensureBoundsCorrect()
{
    if (_boundsChanged)
        updateBounds();
}

void GComponent::updateBounds()
{
    float ax, ay, aw, ah;
    if (_children.size() > 0)
    {
        ax = FLT_MAX;
        ay = FLT_MAX;
        float ar = FLT_MIN, ab = FLT_MIN;
        float tmp;

        int cnt = _children.size();
        for (int i = 0; i < cnt; ++i)
        {
            GObject* child = _children.at(i);
            tmp = child->getX();
            if (tmp < ax)
                ax = tmp;
            tmp = child->getY();
            if (tmp < ay)
                ay = tmp;
            tmp = child->getX() + child->getActualWidth();
            if (tmp > ar)
                ar = tmp;
            tmp = child->getY() + child->getActualHeight();
            if (tmp > ab)
                ab = tmp;
        }
        aw = ar - ax;
        ah = ab - ay;
    }
    else
    {
        ax = 0;
        ay = 0;
        aw = 0;
        ah = 0;
    }
    setBounds(ax, ay, aw, ah);
}

void GComponent::setBounds(float ax, float ay, float aw, float ah)
{
    _boundsChanged = false;
    //if (scrollPane != null)
    //	scrollPane.SetContentSize(Mathf.RoundToInt(ax + aw), Mathf.RoundToInt(ay + ah));
}

GObject* GComponent::hitTest(const Vec2 &pt, const Camera* camera)
{
    GObject * target = nullptr;
    for (auto it = _children.rbegin(); it != _children.rend(); ++it)
    {
        if (!(*it)->_displayObject || !(*it)->_touchable || !(*it)->finalVisible())
            continue;

        target = (*it)->hitTest(pt, camera);
        if (target)
            return target;
    }

    if (_opaque)
    {
        Rect rect;
        rect.size = _displayObject->getContentSize();
        rect.origin = Vec2(0, -rect.size.height);
        if (isScreenPointInRect(pt, camera, _displayObject->getWorldToNodeTransform(), rect, nullptr))
            return this;
    }

    return nullptr;
}

bool GComponent::init()
{
    _rootContainer = ScissorClipNode::create();
    _rootContainer->retain();
    ((ScissorClipNode*)_rootContainer)->setVisitCallback(CC_CALLBACK_0(GComponent::visit, this));

    _container = _rootContainer;
    _displayObject = _rootContainer;

    return true;
}

void GComponent::visit()
{
    if (_boundsChanged)
        updateBounds();
}

void GComponent::constructFromResource()
{
    constructFromResource(nullptr, 0);
}

void GComponent::constructFromResource(std::vector<GObject*>* objectPool, int poolIndex)
{
    tinyxml2::XMLElement* xml = packageItem->componentData->RootElement();

    _underConstruct = true;

    Vec2 v2;
    const char *p;

    ToolSet::getArrayAttribute(xml, "size", v2, true);
    initWidth = sourceWidth = v2.x;
    initHeight = sourceHeight = v2.y;

    setSize(sourceWidth, sourceHeight);

    Vec4 v4;
    if (ToolSet::getArrayAttribute(xml, "restrictSize", v4, true))
    {
        minWidth = v4.x;
        maxWidth = v4.y;
        minHeight = v4.z;
        maxHeight = v4.w;
    }

    //float f1, f2;
    //if (ToolSet::getArrayAttribute(xml, "pivot", f1, f2))
    //	setPivot(f1, f2, xml->BoolAttribute("anchor"));

    _opaque = xml->Attribute("opaque") != "false";

    OverflowType overflow;
    p = xml->Attribute("overflow");
    if (p)
        overflow = ToolSet::parseOverflowType(p);
    else
        overflow = OverflowType::OverflowVisible;

    p = xml->Attribute("margin");
    if (p)
        ToolSet::splitString(p, ',', _margin);

    if (overflow == OverflowType::OverflowScroll)
    {
        ScrollType scroll;
        p = xml->Attribute("scroll");
        if (p)
            scroll = ToolSet::parseScrollType(p);
        else
            scroll = ScrollType::ScrollVertical;

        ScrollBarDisplayType scrollBarDisplay;
        p = xml->Attribute("scrollBar");
        if (p)
            scrollBarDisplay = ToolSet::parseScrollBarDisplayType(p);
        else
            scrollBarDisplay = ScrollBarDisplayType::ScrollBarDisplayDefault;

        int scrollBarFlags = xml->IntAttribute("scrollBarFlags");

        Vec4 scrollBarMargin;
        p = xml->Attribute("scrollBarMargin");
        if (p)
            ToolSet::splitString(p, ',', scrollBarMargin);

        std::string  vtScrollBarRes;
        std::string hzScrollBarRes;
        p = xml->Attribute("scrollBarRes");
        if (p)
            ToolSet::splitString(p, ',', vtScrollBarRes, hzScrollBarRes);

        string headerRes;
        string footerRes;
        p = xml->Attribute("ptrRes");
        if (p)
            ToolSet::splitString(p, ',', headerRes, footerRes);

        setupScroll(scrollBarMargin, scroll, scrollBarDisplay, scrollBarFlags,
            vtScrollBarRes, hzScrollBarRes, headerRes, footerRes);
    }
    else
        setupOverflow(overflow);

    _buildingDisplayList = true;

    XMLElement* exml = xml->FirstChildElement("controller");
    Controller* controller;
    while (exml)
    {
        controller = new Controller();
        _controllers.pushBack(controller);
        controller->setParent(this);
        controller->setup(exml);

        exml = exml->NextSiblingElement("controller");
    }

    GObject* child;

    std::vector<DisplayListItem*>& displayList = *packageItem->displayList;
    int childCount = displayList.size();
    for (int i = 0; i < childCount; i++)
    {
        DisplayListItem& di = *displayList[i];
        if (objectPool)
            child = (*objectPool)[poolIndex + i];
        else if (di.packageItem)
        {
            di.packageItem->load();
            child = UIObjectFactory::newObject(di.packageItem);
            child->packageItem = di.packageItem;
            child->constructFromResource();
        }
        else
            child = UIObjectFactory::newObject(di.type);

        child->_underConstruct = true;
        child->setup_BeforeAdd(di.desc);
        child->setParent(this);
        child->retain();
        _children.pushBack(child);
    }

    relations()->setup(xml);

    for (int i = 0; i < childCount; i++)
        _children.at(i)->relations()->setup(displayList[i]->desc);

    for (int i = 0; i < childCount; i++)
    {
        child = _children.at(i);
        child->setup_AfterAdd(displayList[i]->desc);
        child->_underConstruct = false;
    }

    applyAllControllers();

    _buildingDisplayList = false;
    _underConstruct = false;

    buildNativeDisplayList();
    setBoundsChangedFlag();

    constructFromXML(xml);
}

void GComponent::constructFromXML(tinyxml2::XMLElement * xml)
{
}

void GComponent::setup_AfterAdd(tinyxml2::XMLElement * xml)
{
    GObject::setup_AfterAdd(xml);
}

NS_FGUI_END

