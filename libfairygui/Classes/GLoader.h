#ifndef __GLOADER_H__
#define __GLOADER_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GObject.h"
#include "display/Actions.h"

NS_FGUI_BEGIN

class GLoader : public GObject, public IColorGear, public IAnimationGear
{
public:
    GLoader();
    virtual ~GLoader();

    CREATE_FUNC(GLoader);

    const std::string& getURL() const { return _url; }
    void setURL(const std::string& value);

    virtual const std::string& getIcon() const override { return _url; }
    virtual void setIcon(const std::string& value) override { setURL(value); }

    cocos2d::TextHAlignment getAlign() const { return _align; }
    void setAlign(cocos2d::TextHAlignment value);

    cocos2d::TextVAlignment getVerticalAlign() const { return _verticalAlign; }
    void setVerticalAlign(cocos2d::TextVAlignment value);

    bool getAutoSize() const { return _autoSize; }
    void setAutoSize(bool value);

    LoaderFillType getFill() const { return _fill; }
    void setFill(LoaderFillType value);

    const cocos2d::Size& getContentSize() { return _contentSize; }

    const cocos2d::Color3B& getColor() const { return _content->getColor(); }
    void setColor(const cocos2d::Color3B& value);

    bool isPlaying() const override { return _playing; }
    void setPlaying(bool value) override;

    int getCurrentFrame() const override { return _playAction->getCurrentFrame(); }
    void setCurrentFrame(int value) override;

    cocos2d::Color4B cg_getColor() const override;
    void cg_setColor(const cocos2d::Color4B& value) override;

protected:
    virtual void handleInit() override;
    virtual void handleSizeChanged() override;
    virtual void handleGrayedChanged() override;
    virtual void setup_BeforeAdd(TXMLElement* xml) override;

    virtual void loadExternal();
    virtual void freeExternal(cocos2d::SpriteFrame* spriteFrame);
    void onExternalLoadSuccess(cocos2d::SpriteFrame* spriteFrame);
    void onExternalLoadFailed();

private:
    void loadContent();
    void loadFromPackage();
    void clearContent();
    void updateLayout();
    void setErrorState();
    void clearErrorState();

    std::string _url;
    cocos2d::TextHAlignment _align;
    cocos2d::TextVAlignment _verticalAlign;
    bool _autoSize;
    LoaderFillType _fill;
    bool _updatingLayout;
    PackageItem* _contentItem;
    cocos2d::Size _contentSize;
    cocos2d::Size _contentSourceSize;
    int _contentStatus;
    bool _playing;
    int _frame;

    cocos2d::Sprite* _content;
    ActionMovieClip* _playAction;
};

NS_FGUI_END

#endif
