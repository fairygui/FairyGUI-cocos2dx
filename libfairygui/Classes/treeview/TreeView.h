#ifndef __TREEVIEW_H__
#define __TREEVIEW_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "event/UIEventDispatcher.h"
#include "TreeNode.h"

NS_FGUI_BEGIN

class GList;
class GComponent;

class TreeView : public UIEventDispatcher
{
public:
    typedef std::function<GComponent*(TreeNode* node)> TreeNodeCreateCellFunction;
    typedef std::function<void(TreeNode* node)> TreeNodeRenderFunction;
    typedef std::function<void(TreeNode* node, bool expand)> TreeNodeWillExpandFunction;

    static TreeView* create(GList* list);

    TreeView();
    virtual ~TreeView();

    GList* getList() const { return _list; }
    TreeNode* getRootNode() const { return _rootNode; }
    TreeNode* getSelectedNode() const;
    void getSelection(std::vector<TreeNode*>& result) const;
    void addSelection(TreeNode* node, bool scrollItToView = false);
    void removeSelection(TreeNode* node);
    void clearSelection();
    int getNodeIndex(TreeNode* node) const;
    void updateNode(TreeNode* node);
    void expandAll(TreeNode* folderNode);
    void collapseAll(TreeNode* folderNode);

    TreeNodeCreateCellFunction treeNodeCreateCell;
    TreeNodeRenderFunction treeNodeRender;
    TreeNodeWillExpandFunction treeNodeWillExpand;

private:
    bool init(GList* list);
    void createCell(TreeNode* node);
    void afterInserted(TreeNode* node);
    int getInsertIndexForNode(TreeNode* node);
    void afterRemoved(TreeNode* node);
    void afterExpanded(TreeNode* node);
    void afterCollapsed(TreeNode* node);
    void afterMoved(TreeNode* node);
    int checkChildren(TreeNode* folderNode, int index);
    void hideFolderNode(TreeNode* folderNode);
    void removeNode(TreeNode* node);

    void onClickItem(EventContext* context);
    void onClickExpandButton(EventContext* context);

    GList* _list;
    int _indent;
    TreeNode* _rootNode;

    friend class TreeNode;
};

NS_FGUI_END

#endif
