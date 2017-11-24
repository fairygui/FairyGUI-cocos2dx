#ifndef __GTEXTFIELD_H__
#define __GTEXTFIELD_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GObject.h"
#include "display/FUILabel.h"

NS_FGUI_BEGIN

class GTextField : public GObject, public IColorGear
{
public:
    virtual bool isUBBEnabled() const { return false; }
    virtual void setUBBEnabled(bool value) {}

    virtual TextAutoSize getAutoSize() const { return TextAutoSize::NONE; }
    virtual void setAutoSize(TextAutoSize value) {};

    virtual bool isSingleLine() const { return false; }
    virtual void setSingleLine(bool value) {};

    virtual TextFormat* getTextFormat() const = 0;
    virtual void applyTextFormat() = 0;

    virtual const cocos2d::Size& getTextSize() { return _displayObject->getContentSize(); }

    const cocos2d::Color3B& getColor() const { return getTextFormat()->color; }
    void setColor(const cocos2d::Color3B& value);

    float getFontSize() const { return getTextFormat()->fontSize; }
    void setFontSize(float value);

    cocos2d::Color4B cg_getColor() const override { return (cocos2d::Color4B)getTextFormat()->color; }
    void cg_setColor(const cocos2d::Color4B& value) override;

    cocos2d::Color4B cg_getOutlineColor() const override { return (cocos2d::Color4B)getTextFormat()->outlineColor; }
    void cg_setOutlineColor(const cocos2d::Color4B& value) override;

protected:
    virtual void setup_BeforeAdd(TXMLElement* xml) override;
    virtual void setup_AfterAdd(TXMLElement* xml) override;
};

class GBasicTextField : public GTextField
{
public:
    GBasicTextField();
    ~GBasicTextField();

    CREATE_FUNC(GBasicTextField);

    const std::string& getText() const override { return _label->getString(); }
    void setText(const std::string& value) override;

    virtual TextAutoSize getAutoSize() const override { return _autoSize; }
    virtual void setAutoSize(TextAutoSize value) override;

    virtual bool isSingleLine() const override { return _label->isWrapEnabled(); }
    virtual void setSingleLine(bool value) override;

    virtual TextFormat* getTextFormat() const override { return _label->getTextFormat(); }
    virtual void applyTextFormat() override;

protected:
    virtual void handleInit() override;
    virtual void handleSizeChanged() override;
    virtual void handleGrayedChanged() override;
    virtual void setup_AfterAdd(TXMLElement* xml) override;

    void updateSize();

private:
    FUILabel* _label;
    TextAutoSize _autoSize;
    bool _updatingSize;
};

NS_FGUI_END

#endif
