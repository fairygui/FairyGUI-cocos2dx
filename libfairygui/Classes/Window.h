#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "GComponent.h"

NS_FGUI_BEGIN

class Window : public GComponent
{
public:
    Window();
    ~Window();

    CREATE_FUNC(Window);

    virtual bool init() override;
private:
};

NS_FGUI_END

#endif // __WINDOW_H__
