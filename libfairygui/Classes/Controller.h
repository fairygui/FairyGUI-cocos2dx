#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "event/UIEventDispatcher.h"
#include "tinyxml2/tinyxml2.h"

NS_FGUI_BEGIN

class GComponent;

class Controller : public UIEventDispatcher
{
public:
    Controller();
    ~Controller();

    GComponent* getParent() { return _parent; }
    void setParent(GComponent* value) { _parent = value; }

    int getSelectedIndex() { return _selectedIndex; }
    void setSelectedIndex(int value);

    std::string getSelectedPage();
    void setSelectedPage(const std::string& value);

    std::string getSelectedPageId();
    void setSelectedPageId(const std::string& value);

    int getPrevisousIndex() { return _previousIndex; }
    std::string getPreviousPage();
    std::string getPreviousPageId();

    int getPageCount();
    bool hasPage(const std::string& aName);
    int getPageIndexById(const std::string& value);
    std::string getPageNameById(const std::string& value);
    std::string getPageId(int index);
    void setOppositePageId(const std::string& value);

    void setup(tinyxml2::XMLElement* xml);

    std::string name;

    bool changing;
    bool autoRadioGroupDepth;

private:
    GComponent* _parent;

    int _selectedIndex;
    int _previousIndex;
    std::vector<std::string> _pageIds;
    std::vector<std::string> _pageNames;
    //std::vector<ControllerAction> _actions;
};

NS_FGUI_END

#endif // __CONTROLLER_H__
