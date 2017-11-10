#include "Controller.h"
#include "GComponent.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
USING_NS_CC;

GController::GController() :
    changing(false),
    autoRadioGroupDepth(false),
    _parent(nullptr),
    _selectedIndex(-1),
    _previousIndex(-1)
{
}

GController::~GController()
{
}

void GController::setSelectedIndex(int value)
{
    if (_selectedIndex != value)
    {
        CCASSERT(value < (int)_pageIds.size(), "Invalid selected index");

        changing = true;

        _previousIndex = _selectedIndex;
        _selectedIndex = value;
        _parent->applyController(this);

        dispatchEvent(UIEventType::Changed);

        changing = false;
    }
}

const std::string& GController::getSelectedPage() const
{
    if (_selectedIndex == -1)
        return STD_STRING_EMPTY;
    else
        return _pageNames[_selectedIndex];
}

void GController::setSelectedPage(const std::string & value)
{
    int i = ToolSet::findInStringArray(_pageNames, value);
    if (i == -1)
        i = 0;
    setSelectedIndex(i);
}

const std::string& GController::getSelectedPageId() const
{
    if (_selectedIndex == -1)
        return STD_STRING_EMPTY;
    else
        return _pageIds[_selectedIndex];
}

void GController::setSelectedPageId(const std::string & value)
{
    int i = ToolSet::findInStringArray(_pageIds, value);
    if (i != -1)
        setSelectedIndex(i);
}

const std::string& GController::getPreviousPage() const
{
    if (_previousIndex == -1)
        return STD_STRING_EMPTY;
    else
        return _pageNames[_previousIndex];
}

const std::string& GController::getPreviousPageId() const
{
    if (_previousIndex == -1)
        return STD_STRING_EMPTY;
    else
        return _pageIds[_previousIndex];
}

int GController::getPageCount() const
{
    return (int)_pageIds.size();
}

bool GController::hasPage(const std::string & aName) const
{
    return ToolSet::findInStringArray(_pageNames, aName) != -1;
}

int GController::getPageIndexById(const std::string & value) const
{
    return ToolSet::findInStringArray(_pageIds, value);
}

const std::string& GController::getPageNameById(const std::string & value) const
{
    int i = ToolSet::findInStringArray(_pageIds, value);
    if (i != -1)
        return _pageNames[i];
    else
        return STD_STRING_EMPTY;
}

const std::string& GController::getPageId(int index) const
{
    return _pageIds[index];
}

void GController::setOppositePageId(const std::string & value)
{
    int i = ToolSet::findInStringArray(_pageIds, value);
    if (i > 0)
        setSelectedIndex(0);
    else if (_pageIds.size() > 1)
        setSelectedIndex(1);
}

void GController::setup(TXMLElement * xml)
{
    const char* p;

    p = xml->Attribute("name");
    if (p)
        name = p;

    autoRadioGroupDepth = xml->BoolAttribute("autoRadioGroupDepth");
    
    p = xml->Attribute("pages");
    if (p)
    {
        std::vector<std::string> elems;
        ToolSet::splitString(p, ',', elems);
        int cnt = (int)elems.size();
        for (int i = 0; i < cnt; i += 2)
        {
            _pageIds.push_back(elems[i]);
            _pageNames.push_back(elems[i + 1]);
        }
    }

    if (_parent != nullptr && _pageIds.size() > 0)
        _selectedIndex = 0;
    else
        _selectedIndex = -1;
}

NS_FGUI_END
