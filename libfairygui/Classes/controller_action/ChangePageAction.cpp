#include "ChangePageAction.h"
#include "Controller.h"
#include "GComponent.h"
#include "utils/ByteBuffer.h"

NS_FGUI_BEGIN
USING_NS_CC;

void ChangePageAction::setup(ByteBuffer * buffer)
{
    ControllerAction::setup(buffer);

    objectId = buffer->ReadS();
    controllerName = buffer->ReadS();
    targetPage = buffer->ReadS();
}

void ChangePageAction::enter(GController * controller)
{
    if (controllerName.empty())
        return;

    GComponent* gcom;
    if (!objectId.empty())
        gcom = controller->getParent()->getChildById(objectId)->as<GComponent>();
    else
        gcom = controller->getParent();
    if (gcom != nullptr)
    {
        GController* cc = gcom->getController(controllerName);
        if (cc != nullptr && cc != controller && !cc->changing)
            cc->setSelectedPageId(targetPage);
    }
}

void ChangePageAction::leave(GController * controller)
{
}

NS_FGUI_END