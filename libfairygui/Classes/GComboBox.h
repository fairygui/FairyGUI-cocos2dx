#ifndef __GCOMBOBOX_H__
#define __GCOMBOBOX_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GComponent.h"
#include "GList.h"

NS_FGUI_BEGIN

class GComboBox : public GComponent
{
public:
    GComboBox();
    ~GComboBox();

    CREATE_FUNC(GComboBox);

    const std::string& getTitle() const;
    void setTitle(const std::string& value);

    virtual const std::string& getText() const override { return getTitle(); }
    virtual void setText(const std::string& value) override { setTitle(value); }

    virtual const std::string& getIcon() const override;
    virtual void setIcon(const std::string& value) override;

    const std::string& getValue() const;
    void setValue(const std::string& value);

    int getSelectedIndex() const { return _selectedIndex; }
    void setSelectedIndex(int value);

    Controller* getSelectionController() const { return _selectionController; }
    void setSelectionController(Controller* value) { _selectionController = value; }

    std::vector<std::string>& getItems() { return _items; }
    std::vector<std::string>& getIcons() { return _icons; }
    std::vector<std::string>& getValues() { return _values; }

    void refresh();

    int visibleItemCount;
    PopupDirection popupDirection;

protected:
    virtual void constructFromXML(tinyxml2::XMLElement* xml) override;
    virtual void setup_AfterAdd(tinyxml2::XMLElement* xml) override;
    virtual void handleControllerChanged(Controller* c) override;
    virtual void handleGrayedChanged() override;

    void setState(const std::string& value);
    void setCurrentState();
    void updateSelectionController();
    void updateDropdownList();
    void showDropdown();
    void renderDropdownList();

    GComponent* _dropdown;
    GObject* _titleObject;
    GObject* _iconObject;
    GList* _list;
    Controller* _selectionController;

    std::vector<std::string> _items;
    std::vector<std::string> _icons;
    std::vector<std::string> _values;

private:

    void onClickItem(EventContext* context);
    void onRollover(EventContext* context);
    void onRollout(EventContext* context);
    void onTouchBegin(EventContext* context);
    void onTouchEnd(EventContext* context);
    void onPopupWinClosed(EventContext* context);

    bool _itemsUpdated;
    int _selectedIndex;
    Controller* _buttonController;
    bool _down;
    bool _over;
};

NS_FGUI_END

#endif
