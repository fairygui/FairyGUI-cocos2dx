#include "Window2.h"

void Window2::onInit()
{
    setContentPane(UIPackage::createObject("Basics", "WindowB")->as<GComponent>());
    center();
}

void  Window2::doShowAnimation()
{
    setScale(0.1f, 0.1f);
    setPivot(0.5f, 0.5f);
    displayObject()->runAction(createEaseAction(tweenfunc::Quad_EaseOut,
        ActionVec2::create(0.3f, getScale(), Vec2::ONE,
            [this](const cocos2d::Vec2& value)
    {
        setScale(value.x, value.y);
    },
            [this]()
    {
        onShown();
    }
    )));
}

void  Window2::doHideAnimation()
{
    displayObject()->runAction(createEaseAction(tweenfunc::Quad_EaseOut, ActionVec2::create(0.3f, getScale(), Vec2(0.1f, 0.1f),
        [this](const cocos2d::Vec2& value)
    {
        setScale(value.x, value.y);
    },
        [this]()
    {
        hideImmediately();
    }
    )));
}

void Window2::onShown()
{

}

void Window2::onHide()
{

}