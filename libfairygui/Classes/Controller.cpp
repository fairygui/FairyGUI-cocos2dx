#include "Controller.h"
#include "GComponent.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
USING_NS_CC;

Controller::Controller() :
    _selectedIndex(-1),
    _previousIndex(-1),
    _parent(nullptr)
{
}


Controller::~Controller()
{
}

void Controller::setSelectedIndex(int value)
{
    if (_selectedIndex != value)
    {
        CCASSERT(value < (int)_pageIds.size(), "Invalid selected index");

        changing = true;

        _previousIndex = _selectedIndex;
        _selectedIndex = value;
        _parent->applyController(this);

        dispatchEvent(UIEventType::StatusChange);

        changing = false;
    }
}

std::string Controller::getSelectedPage()
{
    if (_selectedIndex == -1)
        return STD_STRING_EMPTY;
    else
        return _pageNames[_selectedIndex];
}

void Controller::setSelectedPage(const std::string & value)
{
    int i = ToolSet::findInStringArray(_pageNames, value);
    if (i == -1)
        i = 0;
    setSelectedIndex(i);
}

std::string Controller::getSelectedPageId()
{
    if (_selectedIndex == -1)
        return STD_STRING_EMPTY;
    else
        return _pageIds[_selectedIndex];
}

void Controller::setSelectedPageId(const std::string & value)
{
    int i = ToolSet::findInStringArray(_pageIds, value);
    if (i != -1)
        setSelectedIndex(i);
}

std::string Controller::getPreviousPage()
{
    if (_previousIndex == -1)
        return STD_STRING_EMPTY;
    else
        return _pageNames[_previousIndex];
}

std::string Controller::getPreviousPageId()
{
    if (_previousIndex == -1)
        return STD_STRING_EMPTY;
    else
        return _pageIds[_previousIndex];
}

int Controller::getPageCount()
{
    return _pageIds.size();
}

bool Controller::hasPage(const std::string & aName)
{
    return ToolSet::findInStringArray(_pageNames, aName) != -1;
}

int Controller::getPageIndexById(const std::string & value)
{
    return ToolSet::findInStringArray(_pageIds, value);
}

std::string Controller::getPageNameById(const std::string & value)
{
    int i = ToolSet::findInStringArray(_pageIds, value);
    if (i != -1)
        return _pageNames[i];
    else
        return STD_STRING_EMPTY;
}

std::string Controller::getPageId(int index)
{
    return _pageIds[index];
}

void Controller::setOppositePageId(const std::string & value)
{
    int i = ToolSet::findInStringArray(_pageIds, value);
    if (i > 0)
        setSelectedIndex(0);
    else if (_pageIds.size() > 1)
        setSelectedIndex(1);
}

void Controller::setup(tinyxml2::XMLElement * xml)
{
    name = xml->Attribute("name");
    autoRadioGroupDepth = xml->BoolAttribute("autoRadioGroupDepth");

    const char* p = xml->Attribute("pages");
    if (p)
    {
        std::vector<std::string> elems;
        ToolSet::splitString(p, ',', elems);
        int cnt = elems.size();
        for (int i = 0; i < cnt; i += 2)
        {
            _pageIds.push_back(elems[i]);
            _pageNames.push_back(elems[i + 1]);
        }
    }

    if (_parent != nullptr && _pageIds.size() >= 0)
        _selectedIndex = 0;
    else
        _selectedIndex = -1;
}

NS_FGUI_END