#ifndef __GIMAGE_H__
#define __GIMAGE_H__

#include "cocos2d.h"
#include "ui/UIScale9Sprite.h"
#include "FairyGUIMacros.h"
#include "GObject.h"

NS_FGUI_BEGIN

class GImage : public GObject
{
public:

public:
    GImage();
    ~GImage();

    virtual void constructFromResource() override;

    CREATE_FUNC(GImage);

protected:
    virtual bool init() override;

private:
    cocos2d::ui::Scale9Sprite* _content;
};

NS_FGUI_END

#endif // __GIMAGE_H__
