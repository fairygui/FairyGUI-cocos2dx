#ifndef __RELATIONITEM_H__
#define __RELATIONITEM_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"

NS_FGUI_BEGIN

class GObject;
class EventContext;

enum class RelationType
{
    Left_Left,
    Left_Center,
    Left_Right,
    Center_Center,
    Right_Left,
    Right_Center,
    Right_Right,

    Top_Top,
    Top_Middle,
    Top_Bottom,
    Middle_Middle,
    Bottom_Top,
    Bottom_Middle,
    Bottom_Bottom,

    Width,
    Height,

    LeftExt_Left,
    LeftExt_Right,
    RightExt_Left,
    RightExt_Right,
    TopExt_Top,
    TopExt_Bottom,
    BottomExt_Top,
    BottomExt_Bottom,

    Size
};

class RelationDef
{
public:
    bool percent;
    RelationType type;

    RelationDef() {}

    RelationDef(const RelationDef& source)
    {
        this->percent = source.percent;
        this->type = source.type;
    }
};

class RelationItem
{
public:
    RelationItem(GObject* owner);
    ~RelationItem();

    GObject* getTarget() { return _target; }
    void setTarget(GObject* value);

    void add(RelationType relationType, bool usePercent);
    void internalAdd(RelationType relationType, bool usePercent);
    void remove(RelationType relationType);
    void copyFrom(const RelationItem& source);
    bool isEmpty() const;
    void applyOnSelfSizeChanged(float dWidth, float dHeight);

private:
    void applyOnXYChanged(const RelationDef& info, float dx, float dy);

    void applyOnSizeChanged(const RelationDef& info);
    void addRefTarget(GObject* target);
    void releaseRefTarget(GObject* target);
    void onTargetXYChanged(EventContext* context);
    void onTargetSizeChanged(EventContext* context);
    void onTargetGone(EventContext*);

    GObject* _owner;
    GObject* _target;
    std::vector<RelationDef> _defs;
    cocos2d::Vec4 _targetData;
};

NS_FGUI_END

#endif
