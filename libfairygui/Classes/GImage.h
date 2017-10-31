#ifndef __GIMAGE_H__
#define __GIMAGE_H__

#include "cocos2d.h"
#include "ui/UIScale9Sprite.h"
#include "FairyGUIMacros.h"
#include "GObject.h"

NS_FGUI_BEGIN

class GImage : public GObject, public IColorGear
{
public:
    GImage();
    ~GImage();

    CREATE_FUNC(GImage);

    FlipType getFlip() const;
    void setFlip(FlipType value);

    const cocos2d::Color3B& getColor() const { return _content->getColor(); }
    void setColor(const cocos2d::Color3B& value);

    virtual void constructFromResource() override;

    cocos2d::Color4B cg_getColor() const override;
    void cg_setColor(const cocos2d::Color4B& value) override;

protected:
    virtual void handleInit() override;
    virtual void handleSizeChanged() override;
    virtual void setup_BeforeAdd(tinyxml2::XMLElement* xml) override;

private:
    cocos2d::Sprite* _content;
};

NS_FGUI_END

#endif
