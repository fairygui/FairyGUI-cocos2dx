#include "Controller.h"
#include "GComponent.h"
#include "utils/ToolSet.h"
#include "utils/ByteBuffer.h"
#include "controller_action/ControllerAction.h"

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
    for (auto &it : _actions)
        delete it;
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

void GController::runActions()
{
    if (_actions.empty())
        return;

    for (auto &it : _actions)
        it->run(this, getPreviousPageId(), getSelectedPageId());
}

void GController::setup(ByteBuffer* buffer)
{
    int beginPos = buffer->position;
    buffer->Seek(beginPos, 0);

    name = buffer->ReadS();
    autoRadioGroupDepth = buffer->ReadBool();

    buffer->Seek(beginPos, 1);

    int cnt = buffer->ReadShort();
    _pageIds.resize(cnt);
    _pageNames.resize(cnt);
    for (int i = 0; i < cnt; i++)
    {
        _pageIds[i].assign(buffer->ReadS());
        _pageNames[i].assign(buffer->ReadS());
    }

    buffer->Seek(beginPos, 2);

    cnt = buffer->ReadShort();
    if (cnt > 0)
    {
        for (int i = 0; i < cnt; i++)
        {
            int nextPos = buffer->ReadShort();
            nextPos += buffer->position;

            ControllerAction* action = ControllerAction::createAction(buffer->ReadByte());
            action->setup(buffer);
            _actions.push_back(action);

            buffer->position = nextPos;
        }
    }

    if (_parent != nullptr && _pageIds.size() > 0)
        _selectedIndex = 0;
    else
        _selectedIndex = -1;
}

NS_FGUI_END
