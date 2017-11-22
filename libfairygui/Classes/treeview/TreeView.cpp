#include "TreeView.h"
#include "GList.h"
#include "GButton.h"

NS_FGUI_BEGIN
USING_NS_CC;

TreeView * TreeView::create(GList * list)
{
    TreeView* pRet = new (std::nothrow) TreeView();

    if (pRet != nullptr && pRet->init(list))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

TreeView::TreeView() :
    _list(nullptr),
    _rootNode(nullptr),
    _indent(30)
{
}

TreeView::~TreeView()
{
    CC_SAFE_RELEASE(_rootNode);
    CC_SAFE_RELEASE(_list);
}

bool TreeView::init(GList * list)
{
    _list = list;
    _list->retain();
    _list->addEventListener(UIEventType::ClickItem, CC_CALLBACK_1(TreeView::onClickItem, this));
    _list->addEventListener(UIEventType::RightClickItem, CC_CALLBACK_1(TreeView::onClickItem, this));
    _list->removeChildrenToPool();

    _rootNode = TreeNode::create(true);
    _rootNode->_isRootNode = true;
    _rootNode->setRoot(this);
    _rootNode->setExpaned(true);
    _rootNode->retain();

    return true;
}

TreeNode * TreeView::getSelectedNode() const
{
    if (_list->getSelectedIndex() != -1)
        return (TreeNode*)_list->getChildAt(_list->getSelectedIndex())->getData();
    else
        return nullptr;
}

void TreeView::getSelection(std::vector<TreeNode*>& result) const
{
    std::vector<int> ids;
    _list->getSelection(ids);
    for (auto &it : ids)
    {
        TreeNode* node = (TreeNode*)_list->getChildAt(it)->getData();
        result.push_back(node);
    }
}

void TreeView::addSelection(TreeNode * node, bool scrollItToView)
{
    TreeNode* parentNode = node->_parent;
    while (parentNode != nullptr && parentNode != _rootNode)
    {
        parentNode->setExpaned(true);
        parentNode = parentNode->_parent;
    }
    if (node->_cell != nullptr)
        _list->addSelection(_list->getChildIndex(node->_cell), scrollItToView);
}

void TreeView::removeSelection(TreeNode * node)
{
    if (node->_cell != nullptr)
        _list->removeSelection(_list->getChildIndex(node->_cell));
}

void TreeView::clearSelection()
{
    _list->clearSelection();
}

int TreeView::getNodeIndex(TreeNode * node) const
{
    if (node->_cell == nullptr)
        return -1;
    else
        return _list->getChildIndex(node->_cell);
}

void TreeView::updateNode(TreeNode* node)
{
    if (node->_cell == nullptr)
        return;

    if (treeNodeRender != nullptr)
        treeNodeRender(node);
}

void TreeView::expandAll(TreeNode * folderNode)
{
    folderNode->setExpaned(true);
    int cnt = folderNode->numChildren();
    for (auto &it : folderNode->_children)
    {
        if (it->isFolder())
            expandAll(it);
    }
}

void TreeView::collapseAll(TreeNode * folderNode)
{
    if (folderNode != _rootNode)
        folderNode->setExpaned(false);
    int cnt = folderNode->numChildren();
    for (auto &it : folderNode->_children)
    {
        if (it->isFolder())
            collapseAll(it);
    }
}

void TreeView::createCell(TreeNode * node)
{
    GComponent* obj;
    if (treeNodeCreateCell != nullptr)
        obj = treeNodeCreateCell(node);
    else
        obj = _list->getItemPool()->getObject(_list->getDefaultItem())->as<GComponent>();
    CCASSERT(obj, "Unable to create tree cell");
    node->setCell(obj);

    GObject* indentObj = node->_cell->getChild("indent");
    if (indentObj != nullptr)
        indentObj->setWidth((node->_level - 1) * _indent);

    GButton* expandButton = node->_cell->getChild("expandButton")->as<GButton>();
    if (expandButton != nullptr)
    {
        if (node->isFolder())
        {
            expandButton->setVisible(true);
            expandButton->addClickListener(CC_CALLBACK_1(TreeView::onClickExpandButton, this));
            expandButton->setData(node);
            expandButton->setSelected(node->isExpanded());
        }
        else
            expandButton->setVisible(false);
    }

    if (treeNodeRender != nullptr)
        treeNodeRender(node);
}

void TreeView::afterInserted(TreeNode * node)
{
    createCell(node);

    int index = getInsertIndexForNode(node);
    _list->addChildAt(node->_cell, index);
    if (treeNodeRender != nullptr)
        treeNodeRender(node);

    if (node->isFolder() && node->isExpanded())
        checkChildren(node, index);
}

int TreeView::getInsertIndexForNode(TreeNode * node)
{
    TreeNode* prevNode = node->getPrevSibling();
    if (prevNode == nullptr)
        prevNode = node->getParent();
    int insertIndex;
    if (prevNode->_cell)
        insertIndex = _list->getChildIndex(prevNode->_cell) + 1;
    else
        insertIndex = 0;
    int myLevel = node->_level;
    int cnt = _list->numChildren();
    for (int i = insertIndex; i < cnt; i++)
    {
        TreeNode* testNode = (TreeNode*)_list->getChildAt(i)->getData();
        if (testNode->_level <= myLevel)
            break;

        insertIndex++;
    }

    return insertIndex;
}

void TreeView::afterRemoved(TreeNode * node)
{
    removeNode(node);
}

void TreeView::afterExpanded(TreeNode * node)
{
    if (node != _rootNode && treeNodeWillExpand != nullptr)
        treeNodeWillExpand(node, true);

    if (node != _rootNode)
    {
        if (node->_cell == nullptr)
            return;

        if (treeNodeRender != nullptr)
            treeNodeRender(node);

        GButton* expandButton = node->_cell->getChild("expandButton")->as<GButton>();
        if (expandButton != nullptr)
            expandButton->setSelected(true);

        if (node->_cell->getParent() != nullptr)
            checkChildren(node, _list->getChildIndex(node->_cell));
    }
    else
        checkChildren(node, -1);
}

void TreeView::afterCollapsed(TreeNode * node)
{
    if (node != _rootNode && treeNodeWillExpand != nullptr)
        treeNodeWillExpand(node, false);

    if (node != _rootNode)
    {
        if (node->_cell == nullptr)
            return;

        if (treeNodeRender != nullptr)
            treeNodeRender(node);

        GButton* expandButton = node->_cell->getChild("expandButton")->as<GButton>();
        if (expandButton != nullptr)
            expandButton->setSelected(false);

        if (node->_cell->getParent() != nullptr)
            hideFolderNode(node);
    }
    else
        hideFolderNode(node);
}

void TreeView::afterMoved(TreeNode * node)
{
    if (!node->isFolder())
        _list->removeChild(node->_cell);
    else
        hideFolderNode(node);

    int index = getInsertIndexForNode(node);
    _list->addChildAt(node->_cell, index);

    if (node->isFolder() && node->isExpanded())
        checkChildren(node, index);
}

int TreeView::checkChildren(TreeNode * folderNode, int index)
{
    int cnt = folderNode->numChildren();
    for (int i = 0; i < cnt; i++)
    {
        index++;
        TreeNode* node = folderNode->getChildAt(i);
        if (node->_cell == nullptr)
            createCell(node);

        if (node->_cell->getParent() == nullptr)
            _list->addChildAt(node->_cell, index);

        if (node->isFolder() && node->isExpanded())
            index = checkChildren(node, index);
    }

    return index;
}

void TreeView::hideFolderNode(TreeNode * folderNode)
{
    int cnt = folderNode->numChildren();
    for (int i = 0; i < cnt; i++)
    {
        TreeNode* node = folderNode->getChildAt(i);
        if (node->_cell != nullptr)
        {
            if (node->_cell->getParent() != nullptr)
                _list->removeChild(node->_cell);
            _list->getItemPool()->returnObject(node->_cell);
            node->_cell->setData(nullptr);
            node->_cell = nullptr;
        }
        if (node->isFolder() && node->isExpanded())
            hideFolderNode(node);
    }
}

void TreeView::removeNode(TreeNode * node)
{
    if (node->_cell != nullptr)
    {
        if (node->_cell->getParent() != nullptr)
            _list->removeChild(node->_cell);
        _list->getItemPool()->returnObject(node->_cell);
        node->_cell->setData(nullptr);
        node->_cell = nullptr;
    }

    if (node->isFolder())
    {
        int cnt = node->numChildren();
        for (int i = 0; i < cnt; i++)
        {
            TreeNode* node2 = node->getChildAt(i);
            removeNode(node2);
        }
    }
}

void TreeView::onClickExpandButton(EventContext * context)
{
    context->stopPropagation();

    GButton* expandButton = (GButton*)context->getSender();
    TreeNode* node = (TreeNode*)expandButton->getParent()->getData();
    if (_list->getScrollPane() != nullptr)
    {
        float posY = _list->getScrollPane()->getPosY();
        if (expandButton->isSelected())
            node->setExpaned(true);
        else
            node->setExpaned(false);
        _list->getScrollPane()->setPosY(posY);
        _list->getScrollPane()->scrollToView(node->_cell);
    }
    else
    {
        if (expandButton->isSelected())
            node->setExpaned(true);
        else
            node->setExpaned(false);
    }
}

void TreeView::onClickItem(EventContext * context)
{
    float posY = 0;
    if (_list->getScrollPane() != nullptr)
        posY = _list->getScrollPane()->getPosY();

    GObject* item = (GObject*)context->getData();
    TreeNode* node = (TreeNode*)item->getData();

    dispatchEvent(context->getType(), node);

    if (_list->getScrollPane() != nullptr)
    {
        _list->getScrollPane()->setPosY(posY);
        _list->getScrollPane()->scrollToView(node->_cell);
    }
}

NS_FGUI_END
