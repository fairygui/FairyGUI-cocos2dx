#include "FUIInput.h"
#include "UIPackage.h"

NS_FGUI_BEGIN
USING_NS_CC;

FUIInput * FUIInput::create(GTextInput* owner)
{
    FUIInput* pRet = new (std::nothrow) FUIInput();

    if (pRet != nullptr && pRet->initWithSizeAndBackgroundSprite(Size(100, 100),
        (ui::Scale9Sprite*)ui::Scale9Sprite::createWithTexture(UIPackage::getEmptyTexture())))
    {
        pRet->autorelease();
        pRet->_owner = owner;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

FUIInput::FUIInput()
{
}

FUIInput::~FUIInput()
{
}

NS_FGUI_END