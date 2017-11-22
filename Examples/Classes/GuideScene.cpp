#include "GuideScene.h"

USING_NS_CC;

GuideScene::GuideScene():_guideLayer(nullptr)
{

}

GuideScene::~GuideScene()
{
    CC_SAFE_RELEASE(_guideLayer);
}

void GuideScene::continueInit()
{
    UIPackage::addPackage("UI/Guide");

    _view = UIPackage::createObject("Guide", "Main")->as<GComponent>();
    _groot->addChild(_view);

    _guideLayer = UIPackage::createObject("Guide", "GuideLayer")->as<GComponent>();
    _guideLayer->makeFullScreen();
    _guideLayer->addRelation(_groot, RelationType::Size);

    GObject* bagBtn = _view->getChild("bagBtn");
    bagBtn->addClickListener([this](EventContext*)
    {
        _guideLayer->removeFromParent();
    });

    _view->getChild("n2")->addClickListener([this, bagBtn](EventContext*)
    {
        _groot->addChild(_guideLayer); //!!Before using TransformRect(or GlobalToLocal), the object must be added first
        Rect rect = bagBtn->transformRect(Rect(Vec2::ZERO, bagBtn->getSize()), _guideLayer);

        GObject* window = _guideLayer->getChild("window");
        window->setSize((int)rect.size.width, (int)rect.size.height);

        ActionFloat2* action = ActionFloat2::create(0.5f, window->getPosition(), rect.origin, CC_CALLBACK_2(GObject::setPosition, window));
        window->displayObject()->runAction(action);
    });
}