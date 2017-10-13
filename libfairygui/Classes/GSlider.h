#ifndef __GSLIDER_H__
#define __GSLIDER_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GComponent.h"

NS_FGUI_BEGIN

class GSlider : public GComponent
{
public:
    GSlider();
    ~GSlider();

    CREATE_FUNC(GSlider);

protected:
    virtual bool init() override;

private:

};

NS_FGUI_END

#endif // __GSLIDER_H__
