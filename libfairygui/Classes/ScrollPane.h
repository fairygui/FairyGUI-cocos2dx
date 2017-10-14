#ifndef __SCROLLPANE_H__
#define __SCROLLPANE_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"

NS_FGUI_BEGIN

class GComponent;

class ScrollPane
{
public:
    ScrollPane();
    ~ScrollPane();

    float getPercX();
    void setPercX(float value, bool ani = false);
    float getPercY();
    void setPercY(float value, bool ani = false);

    void scrollLeft(float ratio = 1, bool ani = false);
    void scrollRight(float ratio = 1, bool ani = false);
    void scrollUp(float ratio = 1, bool ani = false);
    void scrollDown(float ratio = 1, bool ani = false);

private:

};

NS_FGUI_END

#endif // __SCROLLPANE_H__
