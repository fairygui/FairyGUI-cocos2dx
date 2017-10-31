#ifndef __FUISPRITE_H__
#define __FUISPRITE_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"

NS_FGUI_BEGIN

class FUISprite : public cocos2d::Sprite
{
public:
    FUISprite();
    ~FUISprite();

    CREATE_FUNC(FUISprite);

    void clearContent();
    void setScale9Grid(cocos2d::Rect* value);
};

NS_FGUI_END

#endif
