#include "MenuScene.h"
#include "BasicsScene.h"
#include "TransitionDemoScene.h"
#include "VirtualListScene.h"
#include "LoopListScene.h"
#include "HitTestScene.h"
#include "PullToRefreshScene.h"
#include "ModalWaitingScene.h"
#include "JoystickScene.h"

USING_NS_CC;

void MenuScene::continueInit()
{
    UIPackage::addPackage("UI/MainMenu");
    _view = UIPackage::createObject("MainMenu", "Main")->as<GComponent>();
    _groot->addChild(_view);

    _view->getChild("n1")->addClickListener([this](EventContext*)
    {
        TransitionFade* scene = TransitionFade::create(1, BasicsScene::create());
        Director::getInstance()->replaceScene(scene);
    });
    _view->getChild("n2")->addClickListener([this](EventContext*)
    {
        TransitionProgressRadialCCW* scene  = TransitionProgressRadialCCW::create(1, TransitionDemoScene::create());
        Director::getInstance()->replaceScene(scene);
    });
    _view->getChild("n4")->addClickListener([this](EventContext*)
    {
        TransitionRotoZoom* scene = TransitionRotoZoom::create(1, VirtualListScene::create());
        Director::getInstance()->replaceScene(scene);
    });
    _view->getChild("n5")->addClickListener([this](EventContext*)
    {
        TransitionPageTurn* scene = TransitionPageTurn::create(1, LoopListScene::create(), false);
        Director::getInstance()->replaceScene(scene);
    });
    _view->getChild("n6")->addClickListener([this](EventContext*)
    {
        TransitionSlideInL* scene = TransitionSlideInL::create(1, HitTestScene::create());
        Director::getInstance()->replaceScene(scene);
    });
    _view->getChild("n7")->addClickListener([this](EventContext*)
    {
        TransitionSplitRows* scene = TransitionSplitRows::create(1, PullToRefreshScene::create());
        Director::getInstance()->replaceScene(scene);
    });
    _view->getChild("n8")->addClickListener([this](EventContext*)
    {
        TransitionSplitCols* scene = TransitionSplitCols::create(1, ModalWaitingScene::create());
        Director::getInstance()->replaceScene(scene);
    });
    _view->getChild("n9")->addClickListener([this](EventContext*)
    {
        TransitionSplitCols* scene = TransitionSplitCols::create(1, JoystickScene::create());
        Director::getInstance()->replaceScene(scene);
    });
}