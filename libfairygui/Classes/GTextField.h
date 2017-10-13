#ifndef __GTEXTFIELD_H__
#define __GTEXTFIELD_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GObject.h"

NS_FGUI_BEGIN

class GTextField : public GObject
{
public:
    GTextField();
    ~GTextField();

    CREATE_FUNC(GTextField);

    void setText(const std::string& text) override;
    const std::string& getText() override;

    virtual void handleSizeChanged() override;
    virtual void setup_BeforeAdd(tinyxml2::XMLElement* xml) override;
    virtual void setup_AfterAdd(tinyxml2::XMLElement* xml) override;

protected:
    virtual bool init() override;

private:
    cocos2d::Label* _label;
    int _fontSize;
    std::string _font;
};

NS_FGUI_END

#endif // __GTEXTFIELD_H__
