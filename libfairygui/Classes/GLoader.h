#ifndef __GLOADER_H__
#define __GLOADER_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GObject.h"

NS_FGUI_BEGIN

class GLoader : public GObject
{
public:
    enum FillType
    {
        None,
        Scale,
        ScaleMatchHeight,
        ScaleMatchWidth,
        ScaleFree
    };

    GLoader();
    ~GLoader();

    CREATE_FUNC(GLoader);

    std::string getURL() { return _url; }
    void setURL(const std::string& value);

    virtual const std::string& getIcon() override { return _url; }
    virtual void setIcon(const std::string& value) override { setURL(value); }

    virtual void setup_BeforeAdd(tinyxml2::XMLElement* xml) override;
protected:
    virtual bool init() override;
    virtual void handleSizeChanged() override;

    virtual void loadExternal();
    virtual void freeExternal(cocos2d::Texture2D* texture);

    void setErrorState();
private:
    void loadContent();
    void loadFromPackage();
    void clearContent();
    void updateLayout();
    void clearErrorState();

    std::string _url;
    cocos2d::TextHAlignment _align;
    cocos2d::TextVAlignment _verticalAlign;
    bool _autoSize;
    FillType _fill;
    bool _updatingLayout;
    PackageItem* _contentItem;
    float _contentWidth;
    float _contentHeight;
    float _contentSourceWidth;
    float _contentSourceHeight;
    int _contentStatus;

    cocos2d::Sprite* _content;
    cocos2d::Action* _playAction;
};

NS_FGUI_END

#endif // __GLOADER_H__
