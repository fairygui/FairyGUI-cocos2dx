#ifndef __GSLIDER_H__
#define __GSLIDER_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GProgressBar.h"
#include "GTextField.h"

NS_FGUI_BEGIN

class GSlider : public GComponent
{
public:
    GSlider();
    ~GSlider();

    CREATE_FUNC(GSlider);

    ProgressTitleType getTitleType() { return _titleType; }
    void setTitleType(ProgressTitleType value);

    double getMax() { return _max; }
    void setMax(double value);

    double getValue() { return _value; }
    void setValue(double value);

    virtual void setup_AfterAdd(tinyxml2::XMLElement* xml) override;

    bool changeOnClick;
    bool canDrag;

protected:
    virtual bool init() override;
    virtual void constructFromXML(tinyxml2::XMLElement* xml) override;
    virtual void handleSizeChanged() override;

    void update();
    void updateWidthPercent(float percent);

private:
    void onTouchBegin(EventContext* context);
    void onGripTouchBegin(EventContext* context);
    void onGripTouchMove(EventContext* context);

    double _max;
    double _value;
    ProgressTitleType _titleType;
    bool _reverse;

    GObject* _titleObject;
    GObject* _barObjectH;
    GObject* _barObjectV;
    float _barMaxWidth;
    float _barMaxHeight;
    float _barMaxWidthDelta;
    float _barMaxHeightDelta;
    GObject* _gripObject;
    cocos2d::Vec2 _clickPos;
    float _clickPercent;
    float _barStartX;
    float _barStartY;
};

NS_FGUI_END

#endif // __GSLIDER_H__
