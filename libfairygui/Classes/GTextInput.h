#ifndef __GTEXTINPUT_H__
#define __GTEXTINPUT_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GTextField.h"

NS_FGUI_BEGIN

class GTextInput : public GTextField
{
public:
    GTextInput();
    ~GTextInput();

    CREATE_FUNC(GTextInput);

protected:
    virtual bool init() override;

private:

};

NS_FGUI_END

#endif // __GTEXTINPUT_H__
