#include "GGraph.h"
#include "utils/ByteBuffer.h"

NS_FGUI_BEGIN
USING_NS_CC;

GGraph::GGraph() :
    _shape(nullptr),
    _type(0),
    _lineSize(1),
    _lineColor(Color4F::BLACK),
    _fillColor(Color4F::WHITE)
{
    _touchDisabled = true;
}

GGraph::~GGraph()
{
}

void GGraph::handleInit()
{
    _shape = DrawNode::create();
    _shape->retain();

    _displayObject = _shape;
}

void GGraph::drawRect(float aWidth, float aHeight, int lineSize, const cocos2d::Color4F& lineColor, const cocos2d::Color4F& fillColor)
{
    _type = 0; //avoid updateshape call in handleSizeChange
    setSize(aWidth, aHeight);
    _type = 1;
    _lineSize = lineSize;
    _lineColor = lineColor;
    _fillColor = fillColor;
    updateShape();
}

void GGraph::drawEllipse(float aWidth, float aHeight, int lineSize, const cocos2d::Color4F& lineColor, const cocos2d::Color4F& fillColor)
{
    _type = 0; //avoid updateshape call in handleSizeChange
    setSize(aWidth, aHeight);
    _type = 2;
    _lineSize = lineSize;
    _lineColor = lineColor;
    _fillColor = fillColor;
    updateShape();
}

void GGraph::updateShape()
{
    _shape->clear();
    if (_type == 1)
    {
        if (_lineSize > 0)
        {
            _shape->setLineWidth(_lineSize);

            float tmp = floor(_lineSize*0.5f);
            float tmp2 = ceil(_lineSize*0.5f);
            _shape->drawLine(Vec2(1, tmp), Vec2(_size.width, tmp), _lineColor);
            _shape->drawLine(Vec2(_size.width - tmp, 0), Vec2(_size.width - tmp, _size.height), _lineColor);
            _shape->drawLine(Vec2(_size.width - 1, _size.height - tmp2), Vec2(0, _size.height - tmp2), _lineColor);
            _shape->drawLine(Vec2(tmp2, _size.height - 1), Vec2(tmp2, 0), _lineColor);
        }
        _shape->drawSolidRect(Vec2(_lineSize, _lineSize), Vec2(_size.width - _lineSize, _size.height - _lineSize), _fillColor);
        _touchDisabled = false;
    }
    else if (_type == 2)
    {
        if (_lineSize > 0)
        {
            _shape->setLineWidth(_lineSize);
            _shape->drawCircle(Vec2(_size.width / 2, _size.height / 2), _size.width / 2, 0, 360, false, 1, _size.height / _size.width, _lineColor);
        }
        _shape->drawSolidCircle(Vec2(_size.width / 2, _size.height / 2), _size.width / 2, 0, 360, 1, _size.height / _size.width, _fillColor);
        _touchDisabled = false;
    }
    else
        _touchDisabled = true;
}

cocos2d::Color3B GGraph::getColor() const
{
    return (Color3B)_fillColor;
}

void GGraph::setColor(const cocos2d::Color3B& value)
{
    _fillColor = Color4F(value, _fillColor.a);
    updateShape();
}

void GGraph::handleSizeChanged()
{
    GObject::handleSizeChanged();

    updateShape();
}

void GGraph::setup_beforeAdd(ByteBuffer* buffer, int beginPos)
{
    GObject::setup_beforeAdd(buffer, beginPos);

    buffer->Seek(beginPos, 5);

    _type = buffer->ReadByte();
    if (_type != 0)
    {
        _lineSize = buffer->ReadInt();
        _lineColor = (Color4F)buffer->ReadColor();
        _fillColor = (Color4F)buffer->ReadColor();

        updateShape();
    }
}

NS_FGUI_END