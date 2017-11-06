#ifndef __FUILABEL_H__
#define __FUILABEL_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"

NS_FGUI_BEGIN

class FUILabel : public cocos2d::Label
{
public:
    FUILabel();
    ~FUILabel();

    CREATE_FUNC(FUILabel);

    const std::string& getFontName() const { return _fontName; }
    void setFontName(const std::string & value);

    int getFontSize() const { return _fontSize; }
    void setFontSize(int value);

    virtual void setTextColor(const cocos2d::Color4B &color) override;

    virtual bool setBMFontFilePath(const std::string& bmfontFilePath, const cocos2d::Vec2& imageOffset = cocos2d::Vec2::ZERO, float fontSize = 0) override;

protected:
    /*
    注意！！！如果这里出现了编译错误，需要修改cocos2d的源码，文件2d/CCLabel.h，大约在672行，为updateBMFontScale函数打上virtual修饰符。
    因为这个方法里有强制字体对象指针为FontFnt类型的代码，但我们不使用FontFnt（FontFnt只支持从外部文件中载入配置，更糟糕的是BMFontConfiguration是定义在cpp里的。）
    所以需要重写这个方法。
    */
    virtual void updateBMFontScale() override;

private:
    int _fontSize;
    std::string _fontName;
};

NS_FGUI_END

#endif
