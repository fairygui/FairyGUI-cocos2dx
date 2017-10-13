#ifndef __GGROUP_H__
#define __GGROUP_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GObject.h"

NS_FGUI_BEGIN

class GGroup : public GObject
{
public:

public:
    GGroup();
    ~GGroup();

    CREATE_FUNC(GGroup);

    void setBoundsChangedFlag(bool childSizeChanged = false);
    void moveChildren(float dx, float dy);
    void resizeChildren(float dWidth, float dHeight);

    bool _updating;
protected:
    virtual bool init() override;

private:

};

NS_FGUI_END

#endif // __GGROUP_H__
