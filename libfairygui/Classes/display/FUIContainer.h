#ifndef __FUICONTAINER_H__
#define __FUICONTAINER_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"

NS_FGUI_BEGIN

class RectClippingSupport
{
public:
    RectClippingSupport()
        :_clippingEnabled(false)
    {}

    cocos2d::Rect _clippingRegion;
    bool _clippingEnabled;

    cocos2d::CustomCommand _beforeVisitCmdScissor;
    cocos2d::CustomCommand _afterVisitCmdScissor;
};

class StencilClippingSupport
{
public:
    StencilClippingSupport();

    cocos2d::Node* _stencil;
    cocos2d::GLProgram* _originStencilProgram;
    cocos2d::StencilStateManager* _stencilStateManager;
    cocos2d::GroupCommand _groupCommand;
    cocos2d::CustomCommand _beforeVisitCmd;
    cocos2d::CustomCommand _afterDrawStencilCmd;
    cocos2d::CustomCommand _afterVisitCmd;
};

class FUIContainer : public cocos2d::Node
{
public:
    FUIContainer();
    ~FUIContainer();

    CREATE_FUNC(FUIContainer);

    bool isClippingEnabled() const;
    void setClippingEnabled(bool value);
    const cocos2d::Rect& getClippingRegion() const;
    void setClippingRegion(const cocos2d::Rect& clippingRegion);

    cocos2d::Node* getStencil() const;
    void setStencil(cocos2d::Node* stencil);
    GLfloat getAlphaThreshold() const;
    void setAlphaThreshold(GLfloat alphaThreshold);
    bool isInverted() const;
    void setInverted(bool inverted);

    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExitTransitionDidStart() override;
    void onExit() override;
    void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags) override;
    void setCameraMask(unsigned short mask, bool applyChildren = true) override;

private:
    void onBeforeVisitScissor();
    void onAfterVisitScissor();

    RectClippingSupport* _rectClippingSupport;
    StencilClippingSupport* _stencilClippingSupport;
};

NS_FGUI_END

#endif
