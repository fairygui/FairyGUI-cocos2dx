#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "FairyGUI.h"
#include "Window1.h"
#include "Window2.h"

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

private:
    void runDemo(fairygui::EventContext* context);
    void onClickBack(fairygui::EventContext* context);

    void playDragDrop();
    void playWindow();
    void playDepth();
    void playText();

    fairygui::GComponent* _view;
    fairygui::GObject* _backBtn;
    fairygui::GComponent* _demoContainer;
    fairygui::Controller* _cc;
    Window1* _winA;
    Window2* _winB;

    cocos2d::Map<std::string, fairygui::GComponent*> _demoObjects;

};

#endif // __HELLOWORLD_SCENE_H__
