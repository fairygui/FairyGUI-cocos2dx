#ifndef __GPROGRESSBAR_H__
#define __GPROGRESSBAR_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GComponent.h"

NS_FGUI_BEGIN

enum ProgressTitleType
{
    TitleTypePercent,
    TitleTypeValueAndMax,
    TitleTypeValue,
    TitleTypeMax
};

class GProgressBar : public GComponent
{
public:

public:
    GProgressBar();
    ~GProgressBar();

    CREATE_FUNC(GProgressBar);

    ProgressTitleType getTitleType() { return _titleType; }
    void setTitleType(ProgressTitleType value);

    double getMax() { return _max; }
    void setMax(double value);

    double getValue() { return _value; }
    void setValue(double value);

    virtual void setup_AfterAdd(tinyxml2::XMLElement* xml) override;

protected:
    virtual bool init() override;
    virtual void constructFromXML(tinyxml2::XMLElement* xml) override;
    virtual void handleSizeChanged() override;

    void update();

private:
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
    float _barStartX;
    float _barStartY;
};

NS_FGUI_END

#endif // __GPROGRESSBAR_H__
