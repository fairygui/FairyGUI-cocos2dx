#ifndef __GBUTTON_H
#define __GBUTTON_H

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GComponent.h"

NS_FGUI_BEGIN

class GButton : public GComponent
{
public:
    enum ButtonMode
    {
        Common,
        Check,
        Radio
    };

    const std::string UP = "up";
    const std::string DOWN = "down";
    const std::string OVER = "over";
    const std::string SELECTED_OVER = "selectedOver";
    const std::string DISABLED = "disabled";
    const std::string SELECTED_DISABLED = "selectedDisabled";

    GButton();
    ~GButton();

    CREATE_FUNC(GButton);

    const std::string& getTitle() { return _title; }
    void setTitle(const std::string& value);

    virtual const std::string& getText() override { return _title; }
    virtual void setText(const std::string& value) override { setTitle(value); }

    virtual const std::string& getIcon() override { return _icon; }
    virtual void setIcon(const std::string& value) override;

    bool isSelected() { return _selected; }
    void setSelected(bool value);

    Controller* getRelatedController() { return _relatedController; }
    void setRelatedController(Controller* c);

    virtual void constructFromXML(tinyxml2::XMLElement* xml) override;
    virtual void setup_AfterAdd(tinyxml2::XMLElement* xml) override;

    bool changeStateOnClick;

protected:
    virtual bool init() override;
    virtual void handleControllerChanged(Controller* c) override;

    void setState(const std::string& value);
    void setCurrentState();

private:
    void onRollOver(EventContext* context);
    void onRollOut(EventContext* context);
    void onTouchBegin(EventContext* context);
    void onTouchEnd(EventContext* context);
    void onClick(EventContext* context);
    void onRemoved();

    ButtonMode _mode;
    GObject* _titleObject;
    GObject* _iconObject;
    Controller* _buttonController;
    Controller* _relatedController;
    std::string _relatedPageId;
    std::string _title;
    std::string _selectedTitle;
    std::string _icon;
    std::string _selectedIcon;
    bool _selected;
    bool _over;
    bool _down;
    int _downEffect;
    bool _downScaled;
    float _downEffectValue;
};

NS_FGUI_END

#endif // __GBUTTON_H
