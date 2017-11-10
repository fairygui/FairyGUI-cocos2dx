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
    virtual const std::string getFontName() const = 0;
    virtual void setFontName(const std::string& value) = 0;

    virtual int getFontSize() const = 0;
    virtual void setFontSize(int value) = 0;

    virtual const cocos2d::Color3B& getColor() const = 0;
    virtual void setColor(const cocos2d::Color3B& value) = 0;

    virtual bool isUBBEnabled() const { return false; }
    virtual void setUBBEnabled(bool value) {}

    virtual void setAutoSize(TextAutoSize value) {};
    virtual void setAlign(cocos2d::TextHAlignment value) {}
    virtual void setVerticalAlign(cocos2d::TextVAlignment value) {}
    virtual void setUnderline(bool value) {}
    virtual void setItalic(bool value) {}
    virtual void setBold(bool value) {}
    virtual void setSingleLine(bool value) {}
    virtual void setLetterSpacing(float value) {}
    virtual void setLineSpacing(float value) {}
    virtual void enableOutline(const cocos2d::Color4B& color, float size) {}
    virtual void enableShadow(const cocos2d::Color4B& color, const cocos2d::Size& offset) {};

    virtual cocos2d::Color4B cg_getColor() const override { return (cocos2d::Color4B)getColor(); }
    virtual void cg_setColor(const cocos2d::Color4B& value) override { setColor((cocos2d::Color3B)value); }

    cocos2d::Color4B cg_getOutlineColor() const override { return cocos2d::Color4B::BLACK; }
    void cg_setOutlineColor(const cocos2d::Color4B& value) override { }

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

    const std::string getFontName() const override { return _label->getFontName(); }
    void setFontName(const std::string& value) override;

    int getFontSize() const override { return _label->getFontSize(); }
    void setFontSize(int value) override;

    const cocos2d::Color3B& getColor() const override { return _color; }
    void setColor(const cocos2d::Color3B& value) override;

    virtual void setAutoSize(TextAutoSize value) override;
    virtual void setAlign(cocos2d::TextHAlignment value) override;
    virtual void setVerticalAlign(cocos2d::TextVAlignment value) override;
    virtual void setUnderline(bool value) override;
    virtual void setItalic(bool value) override;
    virtual void setBold(bool value) override;
    virtual void setSingleLine(bool value) override;
    virtual void setLetterSpacing(float value) override;
    virtual void setLineSpacing(float value) override;
    virtual void enableOutline(const cocos2d::Color4B& color, float size) override;
    virtual void enableShadow(const cocos2d::Color4B& color, const cocos2d::Size& offset) override;

    virtual cocos2d::Color4B cg_getOutlineColor() const override { return _outlineColor; }
    virtual void cg_setOutlineColor(const cocos2d::Color4B& value) override;

protected:
    virtual void handleInit() override;
    virtual void handleSizeChanged() override;
    virtual void setup_AfterAdd(TXMLElement* xml) override;

    void updateSize();

private:
    FUILabel* _label;

    TextAutoSize _autoSize;
    cocos2d::Color3B _color;
    cocos2d::Color4B _outlineColor;
    bool _updatingSize;
};

NS_FGUI_END

#endif
