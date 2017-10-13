#ifndef __GSCROLLBAR_H__
#define __GSCROLLBAR_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GComponent.h"

NS_FGUI_BEGIN

class GScrollBar : public GComponent
{
public:
    GScrollBar();
    ~GScrollBar();

    CREATE_FUNC(GScrollBar);

protected:
    virtual bool init() override;

private:

};

NS_FGUI_END

#endif // __GSCROLLBAR_H__
