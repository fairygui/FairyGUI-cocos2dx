#ifndef __GRICHTEXTFIELD_H__
#define __GRICHTEXTFIELD_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GTextField.h"
#include "display/FUIRichText.h"

NS_FGUI_BEGIN

class GRichTextField : public GTextFieldDelegate
{
public:
    GRichTextField();
    ~GRichTextField();

    CREATE_FUNC(GRichTextField);

    const std::string& getText() const override { return _richText->getText(); }
    void setText(const std::string& value) override;

    const std::string getFontName() const override { return _richText->getFontFace(); }
    void setFontName(const std::string& value) override;

    int getFontSize() const override { return _richText->getFontSize(); }
    void setFontSize(int value) override;

    const cocos2d::Color3B& getColor() const override { return _color; }
    void setColor(const cocos2d::Color3B& value) override;

    void setAlign(cocos2d::TextHAlignment value);

protected:
    virtual void handleInit() override;
    virtual void handleSizeChanged() override;

private:
    FUIRichText* _richText;
    cocos2d::Color3B _color;
};

NS_FGUI_END

#endif
