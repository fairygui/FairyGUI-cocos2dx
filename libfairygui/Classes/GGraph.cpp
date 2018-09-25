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

static void drawVertRect(cocos2d::DrawNode* shape, float x, float y, float width, float height, const cocos2d::Color4F &color)
{
    float mx = x + width;
    float my = y + height;
    shape->drawTriangle(Vec2(x, y), Vec2(mx, y), Vec2(x, my), color);
    shape->drawTriangle(Vec2(mx, y), Vec2(mx, my), Vec2(x, my), color);
}

void GGraph::updateShape()
{
    _shape->clear();
    if (_type == 1)
    {
        if (_lineSize > 0)
        {
            float wl = _size.width - _lineSize;
            float hl = _size.height - _lineSize;
            drawVertRect(_shape, 0, 0, wl, _lineSize, _lineColor);
            drawVertRect(_shape, wl, 0, _lineSize, hl, _lineColor);
            drawVertRect(_shape, _lineSize, hl, wl, _lineSize, _lineColor);
            drawVertRect(_shape, 0, _lineSize, _lineSize, hl, _lineColor);

            drawVertRect(_shape, _lineSize, _lineSize, _size.width - _lineSize * 2, _size.height - _lineSize * 2, _fillColor);
        }
        else
            drawVertRect(_shape, 0, 0, _size.width, _size.height, _fillColor);

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