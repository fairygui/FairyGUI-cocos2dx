#include "TreeNode.h"
#include "TreeView.h"
#include "GComponent.h"

NS_FGUI_BEGIN
USING_NS_CC;

TreeNode * TreeNode::create(bool isFolder)
{
    TreeNode* pRet = new (std::nothrow) TreeNode();

    if (pRet != nullptr && pRet->init(isFolder))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

TreeNode::TreeNode() :
    _root(nullptr),
    _parent(nullptr),
    _cell(nullptr),
    _level(0),
    _expanded(false),
    _isFolder(false),
    _isRootNode(false)
{
}

TreeNode::~TreeNode()
{
    for (auto &it : _children)
        it->_parent = nullptr;

    if (_parent)
        _parent->removeChild(this);

    CC_SAFE_RELEASE(_cell);
}

bool TreeNode::init(bool isFolder)
{
    _isFolder = isFolder;

    return true;
}

void TreeNode::setExpaned(bool value)
{
    if (!_isFolder)
        return;

    if (_expanded != value)
    {
        _expanded = value;
        if (_root != nullptr)
        {
            if (_expanded)
                _root->afterExpanded(this);
            else
                _root->afterCollapsed(this);
        }
    }
}

const std::string & TreeNode::getText() const
{
    if (_cell)
        return _cell->getText();
    else
        return STD_STRING_EMPTY;
}


TreeNode* TreeNode::addChild(TreeNode* child)
{
    addChildAt(child, (int)_children.size());
    return child;
}

TreeNode* TreeNode::addChildAt(TreeNode* child, int index)
{
    CCASSERT(child != nullptr, "Argument must be non-nil");

    if (child->_parent == this)
    {
        setChildIndex(child, index);
    }
    else
    {
        child->retain();
        if (child->_parent != nullptr)
            child->_parent->removeChild(child);
        child->_parent = this;

        int cnt = (int)_children.size();
        if (index == cnt)
            _children.pushBack(child);
        else
            _children.insert(index, child);
        child->release();

        child->_level = _level + 1;
        child->setRoot(_root);
        if (_isRootNode || (_cell != nullptr && _cell->getParent() != nullptr && _expanded))
            _root->afterInserted(child);
    }
    return child;
}

void TreeNode::removeChild(TreeNode* child)
{
    CCASSERT(child != nullptr, "Argument must be non-nil");

    int childIndex = (int)_children.getIndex(child);
    if (childIndex != -1)
        removeChildAt(childIndex);
}

void TreeNode::removeChildAt(int index)
{
    TreeNode* child = _children.at(index);
    child->_parent = nullptr;
    _children.erase(index);

    if (_root != nullptr)
    {
        child->setRoot(nullptr);
        _root->afterRemoved(child);
    }
}

void TreeNode::removeChildren(int beginIndex, int endIndex)
{
    if (endIndex < 0 || endIndex >= _children.size())
        endIndex = (int)_children.size() - 1;

    for (int i = beginIndex; i <= endIndex; ++i)
        removeChildAt(beginIndex);
}

TreeNode* TreeNode::getChildAt(int index) const
{
    return _children.at(index);
}

TreeNode * TreeNode::getPrevSibling() const
{
    if (_parent == nullptr)
        return nullptr;

    ssize_t i = _parent->_children.getIndex((TreeNode*)this);
    if (i <= 0)
        return nullptr;

    return _parent->_children.at(i - 1);
}

TreeNode * TreeNode::getNextSibling() const
{
    if (_parent == nullptr)
        return nullptr;

    ssize_t i = _parent->_children.getIndex((TreeNode*)this);
    if (i < 0 || i >= _parent->_children.size() - 1)
        return nullptr;

    return _parent->_children.at(i + 1);
}

int TreeNode::getChildIndex(const TreeNode* child) const
{
    CCASSERT(child != nullptr, "Argument must be non-nil");

    return (int)_children.getIndex((TreeNode*)child);
}

void TreeNode::setChildIndex(TreeNode* child, int index)
{
    CCASSERT(child != nullptr, "Argument must be non-nil");

    int oldIndex = (int)_children.getIndex(child);
    CCASSERT(oldIndex != -1, "Not a child of this container");

    moveChild(child, oldIndex, index);
}

int TreeNode::setChildIndexBefore(TreeNode* child, int index)
{
    CCASSERT(child != nullptr, "Argument must be non-nil");

    int oldIndex = (int)_children.getIndex(child);
    CCASSERT(oldIndex != -1, "Not a child of this container");

    if (oldIndex < index)
        return moveChild(child, oldIndex, index - 1);
    else
        return moveChild(child, oldIndex, index);
}

int TreeNode::moveChild(TreeNode* child, int oldIndex, int index)
{
    int cnt = (int)_children.size();
    if (index > cnt)
        index = cnt;

    if (oldIndex == index)
        return oldIndex;

    child->retain();
    _children.erase(oldIndex);
    if (index >= cnt)
        _children.pushBack(child);
    else
        _children.insert(index, child);
    child->release();
    if (_cell != nullptr && _cell->getParent() != nullptr && _expanded)
        _root->afterMoved(child);

    return index;
}

void TreeNode::swapChildren(TreeNode* child1, TreeNode* child2)
{
    CCASSERT(child1 != nullptr, "Argument1 must be non-nil");
    CCASSERT(child2 != nullptr, "Argument2 must be non-nil");

    int index1 = (int)_children.getIndex(child1);
    int index2 = (int)_children.getIndex(child2);

    CCASSERT(index1 != -1, "Not a child of this container");
    CCASSERT(index2 != -1, "Not a child of this container");

    swapChildrenAt(index1, index2);
}

void TreeNode::swapChildrenAt(int index1, int index2)
{
    TreeNode* child1 = _children.at(index1);
    TreeNode* child2 = _children.at(index2);

    setChildIndex(child1, index2);
    setChildIndex(child2, index1);
}

int TreeNode::numChildren() const
{
    return (int)_children.size();
}

void TreeNode::setRoot(TreeView* value)
{
    _root = value;
    if (_root != nullptr && _root->treeNodeWillExpand != nullptr && _expanded)
        _root->treeNodeWillExpand(this, true);

    if (_isFolder)
    {
        for (auto &child : _children)
        {
            child->_level = _level + 1;
            child->setRoot(value);
        }
    }
}

void TreeNode::setCell(GComponent * value)
{
    if (_cell != value)
    {
        CC_SAFE_RELEASE(_cell);
        _cell = value;
        CC_SAFE_RETAIN(_cell);
        if (_cell)
            _cell->setData(this);
    }
}

NS_FGUI_END
