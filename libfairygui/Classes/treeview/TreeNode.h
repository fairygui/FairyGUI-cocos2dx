#ifndef __TREENODE_H__
#define __TREENODE_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class TreeView;
class GComponent;

class TreeNode : public cocos2d::Ref
{
public:
    static TreeNode* create(bool isFolder = false);

    TreeNode();
    virtual ~TreeNode();

    TreeNode* getParent() const { return _parent; }
    TreeView* getRoot() const { return _root; }
    GComponent* getCell() const { return _cell; }
    const cocos2d::Value& getData() const { return _data; }
    void setData(const cocos2d::Value& value) { _data = value; }
    bool isExpanded() const { return _expanded; }
    void setExpaned(bool value);
    bool isFolder() const { return _isFolder;}
    const std::string& getText() const;

    TreeNode* addChild(TreeNode* child);
    TreeNode* addChildAt(TreeNode* child, int index);

    void removeChild(TreeNode * child);
    void removeChildAt(int index);
    void removeChildren() { removeChildren(0, -1); }
    void removeChildren(int beginIndex, int endIndex);

    TreeNode* getChildAt(int index) const;
    TreeNode* getPrevSibling() const;
    TreeNode* getNextSibling() const;

    int getChildIndex(const TreeNode* child) const;
    void setChildIndex(TreeNode* child, int index);
    int setChildIndexBefore(TreeNode* child, int index);
    void swapChildren(TreeNode* child1, TreeNode* child2);
    void swapChildrenAt(int index1, int index2);

    int numChildren() const;

private:
    bool init(bool isFolder);
    int moveChild(TreeNode* child, int oldIndex, int index);
    void setRoot(TreeView* value);
    void setCell(GComponent* value);

    TreeView* _root;
    TreeNode* _parent;
    GComponent* _cell;
    int _level;
    bool _expanded;
    bool _isFolder;
    bool _isRootNode;
    cocos2d::Value _data;
    cocos2d::Vector<TreeNode*> _children;

    friend class TreeView;
};

NS_FGUI_END

#endif
