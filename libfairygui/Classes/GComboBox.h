#ifndef __GCOMBOBOX_H__
#define __GCOMBOBOX_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GComponent.h"

NS_FGUI_BEGIN

class GComboBox : public GComponent
{
public:

public:
    GComboBox();
    ~GComboBox();

    CREATE_FUNC(GComboBox);

protected:
    virtual bool init() override;

private:

};

NS_FGUI_END

#endif // __GCOMBOBOX_H__
