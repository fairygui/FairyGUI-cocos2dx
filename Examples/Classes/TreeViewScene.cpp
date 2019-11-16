#include "TreeViewScene.h"

USING_NS_CC;

TreeViewScene::TreeViewScene() : _tree1(nullptr)
{
}

TreeViewScene::~TreeViewScene()
{
}

void TreeViewScene::continueInit()
{
    UIPackage::addPackage("UI/TreeView");

    _view = UIPackage::createObject("TreeView", "Main")->as<GComponent>();
    _groot->addChild(_view);

    _tree1 = _view->getChild("tree")->as<GTree>();
    _tree1->addEventListener(UIEventType::ClickItem, CC_CALLBACK_1(TreeViewScene::onClickNode, this));

    _tree2 = _view->getChild("tree2")->as<GTree>();
    _tree2->addEventListener(UIEventType::ClickItem, CC_CALLBACK_1(TreeViewScene::onClickNode, this));
    _tree2->treeNodeRender = CC_CALLBACK_2(TreeViewScene::renderTreeNode, this);

    GTreeNode* topNode = GTreeNode::create(true);
    topNode->setData(Value("I'm a top node"));
    _tree2->getRootNode()->addChild(topNode);
    for (int i = 0; i < 5; i++)
    {
        GTreeNode* node = GTreeNode::create();
        node->setData(Value("Hello " + Value(i).asString()));
        topNode->addChild(node);
    }

    GTreeNode* aFolderNode = GTreeNode::create(true);
    aFolderNode->setData(Value("A folder node"));
    topNode->addChild(aFolderNode);
    for (int i = 0; i < 5; i++)
    {
        GTreeNode* node = GTreeNode::create();
        node->setData(Value("Good " + Value(i).asString()));
        aFolderNode->addChild(node);
    }

    for (int i = 0; i < 3; i++)
    {
        GTreeNode* node = GTreeNode::create();
        node->setData(Value("World " + Value(i).asString()));
        topNode->addChild(node);
    }

    GTreeNode* anotherTopNode = GTreeNode::create();
    anotherTopNode->setData(Value(ValueVector({Value("I'm a top node too"), Value("ui://TreeView/heart")})));
    _tree2->getRootNode()->addChild(anotherTopNode);
}

void TreeViewScene::onClickNode(EventContext* context)
{
    GTreeNode* node = ((GObject*)context->getData())->treeNode();
    CCLOG("click node %s", node->getText().c_str());
}

void TreeViewScene::renderTreeNode(GTreeNode* node, GComponent* obj)
{
    GObject* btn = node->getCell();
    if (node->isFolder())
    {
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
