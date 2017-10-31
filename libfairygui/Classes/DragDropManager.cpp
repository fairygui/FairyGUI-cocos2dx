#include "DragDropManager.h"
#include "UIObjectFactory.h"
#include "GRoot.h"

NS_FGUI_BEGIN
USING_NS_CC;

DragDropManager* DragDropManager::_inst = nullptr;

DragDropManager::DragDropManager() :
    _agent(nullptr),
    _sourceData(nullptr)
{
    _agent = (GLoader*)UIObjectFactory::newObject("loader");
    _agent->retain();
    _agent->setTouchable(false);
    _agent->setDraggable(true);
    _agent->setSize(100, 100);
    _agent->setPivot(0.5f, 0.5f, true);
    _agent->setAlign(TextHAlignment::CENTER);
    _agent->setVerticalAlign(TextVAlignment::CENTER);
    _agent->setSortingOrder(INT_MAX);
    _agent->addEventListener(UIEventType::DragEnd, CC_CALLBACK_1(DragDropManager::onDragEnd, this));
}

DragDropManager::~DragDropManager()
{
    CC_SAFE_RELEASE(_agent);
}

DragDropManager* DragDropManager::getInstance()
{
    if (_inst == nullptr)
        _inst = new DragDropManager();

    return _inst;
}

void DragDropManager::startDrag(const std::string & icon, void * sourceData, int touchPointID)
{
    if (_agent->getParent() != nullptr)
        return;

    _sourceData = sourceData;
    _agent->setURL(icon);
    GRoot::getInstance()->addChild(_agent);
    Vec2 pt = GRoot::getInstance()->globalToLocal(GRoot::getInstance()->getTouchPosition(touchPointID));
    _agent->setPosition(pt.x, pt.y);
    _agent->startDrag(touchPointID);
}

void DragDropManager::cancel()
{
    if (_agent->getParent() != nullptr)
    {
        _agent->stopDrag();
        GRoot::getInstance()->removeChild(_agent);
        _sourceData = nullptr;
    }
}

void DragDropManager::onDragEnd(EventContext * context)
{
    if (_agent->getParent() == nullptr) //cancelled
        return;

    GRoot::getInstance()->removeChild(_agent);

    void* sourceData = _sourceData;
    _sourceData = nullptr;

    GObject* obj = GRoot::getInstance()->getTouchTarget();
    while (obj != nullptr)
    {
        if (dynamic_cast<GComponent*>(obj))
        {
            if (obj->hasEventListener(UIEventType::Drop))
            {
                //obj->requestFocus();
                obj->dispatchEvent(UIEventType::Drop, sourceData);
                return;
            }
        }

        obj = obj->getParent();
    }
}

NS_FGUI_END