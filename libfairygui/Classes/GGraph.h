#ifndef __GGRAPH_H__
#define __GGRAPH_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GObject.h"

NS_FGUI_BEGIN

class GGraph : public GObject, public IColorGear
{
public:
    GGraph();
    virtual ~GGraph();

    CREATE_FUNC(GGraph);

    void drawRect(float aWidth, float aHeight, int lineSize, const cocos2d::Color4F& lineColor, const cocos2d::Color4F& fillColor);
    void drawEllipse(float aWidth, float aHeight, int lineSize, const cocos2d::Color4F& lineColor, const cocos2d::Color4F& fillColor);
    bool isEmpty() const { return _type == 0; }

    cocos2d::Color3B getColor() const override;
    void setColor(const cocos2d::Color3B& value) override;

protected:
    virtual void handleInit() override;
    virtual void setup_BeforeAdd(TXMLElement* xml) override;
    virtual void handleSizeChanged() override;

private:
    void updateShape();

    int _type;
    cocos2d::Color4F _lineColor;
    cocos2d::Color4F _fillColor;
    int _lineSize;
    cocos2d::DrawNode *_shape;
};

NS_FGUI_END

#endif
