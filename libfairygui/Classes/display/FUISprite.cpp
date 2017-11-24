#include "FUISprite.h"

NS_FGUI_BEGIN
USING_NS_CC;

FUISprite::FUISprite()
{
}

FUISprite::~FUISprite()
{
}

void FUISprite::clearContent()
{
    setTexture(nullptr);
    CC_SAFE_RELEASE_NULL(_spriteFrame);
    setCenterRectNormalized(Rect(0, 0, 1, 1));
}

void FUISprite::setScale9Grid(Rect * value)
{
    if (value == nullptr)
    {
        setCenterRectNormalized(Rect(0, 0, 1, 1));
        return;
    }

    Rect insets = *value;

    // When Insets == Zero --> we should use a 1/3 of its untrimmed size
    if (insets.equals(Rect::ZERO)) {
        insets = Rect(_originalContentSize.width / 3.0f,
            _originalContentSize.height / 3.0f,
            _originalContentSize.width / 3.0f,
            _originalContentSize.height / 3.0f);
    }

    // emulate invalid insets. shouldn't be supported, but the original code supported it.
    if (insets.origin.x > _originalContentSize.width)
        insets.origin.x = 0;
    if (insets.origin.y > _originalContentSize.height)
        insets.origin.y = 0;
    if (insets.size.width > _originalContentSize.width)
        insets.size.width = 1;
    if (insets.size.height > _originalContentSize.height)
        insets.size.height = 1;

    // we have to convert from untrimmed to trimmed
    // Sprite::setCenterRect is using trimmed values (to be compatible with Cocos Creator)
    // Scale9Sprite::setCapInsects uses untrimmed values (which makes more sense)

    // use _rect coordinates. recenter origin to calculate the
    // intersecting rectangle
    // can't use _offsetPosition since it is calculated using bottom-left as origin,
    // and the center rect is calculated using top-left
    insets.origin.x -= (_originalContentSize.width - _rect.size.width) / 2 + _unflippedOffsetPositionFromCenter.x;
    insets.origin.y -= (_originalContentSize.height - _rect.size.height) / 2 - _unflippedOffsetPositionFromCenter.y;

    // intersecting rectangle
    const float x1 = std::max(insets.origin.x, 0.0f);
    const float y1 = std::max(insets.origin.y, 0.0f);
    const float x2 = std::min(insets.origin.x + insets.size.width, 0.0f + _rect.size.width);
    const float y2 = std::min(insets.origin.y + insets.size.height, 0.0f + _rect.size.height);

    // centerRect uses the trimmed frame origin as 0,0.
    // so, recenter inset rect
    insets.setRect(x1,
        y1,
        x2 - x1,
        y2 - y1);

    // Only update center rect while in slice mode.
    setCenterRect(insets);
}

void FUISprite::setGrayed(bool value)
{
    GLProgramState *glState = nullptr;
    if(value)
        glState = GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_GRAYSCALE, getTexture());
    else
        glState = GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP, getTexture());

    setGLProgramState(glState);
}

NS_FGUI_END