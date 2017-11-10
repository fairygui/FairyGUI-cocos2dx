#ifndef __GTEXTINPUT_H__
#define __GTEXTINPUT_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GTextField.h"
#include "ui/UIEditBox/UIEditBox.h"

NS_FGUI_BEGIN

class GTextInput : public GTextField
{
public:
    GTextInput();
    ~GTextInput();

    CREATE_FUNC(GTextInput);

    const std::string& getText() const override { return  _text; }
    void setText(const std::string& value) override;

    const std::string getFontName() const override { return _input->getFontName(); }
    void setFontName(const std::string& value) override;

    int getFontSize() const override { return _input->getFontSize(); }
    void setFontSize(int value) override;

    virtual const cocos2d::Color3B& getColor() const override { return _color; }
    virtual void setColor(const cocos2d::Color3B& value) override;

protected:
    virtual void handleInit() override;
    virtual void handleSizeChanged() override;
    virtual void setup_BeforeAdd(TXMLElement* xml) override;

private:
    cocos2d::ui::EditBox* _input;
    std::string _text;
    cocos2d::Color3B _color;
};

NS_FGUI_END

#endif
