#ifndef __GTEXTINPUT_H__
#define __GTEXTINPUT_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GTextField.h"
#include "display/FUIInput.h"

NS_FGUI_BEGIN

class GTextInput : public GTextField
{
public:
    GTextInput();
    virtual ~GTextInput();

    CREATE_FUNC(GTextInput);

    const std::string& getText() const override { return _input->getText(); }
    void setText(const std::string& value) override { _input->setText(value); }

    virtual bool isSingleLine() const override;
    virtual void setSingleLine(bool value) override;

    virtual TextFormat* getTextFormat() const override { return _input->getTextFormat(); }
    virtual void applyTextFormat() override;

    void setPrompt(const std::string& value);
    void setPassword(bool value);
    void setKeyboardType(int value);
    void setMaxLength(int value);
    void setRestrict(const std::string& value);

protected:
    virtual void handleInit() override;
    virtual void handleSizeChanged() override;
    virtual void setup_BeforeAdd(TXMLElement* xml) override;

private:
    FUIInput* _input;
};

NS_FGUI_END

#endif
