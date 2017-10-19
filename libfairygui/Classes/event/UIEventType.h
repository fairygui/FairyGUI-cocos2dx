#ifndef __UIEVENTTYPE_H__
#define __UIEVENTTYPE_H__

#include "FairyGUIMacros.h"

NS_FGUI_BEGIN

class UIEventType
{
public:
    static const int StatusChange = 1;

    static const int TouchBegin = 10;
    static const int TouchMove = 11;
    static const int TouchEnd = 12;
    static const int Click = 13;
    static const int RollOver = 14;
    static const int RollOut = 15;
    static const int MouseWheel = 16;

    static const int PositionChange = 20;
    static const int SizeChange = 21;

    static const int SliderChange = 31;

    static const int Scroll = 40;
    static const int ScrollEnd = 41;
    static const int PullDownRelease = 42;
    static const int PullUpRelease = 43;
};

NS_FGUI_END

#endif // __UIEVENTTYPE_H__
