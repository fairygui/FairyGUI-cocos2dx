#ifndef __SCRISSORCLIPNODE_H__
#define __SCRISSORCLIPNODE_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"

NS_FGUI_BEGIN

class ScissorClipNode : public cocos2d::Node
{
public:
    ScissorClipNode();
    ~ScissorClipNode();

    CREATE_FUNC(ScissorClipNode);

    bool isClippingEnabled() { return _clippingEnabled; }
    void setClippingEnabled(bool value);

    virtual bool init() override;
    virtual void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags) override;
    virtual void onEnter() override;

private:
    const cocos2d::Rect& getClippingRect();

    void onBeforeVisitScissor();
    void onAfterVisitScissor();

    //clipping
    bool _clippingEnabled;
    bool _scissorOldState;
    cocos2d::Rect _clippingOldRect;
    cocos2d::Rect _clippingRect;
    ScissorClipNode* _clippingParent;
    bool _clippingRectDirty;

    cocos2d::CustomCommand _beforeVisitCmdScissor;
    cocos2d::CustomCommand _afterVisitCmdScissor;
};

NS_FGUI_END

#endif // __SCRISSORCLIPNODE_H__
