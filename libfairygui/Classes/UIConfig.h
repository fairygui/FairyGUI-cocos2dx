#ifndef __UICONFIG_H__
#define __UICONFIG_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"

NS_FGUI_BEGIN

class UIConfig
{
public:
    static char* defaultFont;
    static int defaultScrollStep;
    static float defaultScrollDecelerationRate;
    static bool defaultScrollTouchEffect;
    static bool defaultScrollBounceEffect;
    static ScrollBarDisplayType defaultScrollBarDisplay;
    static std::string verticalScrollBar;
    static std::string horizontalScrollBar;
    static int touchDragSensitivity;
    static int touchScrollSensitivity;
};

NS_FGUI_END

#endif // __UICONFIG_H__
