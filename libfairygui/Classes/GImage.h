#ifndef __GIMAGE_H__
#define __GIMAGE_H__

#include "cocos2d.h"
#include "ui/UIScale9Sprite.h"
#include "FairyGUIMacros.h"
#include "GObject.h"

NS_FGUI_BEGIN

class FUISprite;

class GImage : public GObject, public IColorGear
{
public:
    GImage();
    virtual ~GImage();

    CREATE_FUNC(GImage);

    FlipType getFlip() const;
    void setFlip(FlipType value);

    cocos2d::Color3B getColor() const override;
    void setColor(const cocos2d::Color3B& value) override;

    FillMethod getFillMethod() const;
    void setFillMethod(FillMethod value);

    FillOrigin getFillOrigin() const;
    void setFillOrigin(FillOrigin value);

    bool isFillClockwise() const;
    void setFillClockwise(bool value);

    float getFillAmount() const;
    void setFillAmount(float value);

    virtual void constructFromResource() override;

protected:
    virtual void handleInit() override;
    virtual void handleSizeChanged() override;
    virtual void handleGrayedChanged() override;
    virtual void setup_beforeAdd(ByteBuffer* buffer, int beginPos) override;

private:
    FUISprite* _content;
};

NS_FGUI_END

#endif
