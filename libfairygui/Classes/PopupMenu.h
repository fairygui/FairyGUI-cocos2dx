#ifndef __POPUPMENU_H__
#define __POPUPMENU_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "event/UIEventDispatcher.h"

NS_FGUI_BEGIN

class GObject;
class GComponent;
class GButton;
class GList;

class PopupMenu
{
public:
    typedef std::function<void()> MenuItemCallback;

    PopupMenu();
    PopupMenu(const std::string& resourceURL);
    ~PopupMenu();

    void create(const std::string& resourceURL);
    GButton* addItem(const std::string& caption, MenuItemCallback callback);
    GButton* addItemAt(const std::string& caption, int index, MenuItemCallback callback);
    void addSeperator();
    const std::string& getItemName(int index);
    void setItemText(const std::string& name, const std::string& caption);
    void setItemVisible(const std::string& name, bool visible);
    void setItemGrayed(const std::string& name, bool grayed);
    void setItemCheckable(const std::string& name, bool checkable);
    void setItemChecked(const std::string& name, bool check);
    bool isItemChecked(const std::string& name);
    bool removeItem(const std::string& name);
    void clearItems();
    GComponent* getContentPane() { return _contentPane; }
    GList* getList() { return _list; }
    void show();
    void show(GObject* target, PopupDirection dir);

protected:
    GComponent* _contentPane;
    GList* _list;

private:
    void onClickItem(EventContext* context);
    void onEnter(EventContext* context);
};

NS_FGUI_END

#endif
