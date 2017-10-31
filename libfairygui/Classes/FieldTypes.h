#ifndef __FIELDTYPES_H__
#define __FIELDTYPES_H__

#include "FairyGUIMacros.h"

NS_FGUI_BEGIN

enum class PackageItemType
{
    IMAGE,
    MOVIECLIP,
    SOUND,
    COMPONENT,
    ATLAS,
    FONT,
    MISC
};

enum class ButtonMode
{
    COMMON,
    CHECK,
    RADIO
};

enum class ChildrenRenderOrder
{
    ASCENT,
    DESCENT,
    ARCH,
};

enum class OverflowType
{
    VISIBLE,
    HIDDEN,
    SCROLL
};

enum class ScrollType
{
    HORIZONTAL,
    VERTICAL,
    BOTH
};

enum ScrollBarDisplayType
{
    DEFAULT,
    VISIBLE,
    AUTO,
    HIDDEN
};

enum class LoaderFillType
{
    NONE,
    SCALE,
    SCALE_MATCH_HEIGHT,
    SCALE_MATCH_WIDTH,
    SCALE_FREE
};

enum class ProgressTitleType
{
    PERCENT,
    VALUE_MAX,
    VALUE,
    MAX
};

enum class ListLayoutType
{
    SINGLE_COLUMN,
    SINGLE_ROW,
    FLOW_HORIZONTAL,
    FLOW_VERTICAL,
    PAGINATION
};

enum class ListSelectionMode
{
    SINGLE,
    MULTIPLE,
    MULTIPLE_SINGLECLICK,
    NONE
};

enum class GroupLayoutType
{
    NONE,
    HORIZONTAL,
    VERTICAL
};

enum class PopupDirection
{
    AUTO,
    UP,
    DOWN
};

enum class TextAutoSize
{
    NONE,
    BOTH,
    HEIGHT,
    SHRINK
};

enum class FlipType
{
    NONE,
    HORIZONTAL,
    VERTICAL,
    BOTH
};

NS_FGUI_END

#endif