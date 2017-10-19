#ifndef __FIELDTYPES_H__
#define __FIELDTYPES_H__

#include "FairyGUIMacros.h"

NS_FGUI_BEGIN

enum PackageItemType
{
    IMAGE,
    MOVIECLIP,
    SOUND,
    COMPONENT,
    ATLAS,
    FONT,
    MISC
};

enum ButtonMode
{
    BM_COMMON,
    BM_CHECK,
    BM_RADIO
};

enum ChildrenRenderOrder
{
    CRO_ASCENT,
    CRO_DESCENT,
    CRO_ARCH,
};

enum OverflowType
{
    OF_VISIBLE,
    OF_HIDDEN,
    OF_SCROLL
};

enum ScrollType
{
    ST_HORIZONTAL,
    ST_VERTICAL,
    ST_BOTH
};

enum ScrollBarDisplayType
{
    SBD_DEFAULT,
    SBD_VISIBLE,
    SBD_AUTO,
    SDB_HIDDEN
};

enum FillType
{
    FT_NONE,
    FT_SCALE,
    FT_SCALE_MATCH_HEIGHT,
    FT_SCALE_MATCH_WIDTH,
    FT_SCALE_FREE
};

enum ProgressTitleType
{
    PT_PERCENT,
    PT_VALUE_MAX,
    PT_VALUE,
    PT_MAX
};

NS_FGUI_END

#endif