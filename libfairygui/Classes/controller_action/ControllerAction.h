#ifndef __CONTROLLERACTION_H__
#define __CONTROLLERACTION_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class GController;

class ControllerAction
{
public:
    static ControllerAction* createAction(const char* types);

    ControllerAction();
    ~ControllerAction();

    void run(GController* controller, const std::string& prevPage, const std::string& curPage);
    virtual void setup(TXMLElement * xml);

    std::vector<std::string> fromPage;
    std::vector<std::string> toPage;

protected:
    virtual void enter(GController* controller) = 0;
    virtual void leave(GController* controller) = 0;
};

NS_FGUI_END

#endif
