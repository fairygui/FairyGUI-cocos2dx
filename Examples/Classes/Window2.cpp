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

    ActionInterval* action = ActionFloat2::create(0.3f, getScale(), Vec2::ONE, CC_CALLBACK_2(Window2::setScale, this));
    action = composeActions(action, tweenfunc::Quad_EaseOut, 0, CC_CALLBACK_0(Window2::onShown, this));
    displayObject()->runAction(action);
}

void  Window2::doHideAnimation()
{
    ActionInterval* action = ActionFloat2::create(0.3f, getScale(), Vec2(0.1f, 0.1f), CC_CALLBACK_2(Window2::setScale, this));
    action = composeActions(action, tweenfunc::Quad_EaseOut, 0, CC_CALLBACK_0(Window2::hideImmediately, this));
    displayObject()->runAction(action);
}

void Window2::onShown()
{
    _contentPane->getTransition("t1")->play();
}

void Window2::onHide()
{
    _contentPane->getTransition("t1")->stop();
}