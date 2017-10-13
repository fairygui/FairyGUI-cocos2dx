#ifndef __GLIST_H__
#define __GLIST_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GComponent.h"

NS_FGUI_BEGIN

class GList : public GComponent
{
public:

public:
    GList();
    ~GList();

    CREATE_FUNC(GList);

protected:
    virtual bool init() override;

private:

};

NS_FGUI_END

#endif // __GLIST_H__
