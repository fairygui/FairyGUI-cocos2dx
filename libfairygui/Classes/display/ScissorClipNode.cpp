#include "ScissorClipNode.h"

NS_FGUI_BEGIN
USING_NS_CC;

ScissorClipNode::ScissorClipNode() :
    _clippingEnabled(false),
    _clippingRect(Rect::ZERO),
    _clippingParent(nullptr),
    _clippingRectDirty(true)
{
}

ScissorClipNode::~ScissorClipNode()
{

}

void ScissorClipNode::setClippingEnabled(bool value)
{
    _clippingEnabled = value;
}

bool ScissorClipNode::init()
{
    if (!Node::init())
        return false;

    return true;
}

void ScissorClipNode::visit(Renderer * renderer, const Mat4 & parentTransform, uint32_t parentFlags)
{
    if (!_visible)
        return;

    if (_clippingEnabled)
    {
        if (parentFlags & FLAGS_DIRTY_MASK)
        {
            _clippingRectDirty = true;
        }
        _beforeVisitCmdScissor.init(_globalZOrder);
        _beforeVisitCmdScissor.func = CC_CALLBACK_0(ScissorClipNode::onBeforeVisitScissor, this);
        renderer->addCommand(&_beforeVisitCmdScissor);

        Node::visit(renderer, parentTransform, parentFlags);

        _afterVisitCmdScissor.init(_globalZOrder);
        _afterVisitCmdScissor.func = CC_CALLBACK_0(ScissorClipNode::onAfterVisitScissor, this);
        renderer->addCommand(&_afterVisitCmdScissor);
    }
    else
    {
        Node::visit(renderer, parentTransform, parentFlags);
    }
}

void ScissorClipNode::onEnter()
{
    Node::onEnter();
    _clippingRectDirty = true;
}

void ScissorClipNode::onBeforeVisitScissor()
{
    auto glview = Director::getInstance()->getOpenGLView();
    // apply scissor test
    _scissorOldState = glview->isScissorEnabled();
    if (false == _scissorOldState)
    {
        glEnable(GL_SCISSOR_TEST);
    }

    // apply scissor box
    Rect clippingRect = getClippingRect();
    _clippingOldRect = glview->getScissorRect();
    if (false == _clippingOldRect.equals(clippingRect))
    {
        glview->setScissorInPoints(clippingRect.origin.x,
            clippingRect.origin.y,
            clippingRect.size.width,
            clippingRect.size.height);
    }
}

void ScissorClipNode::onAfterVisitScissor()
{
    if (_scissorOldState)
    {
        // revert scissor box
        if (false == _clippingOldRect.equals(_clippingRect))
        {
            auto glview = Director::getInstance()->getOpenGLView();
            glview->setScissorInPoints(_clippingOldRect.origin.x,
                _clippingOldRect.origin.y,
                _clippingOldRect.size.width,
                _clippingOldRect.size.height);
        }
    }
    else
    {
        // revert scissor test
        glDisable(GL_SCISSOR_TEST);
    }
}


const Rect& ScissorClipNode::getClippingRect()
{
    if (_clippingRectDirty)
    {
        AffineTransform t = getNodeToWorldAffineTransform();
        float scissorWidth = _contentSize.width*t.a;
        float scissorHeight = _contentSize.height*t.d;
        Vec2 worldPos = convertToWorldSpace(Vec2(0, -scissorHeight));
        Rect parentClippingRect;
        ScissorClipNode* parent = this;

        while (parent)
        {
            parent = dynamic_cast<ScissorClipNode*>(parent->getParent());
            if (parent)
            {
                if (parent->_clippingEnabled)
                {
                    _clippingParent = parent;
                    break;
                }
            }
        }

        if (_clippingParent)
        {
            parentClippingRect = _clippingParent->getClippingRect();
            float finalX = worldPos.x - (scissorWidth * _anchorPoint.x);
            float finalY = worldPos.y - (scissorHeight * _anchorPoint.y);
            float finalWidth = scissorWidth;
            float finalHeight = scissorHeight;

            float leftOffset = worldPos.x - parentClippingRect.origin.x;
            if (leftOffset < 0.0f)
            {
                finalX = parentClippingRect.origin.x;
                finalWidth += leftOffset;
            }
            float rightOffset = (worldPos.x + scissorWidth) - (parentClippingRect.origin.x + parentClippingRect.size.width);
            if (rightOffset > 0.0f)
            {
                finalWidth -= rightOffset;
            }
            float topOffset = (worldPos.y + scissorHeight) - (parentClippingRect.origin.y + parentClippingRect.size.height);
            if (topOffset > 0.0f)
            {
                finalHeight -= topOffset;
            }
            float bottomOffset = worldPos.y - parentClippingRect.origin.y;
            if (bottomOffset < 0.0f)
            {
                finalY = parentClippingRect.origin.y;
                finalHeight += bottomOffset;
            }
            if (finalWidth < 0.0f)
            {
                finalWidth = 0.0f;
            }
            if (finalHeight < 0.0f)
            {
                finalHeight = 0.0f;
            }
            _clippingRect.origin.x = finalX;
            _clippingRect.origin.y = finalY;
            _clippingRect.size.width = finalWidth;
            _clippingRect.size.height = finalHeight;
        }
        else
        {
            _clippingRect.origin.x = worldPos.x - (scissorWidth * _anchorPoint.x);
            _clippingRect.origin.y = worldPos.y - (scissorHeight * _anchorPoint.y);
            _clippingRect.size.width = scissorWidth;
            _clippingRect.size.height = scissorHeight;
        }
        _clippingRectDirty = false;
    }
    return _clippingRect;
}


NS_FGUI_END