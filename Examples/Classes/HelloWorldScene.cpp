#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
USING_NS_FGUI;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width / 2;
        float y = origin.y + closeItem->getContentSize().height / 2;
        closeItem->setPosition(Vec2(x, y));
    }

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    this->addChild(GRoot::getInstance()->displayObject());

    UIPackage::addPackage("Basics");
    _view = UIPackage::createObject("Basics", "Main")->asCom();
    GRoot::getInstance()->addChild(_view);

    _backBtn = _view->getChild("btn_Back");
    _backBtn->setVisible(false);
    _backBtn->addClickListener(CC_CALLBACK_1(HelloWorld::onClickBack, this));

    _demoContainer = _view->getChild("container")->asCom();
    _cc = _view->getController("c1");

    int cnt = _view->numChildren();
    for (int i = 0; i < cnt; i++)
    {
        GObject* obj = _view->getChildAt(i);
        if (obj->getGroup() != nullptr && obj->getGroup()->name.compare("btns") == 0)
            obj->addClickListener(CC_CALLBACK_1(HelloWorld::runDemo, this));
    }

    return true;
}

void HelloWorld::onClickBack(EventContext* context)
{
    _cc->setSelectedIndex(0);
    _backBtn->setVisible(false);
}

void HelloWorld::runDemo(EventContext* context)
{
    std::string type = ((GObject*)context->getSender())->name.substr(4);
    auto it = _demoObjects.find(type);
    GComponent* obj;
    if (it == _demoObjects.end())
    {
        obj = UIPackage::createObject("Basics", "Demo_" + type)->asCom();
        _demoObjects.insert(type, obj);
    }
    else
        obj = it->second;

    _demoContainer->removeChildren();
    _demoContainer->addChild(obj);
    _cc->setSelectedIndex(1);
    _backBtn->setVisible(true);
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
