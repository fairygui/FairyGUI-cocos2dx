#ifndef __FUISPRITE_H__
#define __FUISPRITE_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"

NS_FGUI_BEGIN

class FUISprite : public cocos2d::Sprite
{
public:
    FUISprite();
    virtual ~FUISprite();

    CREATE_FUNC(FUISprite);

    void clearContent();
    void setScale9Grid(cocos2d::Rect* value);
    void setGrayed(bool value);

    FillMethod getFillMethod() const { return _fillMethod; }
    void setFillMethod(FillMethod value);

    FillOrigin getFillOrigin() const { return _fillOrigin; }
    void setFillOrigin(FillOrigin value);

    bool isFillClockwise() const { return _fillClockwise; }
    void setFillClockwise(bool value);

    float getFillAmount() const { return _fillAmount; }
    void setFillAmount(float value);

    bool isScaleByTile() const { return _scaleByTile; }
    void setScaleByTile(bool value);

    virtual void setContentSize(const cocos2d::Size& size) override;

protected:
    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;

    void onDraw(const cocos2d::Mat4 &transform, uint32_t flags);

    cocos2d::Tex2F textureCoordFromAlphaPoint(cocos2d::Vec2 alpha);
    cocos2d::Vec2 vertexFromAlphaPoint(cocos2d::Vec2 alpha);
    void updateBar(void);
    void updateRadial(void);
    virtual void updateColor(void) override;
    cocos2d::Vec2 boundaryTexCoord(char index);

    void setupFill();

private:
    FillMethod _fillMethod;
    FillOrigin _fillOrigin;
    float _fillAmount;
    bool _fillClockwise;
    bool _scaleByTile;
    cocos2d::CustomCommand _customCommand;
    int _vertexDataCount;
    cocos2d::V2F_C4B_T2F *_vertexData;
    cocos2d::GLProgramState *_fillGlProgramState;
    
    static cocos2d::Texture2D* _empty;
};

NS_FGUI_END

#endif
