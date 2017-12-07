#ifndef __GRICHTEXTFIELD_H__
#define __GRICHTEXTFIELD_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GTextField.h"
#include "display/FUIRichText.h"

NS_FGUI_BEGIN

class GRichTextField : public GTextField
{
public:
    GRichTextField();
    virtual ~GRichTextField();

    CREATE_FUNC(GRichTextField);

    const std::string& getText() const override { return _richText->getText(); }
    void setText(const std::string& value) override;

    virtual bool isUBBEnabled() const override { return _richText->isUBBEnabled(); }
    virtual void setUBBEnabled(bool value) override { _richText->setUBBEnabled(value); }

    virtual TextAutoSize getAutoSize() const override { return _autoSize; }
    virtual void setAutoSize(TextAutoSize value) override;

    virtual bool isSingleLine() const override { return false; }
    virtual void setSingleLine(bool value) override;

    virtual TextFormat* getTextFormat() const override { return _richText->getTextFormat(); }
    virtual void applyTextFormat() override;

protected:
    virtual void handleInit() override;
    virtual void handleSizeChanged() override;
    virtual void setup_AfterAdd(TXMLElement* xml) override;

    void updateSize();

private:
    FUIRichText* _richText;
    TextAutoSize _autoSize;
    bool _updatingSize;
};

NS_FGUI_END

#endif
