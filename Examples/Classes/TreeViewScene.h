#ifndef __TREEVIEW_SCENE_H__
#define __TREEVIEW_SCENE_H__

#include "cocos2d.h"
#include "DemoScene.h"

USING_NS_FGUI;

class TreeViewScene : public DemoScene
{
public:
    TreeViewScene();
    ~TreeViewScene();

    // implement the "static create()" method manually
    CREATE_FUNC(TreeViewScene);

protected:
    virtual void continueInit() override;

private:
    void onClickNode(EventContext* context);
    void renderTreeNode(TreeNode* node);

    GComponent* _view;
    TreeView* _treeView;
};

#endif
