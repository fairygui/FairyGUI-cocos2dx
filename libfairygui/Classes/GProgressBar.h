#ifndef __GPROGRESSBAR_H__
#define __GPROGRESSBAR_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GComponent.h"

NS_FGUI_BEGIN

class GProgressBar : public GComponent
{
public:

public:
    GProgressBar();
    ~GProgressBar();

    CREATE_FUNC(GProgressBar);

protected:
    virtual bool init() override;

private:

};

NS_FGUI_END

#endif // __GPROGRESSBAR_H__
