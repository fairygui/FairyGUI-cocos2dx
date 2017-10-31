#include "Window1.h"

void Window1::onInit()
{
    setContentPane(UIPackage::createObject("Basics", "WindowA")->asCom());
    center();
}

void Window1::onShown()
{
    GList* list = _contentPane->getChild("n6")->asList();
    list->removeChildrenToPool();

    for (int i = 0; i < 6; i++)
    {
        GButton* item = list->addItemFromPool()->asButton();
        item->setTitle(Value(i).asString());
        item->setIcon("ui://Basics/r4");
    }
}