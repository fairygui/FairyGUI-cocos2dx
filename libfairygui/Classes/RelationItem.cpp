#include "RelationItem.h"
#include "GComponent.h"
#include "GGroup.h"
#include "event/UIEventType.h"

NS_FGUI_BEGIN
USING_NS_CC;

RelationItem::RelationItem(GObject* owner) :_target(nullptr)
{
    _owner = owner;
}

RelationItem::~RelationItem()
{
    if (_target != nullptr)
    {
        releaseRefTarget(_target);
        _target = nullptr;
    }
}

void RelationItem::setTarget(GObject * value)
{
    if (_target != value)
    {
        if (_target != nullptr)
            releaseRefTarget(_target);
        _target = value;
        if (_target != nullptr)
            addRefTarget(_target);
    }
}

void RelationItem::add(RelationType relationType, bool usePercent)
{
    if (relationType == RelationType::Size_Size)
    {
        add(RelationType::Width_Width, usePercent);
        add(RelationType::Height_Height, usePercent);
        return;
    }

    for (auto it = _defs.begin(); it != _defs.end(); ++it)
    {
        if (it->type == relationType)
            return;
    }

    internalAdd(relationType, usePercent);
}

void RelationItem::internalAdd(RelationType relationType, bool usePercent)
{
    if (relationType == RelationType::Size_Size)
    {
        internalAdd(RelationType::Width_Width, usePercent);
        internalAdd(RelationType::Height_Height, usePercent);
        return;
    }

    RelationDef info;
    info.percent = usePercent;
    info.type = relationType;
    _defs.push_back(info);

    //当使用中线关联时，因为需要除以2，很容易因为奇数宽度/高度造成小数点坐标；当使用百分比时，也会造成小数坐标；
    //所以设置了这类关联的对象，自动启用pixelSnapping
    if (usePercent || relationType == RelationType::Left_Center || relationType == RelationType::Center_Center || relationType == RelationType::Right_Center
        || relationType == RelationType::Top_Middle || relationType == RelationType::Middle_Middle || relationType == RelationType::Bottom_Middle)
        _owner->setPixelSnapping(true);
}

void RelationItem::remove(RelationType relationType)
{
    if (relationType == RelationType::Size_Size)
    {
        remove(RelationType::Width_Width);
        remove(RelationType::Height_Height);
        return;
    }

    for (auto it = _defs.begin(); it != _defs.end(); ++it)
    {
        if (it->type == relationType)
        {
            _defs.erase(it);
            break;
        }
    }
}

void RelationItem::copyFrom(const RelationItem& source)
{
    setTarget(source._target);

    _defs.clear();
    for (auto it = source._defs.begin(); it != source._defs.end(); ++it)
        _defs.push_back(*it);
}

bool RelationItem::isEmpty() const
{
    return _defs.size() == 0;
}

void RelationItem::applyOnSelfSizeChanged(float dWidth, float dHeight)
{
    int cnt = _defs.size();
    if (cnt == 0)
        return;

    float ox = _owner->_x;
    float oy = _owner->_y;

    for (int i = 0; i < cnt; i++)
    {
        RelationDef info = _defs[i];
        switch (info.type)
        {
        case RelationType::Center_Center:
        case RelationType::Right_Center:
            _owner->setX(_owner->_x - dWidth / 2);
            break;

        case RelationType::Right_Left:
        case RelationType::Right_Right:
            _owner->setX(_owner->_x - dWidth);
            break;

        case RelationType::Middle_Middle:
        case RelationType::Bottom_Middle:
            _owner->setY(_owner->_y - dHeight / 2);
            break;
        case RelationType::Bottom_Top:
        case RelationType::Bottom_Bottom:
            _owner->setY(_owner->_y - dHeight);
            break;
        }
    }

    if (ox != _owner->_x || oy != _owner->_y)
    {
        ox = _owner->_x - ox;
        oy = _owner->_y - oy;

        _owner->updateGearFromRelations(1, ox, oy);

        /*if (_owner->_parent != nullptr)
        {
            int transCount = _owner->parent._transitions.Count;
            for (int i = 0; i < transCount; i++)
                _owner->parent._transitions[i].UpdateFromRelations(_owner->id, ox, oy);
        }*/
    }
}

void RelationItem::applyOnXYChanged(const RelationDef& info, float dx, float dy)
{
    switch (info.type)
    {
    case RelationType::Left_Left:
    case RelationType::Left_Center:
    case RelationType::Left_Right:
    case RelationType::Center_Center:
    case RelationType::Right_Left:
    case RelationType::Right_Center:
    case RelationType::Right_Right:
        _owner->setX(_owner->_x + dx);
        break;

    case RelationType::Top_Top:
    case RelationType::Top_Middle:
    case RelationType::Top_Bottom:
    case RelationType::Middle_Middle:
    case RelationType::Bottom_Top:
    case RelationType::Bottom_Middle:
    case RelationType::Bottom_Bottom:
        _owner->setY(_owner->_y + dy);
        break;

    case RelationType::Width_Width:
    case RelationType::Height_Height:
        break;

    case RelationType::LeftExt_Left:
    case RelationType::LeftExt_Right:
        _owner->setX(_owner->_x + dx);
        _owner->setWidth(_owner->_rawWidth - dx);
        break;

    case RelationType::RightExt_Left:
    case RelationType::RightExt_Right:
        _owner->setWidth(_owner->_rawWidth + dx);
        break;

    case RelationType::TopExt_Top:
    case RelationType::TopExt_Bottom:
        _owner->setY(_owner->_y + dy);
        _owner->setHeight(_owner->_rawHeight - dy);
        break;

    case RelationType::BottomExt_Top:
    case RelationType::BottomExt_Bottom:
        _owner->setHeight(_owner->_rawHeight + dy);
        break;
    }
}

void RelationItem::applyOnSizeChanged(const RelationDef& info)
{
    float targetX, targetY;
    if (_target != _owner->_parent)
    {
        targetX = _target->_x;
        targetY = _target->_y;
    }
    else
    {
        targetX = 0;
        targetY = 0;
    }
    float v, tmp;

    switch (info.type)
    {
    case RelationType::Left_Left:
        if (info.percent && _target == _owner->_parent)
        {
            v = _owner->_x - targetX;
            if (info.percent)
                v = v / _targetData.z * _target->_width;
            _owner->setX(targetX + v);
        }
        break;
    case RelationType::Left_Center:
        v = _owner->_x - (targetX + _targetData.z / 2);
        if (info.percent)
            v = v / _targetData.z * _target->_width;
        _owner->setX(targetX + _target->_width / 2 + v);
        break;
    case RelationType::Left_Right:
        v = _owner->_x - (targetX + _targetData.z);
        if (info.percent)
            v = v / _targetData.z * _target->_width;
        _owner->setX(targetX + _target->_width + v);
        break;
    case RelationType::Center_Center:
        v = _owner->_x + _owner->_rawWidth / 2 - (targetX + _targetData.z / 2);
        if (info.percent)
            v = v / _targetData.z * _target->_width;
        _owner->setX(targetX + _target->_width / 2 + v - _owner->_rawWidth / 2);
        break;
    case RelationType::Right_Left:
        v = _owner->_x + _owner->_rawWidth - targetX;
        if (info.percent)
            v = v / _targetData.z * _target->_width;
        _owner->setX(targetX + v - _owner->_rawWidth);
        break;
    case RelationType::Right_Center:
        v = _owner->_x + _owner->_rawWidth - (targetX + _targetData.z / 2);
        if (info.percent)
            v = v / _targetData.z * _target->_width;
        _owner->setX(targetX + _target->_width / 2 + v - _owner->_rawWidth);
        break;
    case RelationType::Right_Right:
        v = _owner->_x + _owner->_rawWidth - (targetX + _targetData.z);
        if (info.percent)
            v = v / _targetData.z * _target->_width;
        _owner->setX(targetX + _target->_width + v - _owner->_rawWidth);
        break;

    case RelationType::Top_Top:
        if (info.percent && _target == _owner->_parent)
        {
            v = _owner->_y - targetY;
            if (info.percent)
                v = v / _targetData.w * _target->_height;
            _owner->setY(targetY + v);
        }
        break;
    case RelationType::Top_Middle:
        v = _owner->_y - (targetY + _targetData.w / 2);
        if (info.percent)
            v = v / _targetData.w * _target->_height;
        _owner->setY(targetY + _target->_height / 2 + v);
        break;
    case RelationType::Top_Bottom:
        v = _owner->_y - (targetY + _targetData.w);
        if (info.percent)
            v = v / _targetData.w * _target->_height;
        _owner->setY(targetY + _target->_height + v);
        break;
    case RelationType::Middle_Middle:
        v = _owner->_y + _owner->_rawHeight / 2 - (targetY + _targetData.w / 2);
        if (info.percent)
            v = v / _targetData.w * _target->_height;
        _owner->setY(targetY + _target->_height / 2 + v - _owner->_rawHeight / 2);
        break;
    case RelationType::Bottom_Top:
        v = _owner->_y + _owner->_rawHeight - targetY;
        if (info.percent)
            v = v / _targetData.w * _target->_height;
        _owner->setY(targetY + v - _owner->_rawHeight);
        break;
    case RelationType::Bottom_Middle:
        v = _owner->_y + _owner->_rawHeight - (targetY + _targetData.w / 2);
        if (info.percent)
            v = v / _targetData.w * _target->_height;
        _owner->setY(targetY + _target->_height / 2 + v - _owner->_rawHeight);
        break;
    case RelationType::Bottom_Bottom:
        v = _owner->_y + _owner->_rawHeight - (targetY + _targetData.w);
        if (info.percent)
            v = v / _targetData.w * _target->_height;
        _owner->setY(targetY + _target->_height + v - _owner->_rawHeight);
        break;

    case RelationType::Width_Width:
        if (_owner->_underConstruct && _owner == _target->_parent)
            v = _owner->sourceWidth - _target->initWidth;
        else
            v = _owner->_rawWidth - _targetData.z;
        if (info.percent)
            v = v / _targetData.z * _target->_width;
        if (_target == _owner->_parent)
            _owner->setSize(_target->_width + v, _owner->_rawHeight, true);
        else
            _owner->setWidth(_target->_width + v);
        break;
    case RelationType::Height_Height:
        if (_owner->_underConstruct && _owner == _target->_parent)
            v = _owner->sourceHeight - _target->initHeight;
        else
            v = _owner->_rawHeight - _targetData.w;
        if (info.percent)
            v = v / _targetData.w * _target->_height;
        if (_target == _owner->_parent)
            _owner->setSize(_owner->_rawWidth, _target->_height + v, true);
        else
            _owner->setHeight(_target->_height + v);
        break;

    case RelationType::LeftExt_Left:
        break;
    case RelationType::LeftExt_Right:
        v = _owner->_x - (targetX + _targetData.z);
        if (info.percent)
            v = v / _targetData.z * _target->_width;
        tmp = _owner->_x;
        _owner->setX(targetX + _target->_width + v);
        _owner->setWidth(_owner->_rawWidth - (_owner->_x - tmp));
        break;
    case RelationType::RightExt_Left:
        break;
    case RelationType::RightExt_Right:
        if (_owner->_underConstruct && _owner == _target->_parent)
            v = _owner->sourceWidth - (targetX + _target->initWidth);
        else
            v = _owner->_rawWidth - (targetX + _targetData.z);
        if (_owner != _target->_parent)
            v += _owner->_x;
        if (info.percent)
            v = v / _targetData.z * _target->_width;
        if (_owner != _target->_parent)
            _owner->setWidth(targetX + _target->_width + v - _owner->_x);
        else
            _owner->setWidth(targetX + _target->_width + v);
        break;
    case RelationType::TopExt_Top:
        break;
    case RelationType::TopExt_Bottom:
        v = _owner->_y - (targetY + _targetData.w);
        if (info.percent)
            v = v / _targetData.w * _target->_height;
        tmp = _owner->_y;
        _owner->setY(targetY + _target->_height + v);
        _owner->setHeight(_owner->_rawHeight - (_owner->_y - tmp));
        break;
    case RelationType::BottomExt_Top:
        break;
    case RelationType::BottomExt_Bottom:
        if (_owner->_underConstruct && _owner == _target->_parent)
            v = _owner->sourceHeight - (targetY + _target->initHeight);
        else
            v = _owner->_rawHeight - (targetY + _targetData.w);
        if (_owner != _target->_parent)
            v += _owner->_y;
        if (info.percent)
            v = v / _targetData.w * _target->_height;
        if (_owner != _target->_parent)
            _owner->setHeight(targetY + _target->_height + v - _owner->_y);
        else
            _owner->setHeight(targetY + _target->_height + v);
        break;
    }
}

void RelationItem::addRefTarget(GObject* target)
{
    if (target != _owner->_parent)
        target->addEventListener(UIEventType::PositionChange, CC_CALLBACK_1(RelationItem::onTargetXYChanged, this), (uint32_t)this);
    target->addEventListener(UIEventType::SizeChange, CC_CALLBACK_1(RelationItem::onTargetSizeChanged, this), (uint32_t)this);
    _targetData.x = _target->_x;
    _targetData.y = _target->_y;
    _targetData.z = _target->_width;
    _targetData.w = _target->_height;
}

void RelationItem::releaseRefTarget(GObject* target)
{
    target->removeEventListener(UIEventType::PositionChange, (uint32_t)this);
    target->removeEventListener(UIEventType::SizeChange, (uint32_t)this);
}

void RelationItem::onTargetXYChanged(EventContext* context)
{
    if (_owner->relations()->handling != nullptr
        || _owner->_group != nullptr && _owner->_group->_updating != 0)
    {
        _targetData.x = _target->_x;
        _targetData.y = _target->_y;
        return;
    }

    _owner->relations()->handling = (GObject*)context->getSender();

    float ox = _owner->_x;
    float oy = _owner->_y;
    float dx = _target->_x - _targetData.x;
    float dy = _target->_y - _targetData.y;

    for (auto it = _defs.cbegin(); it != _defs.cend(); ++it)
        applyOnXYChanged(*it, dx, dy);

    _targetData.x = _target->_x;
    _targetData.y = _target->_y;

    if (ox != _owner->_x || oy != _owner->_y)
    {
        ox = _owner->_x - ox;
        oy = _owner->_y - oy;

        _owner->updateGearFromRelations(1, ox, oy);

        /*if (_owner->_parent != nullptr)
        {
            int transCount = _owner->parent._transitions.Count;
            for (int i = 0; i < transCount; i++)
                _owner->parent._transitions[i].UpdateFromRelations(_owner->id, ox, oy);
        }*/
    }

    _owner->relations()->handling = nullptr;
}

void RelationItem::onTargetSizeChanged(EventContext* context)
{
    if (_owner->relations()->handling != nullptr
        || _owner->_group != nullptr && _owner->_group->_updating != 0)
    {
        _targetData.z = _target->_width;
        _targetData.w = _target->_height;
        return;
    }

    _owner->relations()->handling = (GObject*)context->getSender();

    float ox = _owner->_x;
    float oy = _owner->_y;
    float ow = _owner->_rawWidth;
    float oh = _owner->_rawHeight;

    for (auto it = _defs.cbegin(); it != _defs.cend(); ++it)
        applyOnSizeChanged(*it);

    _targetData.z = _target->_width;
    _targetData.w = _target->_height;

    if (ox != _owner->_x || oy != _owner->_y)
    {
        ox = _owner->_x - ox;
        oy = _owner->_y - oy;

        _owner->updateGearFromRelations(1, ox, oy);

        /*if (_owner->_parent != nullptr)
        {
            int transCount = _owner->parent._transitions.Count;
            for (int i = 0; i < transCount; i++)
                _owner->parent._transitions[i].UpdateFromRelations(_owner->id, ox, oy);
        }*/
    }

    if (ow != _owner->_rawWidth || oh != _owner->_rawHeight)
    {
        ow = _owner->_rawWidth - ow;
        oh = _owner->_rawHeight - oh;

        _owner->updateGearFromRelations(2, ow, oh);
    }

    _owner->relations()->handling = nullptr;
}


NS_FGUI_END