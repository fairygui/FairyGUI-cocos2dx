#include "PopupMenu.h"
#include "GComponent.h"
#include "GRoot.h"

NS_FGUI_BEGIN
USING_NS_CC;

PopupMenu::PopupMenu()
{
}

PopupMenu::PopupMenu(const std::string & resourceURL)
{
}

PopupMenu::~PopupMenu()
{
}

void PopupMenu::create(const std::string & resourceURL)
{
}

GButton * PopupMenu::addItem(const std::string & caption, MenuItemCallback callback)
{
    return nullptr;
}

GButton * PopupMenu::addItemAt(const std::string & caption, int index, MenuItemCallback callback)
{
    return nullptr;
}

void PopupMenu::addSeperator()
{
}

const std::string & PopupMenu::getItemName(int index)
{
    return STD_STRING_EMPTY;
}

void PopupMenu::setItemText(const std::string & name, const std::string & caption)
{
}

void PopupMenu::setItemVisible(const std::string & name, bool visible)
{
}

void PopupMenu::setItemGrayed(const std::string & name, bool grayed)
{
}

void PopupMenu::setItemCheckable(const std::string & name, bool checkable)
{
}

void PopupMenu::setItemChecked(const std::string & name, bool check)
{
}

bool PopupMenu::isItemChecked(const std::string & name)
{
    return false;
}

bool PopupMenu::removeItem(const std::string & name)
{
    return false;
}

void PopupMenu::clearItems()
{
}

void PopupMenu::show()
{
}

void PopupMenu::show(GObject * target, PopupDirection dir)
{
}

void PopupMenu::onClickItem(EventContext * context)
{
}

void PopupMenu::onEnter(EventContext * context)
{
}

NS_FGUI_END