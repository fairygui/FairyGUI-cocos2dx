#ifndef __GRICHTEXTFIELD_H__
#define __GRICHTEXTFIELD_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GTextField.h"

NS_FGUI_BEGIN

class GRichTextField : public GTextField
{
public:

public:
    GRichTextField();
    ~GRichTextField();

    CREATE_FUNC(GRichTextField);

protected:
    virtual bool init() override;

private:

};

NS_FGUI_END

#endif // __GRICHTEXTFIELD_H__
