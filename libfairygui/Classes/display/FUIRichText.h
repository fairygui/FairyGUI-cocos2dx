#ifndef __FUIRICHTEXT_H__
#define __FUIRICHTEXT_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "ui/UIRichText.h"

NS_FGUI_BEGIN

class FUIRichText : public cocos2d::ui::RichText
{
public:
    FUIRichText();
    ~FUIRichText();

    CREATE_FUNC(FUIRichText);

    const std::string& getText() const { return _text; }
    void setText(const std::string& value);

    bool isUBBEnabled() const { return _ubbEnabled; }
    void setUBBEnabled(bool value) { _ubbEnabled = value; }

protected:
    virtual bool init() override;

private:
    std::string _text;
    bool _ubbEnabled;
};

NS_FGUI_END

#endif
