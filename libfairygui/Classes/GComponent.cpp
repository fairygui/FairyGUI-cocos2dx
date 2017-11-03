#include "GComponent.h"
#include "UIObjectFactory.h"
#include "Relations.h"
#include "GGroup.h"
#include "ScrollPane.h"
#include "utils/ToolSet.h"
#include "display/FUIContainer.h"

NS_FGUI_BEGIN
USING_NS_CC;

using namespace std;
using namespace tinyxml2;

GComponent::GComponent() :
    _container(nullptr),
    _scrollPane(nullptr),
    _childrenRenderOrder(ChildrenRenderOrder::ASCENT),
    _apexIndex(0),
    _boundsChanged(false),
    _trackBounds(false),
    _opaque(false),
    _sortingChildCount(0),
    _applyingController(nullptr),
    _buildingDisplayList(false),
    _maskOwner(nullptr)
{
}

GComponent::~GComponent()
{
    _children.clear();
    _controllers.clear();
    _transitions.clear();
    CC_SAFE_RELEASE(_maskOwner);
    CC_SAFE_RELEASE(_container);
    CC_SAFE_RELEASE(_scrollPane);
}

void GComponent::handleInit()
{
    _displayObject = FUIContainer::create();
    _displayObject->retain();

    _container = Node::create();
    _container->retain();
    _container->setCascadeOpacityEnabled(true);
    _displayObject->addChild(_container);
}

GObject* GComponent::addChild(GObject* child)
{
    addChildAt(child, _children.size());
    return child;
}

GObject* GComponent::addChildAt(GObject* child, int index)
{
    CCASSERT(child != nullptr, "Argument must be non-nil");

    if (child->_parent == this)
    {
        setChildIndex(child, index);
    }
    else
    {
        child->removeFromParent();
        child->_parent = this;

        int cnt = _children.size();
        if (child->_sortingOrder != 0)
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
        if (child->_group != nullptr)
            child->_group->setBoundsChangedFlag(true);
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

        if (target->_sortingOrder < child->_sortingOrder)
            break;
    }
    return i;
}

void GComponent::removeChild(GObject* child)
{
    CCASSERT(child != nullptr, "Argument must be non-nil");

    int childIndex = _children.getIndex(child);
    if (childIndex != -1)
        removeChildAt(childIndex);
}

void GComponent::removeChildAt(int index)
{
    GObject* child = _children.at(index);

    child->_parent = nullptr;

    if (child->_sortingOrder != 0)
        _sortingChildCount--;

    child->setGroup(nullptr);
    if (child->_displayObject->getParent() != nullptr)
    {
        _container->removeChild(child->_displayObject, false);
        if (_childrenRenderOrder == ChildrenRenderOrder::ARCH)
            CALL_LATER(GComponent, buildNativeDisplayList);
    }

    _children.erase(index);
    setBoundsChangedFlag();
}

void GComponent::removeChildren(int beginIndex, int endIndex)
{
    if (endIndex < 0 || endIndex >= _children.size())
        endIndex = _children.size() - 1;

    for (int i = beginIndex; i <= endIndex; ++i)
        removeChildAt(beginIndex);
}

GObject* GComponent::getChildAt(int index) const
{
    return _children.at(index);
}

GObject* GComponent::getChild(const std::string& name) const
{
    for (const auto& child : _children)
    {
        if (child->name.compare(name) == 0)
            return child;
    }

    return nullptr;
}

GObject* GComponent::getChildInGroup(const GGroup* group, const std::string& name) const
{
    CCASSERT(group != nullptr, "Argument must be non-nil");

    for (const auto& child : _children)
    {
        if (child->_group == group && child->name.compare(name) == 0)
            return child;
    }

    return nullptr;
}

GObject* GComponent::getChildById(const std::string& id) const
{
    for (const auto& child : _children)
    {
        if (child->id.compare(id) == 0)
            return child;
    }

    return nullptr;
}

int GComponent::getChildIndex(const GObject* child) const
{
    CCASSERT(child != nullptr, "Argument must be non-nil");

    return _children.getIndex((GObject*)child);
}

void GComponent::setChildIndex(GObject* child, int index)
{
    CCASSERT(child != nullptr, "Argument must be non-nil");

    int oldIndex = _children.getIndex(child);
    CCASSERT(oldIndex != -1, "Not a child of this container");

    if (child->_sortingOrder != 0) //no effect
        return;

    int cnt = _children.size();
    if (_sortingChildCount > 0)
    {
        if (index > (cnt - _sortingChildCount - 1))
            index = cnt - _sortingChildCount - 1;
    }

    moveChild(child, oldIndex, index);
}

int GComponent::setChildIndexBefore(GObject* child, int index)
{
    CCASSERT(child != nullptr, "Argument must be non-nil");

    int oldIndex = _children.getIndex(child);
    CCASSERT(oldIndex != -1, "Not a child of this container");

    if (child->_sortingOrder != 0) //no effect
        return oldIndex;

    int cnt = _children.size();
    if (_sortingChildCount > 0)
    {
        if (index > (cnt - _sortingChildCount - 1))
            index = cnt - _sortingChildCount - 1;
    }

    if (oldIndex < index)
        return moveChild(child, oldIndex, index - 1);
    else
        return moveChild(child, oldIndex, index);
}

int GComponent::moveChild(GObject* child, int oldIndex, int index)
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

    if (child->_displayObject->getParent() != nullptr)
    {
        if (_childrenRenderOrder == ChildrenRenderOrder::ASCENT)
        {
            int fromIndex = min(index, oldIndex);
            int toIndex = min(max(index, oldIndex), cnt - 1);
            for (int i = fromIndex; i <= toIndex; i++)
            {
                GObject* g = _children.at(i);
                if (g->_displayObject->getParent() != nullptr)
                    g->_displayObject->setLocalZOrder(i);
            }
        }
        else if (_childrenRenderOrder == ChildrenRenderOrder::DESCENT)
        {
            int fromIndex = min(index, oldIndex);
            int toIndex = min(max(index, oldIndex), cnt - 1);
            for (int i = fromIndex; i <= toIndex; i++)
            {
                GObject* g = _children.at(i);
                if (g->_displayObject->getParent() != nullptr)
                    g->_displayObject->setLocalZOrder(cnt - 1 - i);
            }
        }
        else
            CALL_LATER(GComponent, buildNativeDisplayList);

        setBoundsChangedFlag();
    }

    return index;
}

void GComponent::swapChildren(GObject* child1, GObject* child2)
{
    CCASSERT(child1 != nullptr, "Argument1 must be non-nil");
    CCASSERT(child2 != nullptr, "Argument2 must be non-nil");

    int index1 = _children.getIndex(child1);
    int index2 = _children.getIndex(child2);

    CCASSERT(index1 != -1, "Not a child of this container");
    CCASSERT(index2 != -1, "Not a child of this container");

    swapChildrenAt(index1, index2);
}

void GComponent::swapChildrenAt(int index1, int index2)
{
    GObject* child1 = _children.at(index1);
    GObject* child2 = _children.at(index2);

    setChildIndex(child1, index2);
    setChildIndex(child2, index1);
}

int GComponent::numChildren() const
{
    return _children.size();
}

bool GComponent::isAncestorOf(const GObject * obj) const
{
    if (obj == nullptr)
        return false;

    GComponent* p = obj->_parent;
    while (p != nullptr)
    {
        if (p == this)
            return true;

        p = p->_parent;
    }
    return false;
}

Controller* GComponent::getController(const std::string& name) const
{
    for (const auto& c : _controllers)
    {
        if (c->name.compare(name) == 0)
            return c;
    }

    return nullptr;
}

void GComponent::addController(Controller* c)
{
    CCASSERT(c != nullptr, "Argument must be non-nil");

    _controllers.pushBack(c);
}

void GComponent::removeController(Controller* c)
{
    CCASSERT(c != nullptr, "Argument must be non-nil");

    int index = _controllers.getIndex(c);
    CCASSERT(index != -1, "controller not exists");

    c->setParent(nullptr);
    applyController(c);
    _controllers.erase(index);
}

void GComponent::applyController(Controller * c)
{
    _applyingController = c;

    for (const auto& child : _children)
        child->handleControllerChanged(c);

    _applyingController = nullptr;
}

void GComponent::applyAllControllers()
{
    for (const auto& c : _controllers)
        applyController(c);
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

void GComponent::setOpaque(bool value)
{
    _opaque = value;
}

void GComponent::setMargin(const Margin & value)
{
    _margin = value;
}

void GComponent::setChildrenRenderOrder(ChildrenRenderOrder value)
{
    if (_childrenRenderOrder != value)
    {
        _childrenRenderOrder = value;
        CALL_LATER(GComponent, buildNativeDisplayList);
    }
}

void GComponent::setApexIndex(int value)
{
    if (_apexIndex != value)
    {
        _apexIndex = value;

        if (_childrenRenderOrder == ChildrenRenderOrder::ARCH)
            CALL_LATER(GComponent, buildNativeDisplayList);
    }
}

cocos2d::Node* GComponent::getMask() const
{
    return ((FUIContainer*)_displayObject)->getStencil();
}

void GComponent::setMask(cocos2d::Node * value, bool inverted)
{
    if (_maskOwner)
    {
        _maskOwner->_directToParent = false;
        childStateChanged(_maskOwner);
        _maskOwner->handlePositionChanged();
        _maskOwner->release();
        _maskOwner = nullptr;
    }

    if (value)
    {
        for (auto &child : _children)
        {
            if (child->_displayObject == value)
            {
                _maskOwner = child;
                if (value->getParent())
                    value->getParent()->removeChild(value, false);
                _maskOwner->_directToParent = true;
                _maskOwner->handlePositionChanged();
                _maskOwner->retain();
                break;
            }
        }
    }

    ((FUIContainer*)_displayObject)->setStencil(value);
    if (value)
    {
        ((FUIContainer*)_displayObject)->setAlphaThreshold(0.05f);
        ((FUIContainer*)_displayObject)->setInverted(inverted);
    }
}

float GComponent::getViewWidth() const
{
    if (_scrollPane != nullptr)
        return _scrollPane->getViewSize().width;
    else
        return _size.width - _margin.left - _margin.right;
}

void GComponent::setViewWidth(float value)
{
    if (_scrollPane != nullptr)
        _scrollPane->setViewWidth(value);
    else
        setWidth(value + _margin.left + _margin.right);
}

float GComponent::getViewHeight() const
{
    if (_scrollPane != nullptr)
        return _scrollPane->getViewSize().height;
    else
        return _size.height - _margin.top - _margin.bottom;
}

void GComponent::setViewHeight(float value)
{
    if (_scrollPane != nullptr)
        _scrollPane->setViewHeight(value);
    else
        setWidth(value + _margin.top + _margin.bottom);
}

void GComponent::setBoundsChangedFlag()
{
    if (_scrollPane == nullptr && !_trackBounds)
        return;

    _boundsChanged = true;
    CALL_LATER(GComponent, doUpdateBounds);
}

void GComponent::ensureBoundsCorrect()
{
    if (_boundsChanged)
        updateBounds();
}

void GComponent::updateBounds()
{
    float ax, ay, aw, ah;
    if (!_children.empty())
    {
        ax = FLT_MAX;
        ay = FLT_MAX;
        float ar = -FLT_MAX, ab = -FLT_MAX;
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
            tmp = child->getX() + child->getWidth();
            if (tmp > ar)
                ar = tmp;
            tmp = child->getY() + child->getHeight();
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
    if (_scrollPane != nullptr)
        _scrollPane->setContentSize(ceil(ax + aw), ceil(ay + ah));
}

void GComponent::doUpdateBounds()
{
    if (_boundsChanged)
        updateBounds();
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
            if (g->_group == child)
                childStateChanged(g);
        }
    }

    if (child->_displayObject == nullptr || child == _maskOwner)
        return;

    if (child->finalVisible())
    {
        if (child->_displayObject->getParent() == nullptr)
        {
            if (_childrenRenderOrder == ChildrenRenderOrder::ASCENT)
            {
                int index = _children.getIndex(child);
                _container->addChild(child->_displayObject, index);
                int cnt = _children.size();
                for (int i = index + 1; i < cnt; i++)
                {
                    child = _children.at(i);
                    if (child->_displayObject->getParent() != nullptr)
                        child->_displayObject->setLocalZOrder(i);
                }
            }
            else if (_childrenRenderOrder == ChildrenRenderOrder::DESCENT)
            {
                int index = cnt - 1 - _children.getIndex(child);
                _container->addChild(child->_displayObject, index);
                for (int i = index - 1; i >= 0; i--)
                {
                    child = _children.at(i);
                    if (child->_displayObject->getParent() != nullptr)
                        child->_displayObject->setLocalZOrder(cnt - i - 1);
                }
            }
            else
            {
                CALL_LATER(GComponent, buildNativeDisplayList);
            }
        }
    }
    else
    {
        if (child->_displayObject->getParent() != nullptr)
        {
            _container->removeChild(child->_displayObject, false);
            if (_childrenRenderOrder == ChildrenRenderOrder::ARCH)
            {
                CALL_LATER(GComponent, buildNativeDisplayList);
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
            moveChild(child, oldIndex, index - 1);
        else
            moveChild(child, oldIndex, index);
    }
}

void GComponent::buildNativeDisplayList()
{
    int cnt = _children.size();
    if (cnt == 0)
        return;

    switch (_childrenRenderOrder)
    {
    case ChildrenRenderOrder::ASCENT:
    {
        for (int i = 0; i < cnt; i++)
        {
            GObject* child = _children.at(i);
            if (child->_displayObject != nullptr && child != _maskOwner && child->finalVisible())
                _container->addChild(child->_displayObject, i);
        }
    }
    break;
    case ChildrenRenderOrder::DESCENT:
    {
        for (int i = 0; i < cnt; i++)
        {
            GObject* child = _children.at(i);
            if (child->_displayObject != nullptr && child != _maskOwner && child->finalVisible())
                _container->addChild(child->_displayObject, cnt - 1 - i);
        }
    }
    break;

    case ChildrenRenderOrder::ARCH:
    {
        for (int i = 0; i < _apexIndex; i++)
        {
            GObject* child = _children.at(i);
            if (child->_displayObject != nullptr && child != _maskOwner && child->finalVisible())
                _container->addChild(child->_displayObject, i);
        }
        for (int i = cnt - 1; i >= _apexIndex; i--)
        {
            GObject* child = _children.at(i);
            if (child->_displayObject != nullptr && child != _maskOwner && child->finalVisible())
                _container->addChild(child->_displayObject, _apexIndex + cnt - 1 - i);
        }
    }
    break;
    }
}

cocos2d::Vec2 GComponent::getSnappingPosition(const cocos2d::Vec2 & pt)
{
    int cnt = _children.size();
    if (cnt == 0)
        return pt;

    ensureBoundsCorrect();

    GObject* obj = nullptr;

    Vec2 ret = pt;

    int i = 0;
    if (ret.y != 0)
    {
        for (; i < cnt; i++)
        {
            obj = _children.at(i);
            if (ret.y < obj->getY())
            {
                if (i == 0)
                {
                    ret.y = 0;
                    break;
                }
                else
                {
                    GObject* prev = _children.at(i - 1);
                    if (ret.y < prev->getY() + prev->getHeight() / 2) //top half part
                        ret.y = prev->getY();
                    else //bottom half part
                        ret.y = obj->getY();
                    break;
                }
            }
        }

        if (i == cnt)
            ret.y = obj->getY();
    }

    if (ret.x != 0)
    {
        if (i > 0)
            i--;
        for (; i < cnt; i++)
        {
            obj = _children.at(i);
            if (ret.x < obj->getX())
            {
                if (i == 0)
                {
                    ret.x = 0;
                    break;
                }
                else
                {
                    GObject* prev = _children.at(i - 1);
                    if (ret.x < prev->getX() + prev->getWidth() / 2) // top half part
                        ret.x = prev->getX();
                    else//bottom half part
                        ret.x = obj->getX();
                    break;
                }
            }
        }
        if (i == cnt)
            ret.x = obj->getX();
    }

    return ret;
}

GObject* GComponent::hitTest(const Vec2 &pt, const Camera* camera)
{
    if (!_touchable || !finalVisible())
        return nullptr;

    GObject * target = nullptr;
    if (_maskOwner)
    {
        if (_maskOwner->hitTest(pt, camera) != nullptr)
        {
            if (((FUIContainer*)_displayObject)->isInverted())
                return nullptr;
        }
        else
        {
            if (!((FUIContainer*)_displayObject)->isInverted())
                return nullptr;
        }
    }

    for (auto it = _children.rbegin(); it != _children.rend(); ++it)
    {
        if (!(*it)->_displayObject || *it == _maskOwner)
            continue;

        target = (*it)->hitTest(pt, camera);
        if (target)
            return target;
    }

    if (_scrollPane)
    {
        target = _scrollPane->hitTest(pt, camera);
        if (target)
            return target;
    }

    if (_opaque)
    {
        Rect rect;
        rect.size = _displayObject->getContentSize();
        if (isScreenPointInRect(pt, camera, _displayObject->getWorldToNodeTransform(), rect, nullptr))
            return this;
    }

    return nullptr;
}

void GComponent::setupOverflow(OverflowType overflow)
{
    if (overflow == OverflowType::HIDDEN)
    {
        ((FUIContainer*)_displayObject)->setClippingEnabled(true);
        ((FUIContainer*)_displayObject)->setClippingRegion(
            Rect(_margin.left, _margin.top,
                _size.width - _margin.left - _margin.right,
                _size.height - _margin.top - _margin.bottom));
    }

    _container->setPosition(_margin.left, _size.height - _margin.top);
}

void GComponent::setupScroll(const Margin& scrollBarMargin,
    ScrollType scroll, ScrollBarDisplayType scrollBarDisplay, int flags,
    const std::string& vtScrollBarRes, const std::string& hzScrollBarRes,
    const std::string& headerRes, const std::string& footerRes)
{
    _scrollPane = new ScrollPane(this, scroll, scrollBarMargin, scrollBarDisplay, flags, vtScrollBarRes, hzScrollBarRes, headerRes, footerRes);
    _scrollPane->retain();
}

void GComponent::handleSizeChanged()
{
    GObject::handleSizeChanged();

    if (_scrollPane != nullptr)
        _scrollPane->onOwnerSizeChanged();
    else
        _container->setPosition(_margin.left, _size.height - _margin.top);

    if (_maskOwner)
        _maskOwner->handlePositionChanged();

    if (((FUIContainer*)_displayObject)->isClippingEnabled())
        ((FUIContainer*)_displayObject)->setClippingRegion(
            Rect(_margin.left, _margin.top,
                _size.width - _margin.left - _margin.right,
                _size.height - _margin.top - _margin.bottom));
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
        _scrollPane->handleControllerChanged(c);
}

void GComponent::constructFromResource()
{
    constructFromResource(nullptr, 0);
}

void GComponent::constructFromResource(std::vector<GObject*>* objectPool, int poolIndex)
{
    tinyxml2::XMLElement* xml = _packageItem->componentData->RootElement();

    _underConstruct = true;

    Vec2 v2;
    Vec4 v4;
    const char *p;

    p = xml->Attribute("size");
    ToolSet::splitString(p, ',', v2, true);
    initSize = sourceSize = v2;

    setSize(sourceSize.width, sourceSize.height);

    p = xml->Attribute("restrictSize");
    if (p)
    {
        ToolSet::splitString(p, ',', v4, true);
        minSize.width = v4.x;
        minSize.height = v4.z;
        maxSize.width = v4.y;
        maxSize.height = v4.w;
    }

    p = xml->Attribute("pivot");
    if (p)
    {
        ToolSet::splitString(p, ',', v2);
        setPivot(v2.x, v2.y, xml->BoolAttribute("anchor"));
    }

    p = xml->Attribute("opaque");
    if (p)
        _opaque = strcmp(p, "true") == 0;
    else
        _opaque = true;

    OverflowType overflow;
    p = xml->Attribute("overflow");
    if (p)
        overflow = ToolSet::parseOverflowType(p);
    else
        overflow = OverflowType::VISIBLE;

    p = xml->Attribute("margin");
    if (p)
    {
        ToolSet::splitString(p, ',', v4);
        _margin.setMargin(v4.z, v4.x, v4.w, v4.y);
    }

    if (overflow == OverflowType::SCROLL)
    {
        ScrollType scroll;
        p = xml->Attribute("scroll");
        if (p)
            scroll = ToolSet::parseScrollType(p);
        else
            scroll = ScrollType::VERTICAL;

        ScrollBarDisplayType scrollBarDisplay;
        p = xml->Attribute("scrollBar");
        if (p)
            scrollBarDisplay = ToolSet::parseScrollBarDisplayType(p);
        else
            scrollBarDisplay = ScrollBarDisplayType::DEFAULT;

        int scrollBarFlags = xml->IntAttribute("scrollBarFlags");

        Margin scrollBarMargin;
        p = xml->Attribute("scrollBarMargin");
        if (p)
        {
            ToolSet::splitString(p, ',', v4);
            scrollBarMargin.setMargin(v4.z, v4.x, v4.w, v4.y);
        }

        string  vtScrollBarRes;
        string hzScrollBarRes;
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

    vector<DisplayListItem*>& displayList = *_packageItem->displayList;
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
            child->constructFromResource();
        }
        else
            child = UIObjectFactory::newObject(di.type);

        child->_underConstruct = true;
        child->setup_BeforeAdd(di.desc);
        child->_parent = this;
        child->retain();
        _children.pushBack(child);
    }

    _relations->setup(xml);

    for (int i = 0; i < childCount; i++)
        _children.at(i)->_relations->setup(displayList[i]->desc);

    for (int i = 0; i < childCount; i++)
    {
        child = _children.at(i);
        child->setup_AfterAdd(displayList[i]->desc);
        child->_underConstruct = false;
    }

    p = xml->Attribute("mask");
    if (p)
    {
        bool inverted = xml->BoolAttribute("reversedMask");
        setMask(getChildById(p)->displayObject(), inverted);
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

    const char *p;

    if (_scrollPane != nullptr && _scrollPane->isPageMode())
    {
        p = xml->Attribute("pageController");
        if (p)
            _scrollPane->setPageController(_parent->getController(p));
    }

    p = xml->Attribute("controller");
    if (p)
    {
        std::vector<std::string> arr;
        ToolSet::splitString(p, ',', arr);
        int cnt = arr.size();
        for (int i = 0; i < cnt; i += 2)
        {
            Controller* cc = getController(arr[i]);
            if (cc != nullptr)
                cc->setSelectedPageId(arr[i + 1]);
        }
    }
}

NS_FGUI_END

