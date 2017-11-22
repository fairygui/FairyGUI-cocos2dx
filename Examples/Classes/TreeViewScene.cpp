#include "TreeViewScene.h"

USING_NS_CC;

TreeViewScene::TreeViewScene() :_treeView(nullptr)
{

}

TreeViewScene::~TreeViewScene()
{
    CC_SAFE_RELEASE(_treeView);
}

void TreeViewScene::continueInit()
{
    UIPackage::addPackage("UI/TreeView");

    _view = UIPackage::createObject("TreeView", "Main")->as<GComponent>();
    _groot->addChild(_view);

    _treeView = TreeView::create(_view->getChild("tree")->as<GList>());
    _treeView->retain();

    _treeView->addEventListener(UIEventType::ClickItem, CC_CALLBACK_1(TreeViewScene::onClickNode, this));
    _treeView->treeNodeRender = CC_CALLBACK_1(TreeViewScene::renderTreeNode, this);

    TreeNode* topNode = TreeNode::create(true);
    topNode->setData(Value("I'm a top node"));
    _treeView->getRootNode()->addChild(topNode);
    for (int i = 0; i < 5; i++)
    {
        TreeNode* node = TreeNode::create();
        node->setData(Value("Hello " + Value(i).asString()));
        topNode->addChild(node);
    }

    TreeNode* aFolderNode = TreeNode::create(true);
    aFolderNode->setData(Value("A folder node"));
    topNode->addChild(aFolderNode);
    for (int i = 0; i < 5; i++)
    {
        TreeNode* node = TreeNode::create();
        node->setData(Value("Good " + Value(i).asString()));
        aFolderNode->addChild(node);
    }

    for (int i = 0; i < 3; i++)
    {
        TreeNode* node = TreeNode::create();
        node->setData(Value("World " + Value(i).asString()));
        topNode->addChild(node);
    }

    TreeNode* anotherTopNode = TreeNode::create();
    anotherTopNode->setData(Value(ValueVector({ Value("I'm a top node too"), Value("ui://TreeView/heart") })));
    _treeView->getRootNode()->addChild(anotherTopNode);
}

void TreeViewScene::onClickNode(EventContext * context)
{
    TreeNode* node = (TreeNode*)context->getData();
    if (node->isFolder() && context->getInput()->isDoubleClick())
        node->setExpaned(!node->isExpanded());
}

void TreeViewScene::renderTreeNode(TreeNode * node)
{
    GObject* btn = node->getCell();
    if (node->isFolder())
    {
        if (node->isExpanded())
            btn->setIcon("ui://TreeView/folder_opened");
        else
            btn->setIcon("ui://TreeView/folder_closed");
        btn->setText(node->getData().asString());
    }
    else if (node->getData().getType() == Value::Type::VECTOR)
    {
        btn->setIcon(node->getData().asValueVector().at(1).asString());
        btn->setText(node->getData().asValueVector().at(0).asString());
    }
    else
    {
        btn->setIcon("ui://TreeView/file");
        btn->setText(node->getData().asString());
    }
}
