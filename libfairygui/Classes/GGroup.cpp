#include "GGroup.h"
#include "GComponent.h"
#include "utils/ByteBuffer.h"

NS_FGUI_BEGIN
USING_NS_CC;

GGroup::GGroup() :
    _layout(GroupLayoutType::NONE),
    _lineGap(0),
    _columnGap(0),
    _percentReady(false),
    _boundsChanged(false),
    _updating(false)
{
    _touchDisabled = true;
}

GGroup::~GGroup()
{
    CALL_LATER_CANCEL(GGroup, ensureBoundsCorrect);
}

void GGroup::setLayout(GroupLayoutType value)
{
    if (_layout != value)
    {
        _layout = value;
        setBoundsChangedFlag(true);
    }
}

void GGroup::setColumnGap(int value)
{
    if (_columnGap != value)
    {
        _columnGap = value;
        setBoundsChangedFlag();
    }
}

void GGroup::setLineGap(int value)
{
    if (_lineGap != value)
    {
        _lineGap = value;
        setBoundsChangedFlag();
    }
}

void GGroup::setBoundsChangedFlag(bool childSizeChanged)
{
    if (_updating == 0 && _parent != nullptr)
    {
        if (childSizeChanged)
            _percentReady = false;

        if (!_boundsChanged)
        {
            _boundsChanged = true;

            if (_layout != GroupLayoutType::NONE)
                CALL_LATER(GGroup, ensureBoundsCorrect);
        }
    }
}

void GGroup::moveChildren(float dx, float dy)
{
    if ((_updating & 1) != 0 || _parent == nullptr)
        return;

    _updating |= 1;

    int cnt = _parent->numChildren();
    int i;
    GObject* child;
    for (i = 0; i < cnt; i++)
    {
        child = _parent->getChildAt(i);
        if (child->_group == this)
        {
            child->setPosition(child->getX() + dx, child->getY() + dy);
        }
    }

    _updating &= 2;
}

void GGroup::resizeChildren(float dw, float dh)
{
    if (_layout == GroupLayoutType::NONE || (_updating & 2) != 0 || _parent == nullptr)
        return;

    _updating |= 2;

    if (!_percentReady)
        updatePercent();

    int cnt = _parent->numChildren();
    int i;
    int j;
    GObject* child;
    int last = -1;
    int numChildren = 0;
    float lineSize = 0;
    float remainSize = 0;
    bool found = false;

    for (i = 0; i < cnt; i++)
    {
        child = _parent->getChildAt(i);
        if (child->_group != this)
            continue;

        last = i;
        numChildren++;
    }

    if (_layout == GroupLayoutType::HORIZONTAL)
    {
        remainSize = lineSize = _size.width - (numChildren - 1) * _columnGap;
        float curX = 0;
        bool started = false;
        float nw;
        for (i = 0; i < cnt; i++)
        {
            child = _parent->getChildAt(i);
            if (child->_group != this)
                continue;

            if (!started)
            {
                started = true;
                curX = (int)child->getX();
            }
            else
                child->setX(curX);
            if (last == i)
                nw = remainSize;
            else
                nw = round(child->_sizePercentInGroup * lineSize);
            child->setSize(nw, child->_rawSize.height + dh, true);
            remainSize -= child->getWidth();
            if (last == i)
            {
                if (remainSize >= 1)
                {
                    for (j = 0; j <= i; j++)
                    {
                        child = _parent->getChildAt(j);
                        if (child->_group != this)
                            continue;

                        if (!found)
                        {
                            nw = child->getWidth() + remainSize;
                            if ((child->maxSize.width == 0 || nw < child->maxSize.width)
                                && (child->minSize.width == 0 || nw > child->minSize.width))
                            {
                                child->setSize(nw, child->getHeight(), true);
                                found = true;
                            }
                        }
                        else
                            child->setX(child->getX() + remainSize);
                    }
                }
            }
            else
                curX += (child->getWidth() + _columnGap);
        }
    }
    else if (_layout == GroupLayoutType::VERTICAL)
    {
        remainSize = lineSize = _size.height - (numChildren - 1) * _lineGap;
        bool started = false;
        float curY = 0;
        float nh;
        for (i = 0; i < cnt; i++)
        {
            child = _parent->getChildAt(i);
            if (child->_group != this)
                continue;

            if (!started)
            {
                started = true;
                curY = (int)child->getY();
            }
            else
                child->setY(curY);
            if (last == i)
                nh = remainSize;
            else
                nh = round(child->_sizePercentInGroup * lineSize);
            child->setSize(child->_rawSize.width + dw, nh, true);
            remainSize -= child->getHeight();
            if (last == i)
            {
                if (remainSize >= 1)
                {
                    for (j = 0; j <= i; j++)
                    {
                        child = _parent->getChildAt(j);
                        if (child->_group != this)
                            continue;

                        if (!found)
                        {
                            nh = child->getHeight() + remainSize;
                            if ((child->maxSize.height == 0 || nh < child->maxSize.height)
                                && (child->minSize.height == 0 || nh > child->minSize.height))
                            {
                                child->setSize(child->getWidth(), nh, true);
                                found = true;
                            }
                        }
                        else
                            child->setY(child->getY() + remainSize);
                    }
                }
            }
            else
                curY += (child->getHeight() + _lineGap);
        }
    }

    _updating &= 1;
}

void GGroup::ensureBoundsCorrect()
{
    if (_boundsChanged)
        updateBounds();
}

void GGroup::updateBounds()
{
    _boundsChanged = false;

    if (_parent == nullptr)
        return;

    handleLayout();

    int cnt = _parent->numChildren();
    int i;
    GObject* child;
    float ax = FLT_MAX, ay = FLT_MAX;
    float ar = FLT_MIN, ab = FLT_MIN;
    float tmp;
    bool empty = true;

    for (i = 0; i < cnt; i++)
    {
        child = _parent->getChildAt(i);
        if (child->_group != this)
            continue;

        tmp = child->getX();
        if (tmp < ax)
            ax = tmp;
        tmp = child->getY();
        if (tmp < ay)
            ay = tmp;
        tmp = child->getX() + child->getWidth();
        if (tmp > ar)
            ar = tmp;
        tmp = child->getY() + child->getHeight();
        if (tmp > ab)
            ab = tmp;

        empty = false;
    }

    if (!empty)
    {
        _updating = 1;
        setPosition(ax, ay);
        _updating = 2;
        setSize(ar - ax, ab - ay);
    }
    else
    {
        _updating = 2;
        setSize(0, 0);
    }

    _updating = 0;
}

void GGroup::handleLayout()
{
    _updating |= 1;

    if (_layout == GroupLayoutType::HORIZONTAL)
    {
        float curX = 0;
        bool started = false;
        int cnt = _parent->numChildren();
        for (int i = 0; i < cnt; i++)
        {
            GObject* child = _parent->getChildAt(i);
            if (child->_group != this)
                continue;

            if (!started)
            {
                started = true;
                curX = (int)child->getX();
            }
            else
                child->setX(curX);
            if (child->getWidth() != 0)
                curX += (int)(child->getWidth() + _columnGap);
        }
        if (!_percentReady)
            updatePercent();
    }
    else if (_layout == GroupLayoutType::VERTICAL)
    {
        float curY = 0;
        bool started = false;
        int cnt = _parent->numChildren();
        for (int i = 0; i < cnt; i++)
        {
            GObject* child = _parent->getChildAt(i);
            if (child->_group != this)
                continue;

            if (!started)
            {
                started = true;
                curY = (int)child->getY();
            }
            else
                child->setY(curY);
            if (child->getHeight() != 0)
                curY += (int)(child->getHeight() + _lineGap);
        }
        if (!_percentReady)
            updatePercent();
    }

    _updating &= 2;
}

void GGroup::updatePercent()
{
    _percentReady = true;

    int cnt = _parent->numChildren();
    int i;
    GObject* child;
    float size = 0;
    if (_layout == GroupLayoutType::HORIZONTAL)
    {
        for (i = 0; i < cnt; i++)
        {
            child = _parent->getChildAt(i);
            if (child->_group != this)
                continue;

            size += child->getWidth();
        }

        for (i = 0; i < cnt; i++)
        {
            child = _parent->getChildAt(i);
            if (child->_group != this)
                continue;

            if (size > 0)
                child->_sizePercentInGroup = child->getWidth() / size;
            else
                child->_sizePercentInGroup = 0;
        }
    }
    else
    {
        for (i = 0; i < cnt; i++)
        {
            child = _parent->getChildAt(i);
            if (child->_group != this)
                continue;

            size += child->getHeight();
        }

        for (i = 0; i < cnt; i++)
        {
            child = _parent->getChildAt(i);
            if (child->_group != this)
                continue;

            if (size > 0)
                child->_sizePercentInGroup = child->getHeight() / size;
            else
                child->_sizePercentInGroup = 0;
        }
    }
}

void GGroup::handleAlphaChanged()
{
    GObject::handleAlphaChanged();

    if (_underConstruct)
        return;

    int cnt = _parent->numChildren();
    for (int i = 0; i < cnt; i++)
    {
        GObject* child = _parent->getChildAt(i);
        if (child->_group == this)
            child->setAlpha(_alpha);
    }
}

void GGroup::handleVisibleChanged()
{
    if (!_parent)
        return;

    int cnt = _parent->numChildren();
    for (int i = 0; i < cnt; i++)
    {
        GObject* child = _parent->getChildAt(i);
        if (child->_group == this)
            child->handleVisibleChanged();
    }
}

void GGroup::setup_beforeAdd(ByteBuffer* buffer, int beginPos)
{
    GObject::setup_beforeAdd(buffer, beginPos);

    buffer->Seek(beginPos, 5);

    _layout = (GroupLayoutType)buffer->ReadByte();
    _lineGap = buffer->ReadInt();
    _columnGap = buffer->ReadInt();
}

void GGroup::setup_afterAdd(ByteBuffer* buffer, int beginPos)
{
    GObject::setup_afterAdd(buffer, beginPos);

    if (!_visible)
        handleVisibleChanged();
}

NS_FGUI_END