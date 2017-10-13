#include "GGraph.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
USING_NS_CC;
using namespace tinyxml2;

GGraph::GGraph() :
    _shape(nullptr),
    _type(0),
    _lineSize(1),
    _lineColor(Color4F::BLACK),
    _fillColor(Color4F::WHITE)
{
}

GGraph::~GGraph()
{
}

void GGraph::drawRect(float aWidth, float aHeight, int lineSize, const cocos2d::Color4F& lineColor, const cocos2d::Color4F& fillColor)
{
    setSize(aWidth, aHeight);
    _shape->clear();
    if (lineSize > 0)
    {
        _shape->setLineWidth(lineSize);
        _shape->drawRect(Vec2::ZERO, Vec2(aWidth, aHeight), lineColor);
    }
    _shape->drawSolidRect(Vec2::ZERO, Vec2(aWidth, aHeight), fillColor);
}

void GGraph::drawEllipse(float aWidth, float aHeight, int lineSize, const cocos2d::Color4F& lineColor, const cocos2d::Color4F& fillColor)
{
    setSize(aWidth, aHeight);
    _shape->clear();
    if (lineSize > 0)
    {
        _shape->setLineWidth(lineSize);
        _shape->drawCircle(Vec2(aWidth / 2, aHeight / 2), aWidth / 2, 0, 360, false, 1, aHeight / aWidth, lineColor);
    }
    _shape->drawSolidCircle(Vec2(aWidth / 2, aHeight / 2), aWidth / 2, 0, 360, 1, aHeight / aWidth, fillColor);
}

void GGraph::setup_BeforeAdd(tinyxml2::XMLElement * xml)
{
    GObject::setup_BeforeAdd(xml);

    const char* p = xml->Attribute("type");
    if (p)
    {
        if (strcmp(p, "rect") == 0)
            _type = 1;
        else if (strcmp(p, "eclipse") == 0)
            _type = 2;
    }

    if (_type != 0)
    {
        p = xml->Attribute("lineSize");
        if (p)
            _lineSize = atoi(p);

        p = xml->Attribute("lineColor");
        if (p)
            _lineColor = (Color4F)ToolSet::convertFromHtmlColor(p);

        p = xml->Attribute("fillColor");
        if (p)
            _fillColor = (Color4F)ToolSet::convertFromHtmlColor(p);

        updateShape();
    }
}

void GGraph::updateShape()
{
    if (_type == 1)
        drawRect(this->getWidth(), this->getHeight(), _lineSize, _lineColor, _fillColor);
    else if (_type == 2)
        drawEllipse(this->getWidth(), this->getHeight(), _lineSize, _lineColor, _fillColor);
}

bool GGraph::init()
{
    _shape = DrawNode::create();
    _shape->retain();
    _shape->setAnchorPoint(Vec2(0, 1));
    _displayObject = _shape;

    return true;
}

void GGraph::handleSizeChanged()
{
    GObject::handleSizeChanged();

    updateShape();
}

NS_FGUI_END