#include "FUIContainer.h"
#include "base/CCStencilStateManager.h"

NS_FGUI_BEGIN
USING_NS_CC;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC || CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
#define CC_CLIPPING_NODE_OPENGLES 0
#else
#define CC_CLIPPING_NODE_OPENGLES 1
#endif

#if CC_CLIPPING_NODE_OPENGLES
static void setProgram(Node *n, GLProgram *p)
{
    n->setGLProgram(p);

    auto& children = n->getChildren();
    for (const auto &child : children) {
        setProgram(child, p);
    }
}
#endif

StencilClippingSupport::StencilClippingSupport() :
    _stencil(nullptr),
    _originStencilProgram(nullptr),
    _stencilStateManager(new StencilStateManager())
{
}

FUIContainer::FUIContainer() :
    _rectClippingSupport(nullptr),
    _stencilClippingSupport(nullptr)
{
}

FUIContainer::~FUIContainer()
{
    CC_SAFE_DELETE(_rectClippingSupport);
    if (_stencilClippingSupport)
    {
        if (_stencilClippingSupport->_stencil)
        {
            _stencilClippingSupport->_stencil->stopAllActions();
            _stencilClippingSupport->_stencil->release();
        }
        CC_SAFE_DELETE(_stencilClippingSupport->_stencilStateManager);
        delete _stencilClippingSupport;
    }
}

bool FUIContainer::isClippingEnabled() const
{
    if (_rectClippingSupport != nullptr)
        return _rectClippingSupport->_clippingEnabled;
    else
        return false;
}

void FUIContainer::setClippingEnabled(bool value)
{
    if (_rectClippingSupport == nullptr)
    {
        if (!value)
            return;

        _rectClippingSupport = new RectClippingSupport();
    }

    _rectClippingSupport->_clippingEnabled = value;
}

const Rect & FUIContainer::getClippingRegion() const
{
    if (_rectClippingSupport != nullptr)
        return _rectClippingSupport->_clippingRegion;
    else
        return Rect::ZERO;
}

void FUIContainer::setClippingRegion(const Rect & clippingRegion)
{
    if (_rectClippingSupport == nullptr)
        _rectClippingSupport = new RectClippingSupport();

    _rectClippingSupport->_clippingRegion = clippingRegion;
}

cocos2d::Node * FUIContainer::getStencil() const
{
    if (_stencilClippingSupport != nullptr)
        return _stencilClippingSupport->_stencil;
    else
        return nullptr;
}

void FUIContainer::setStencil(cocos2d::Node * stencil)
{
    if (_stencilClippingSupport == nullptr)
    {
        if (stencil == nullptr)
            return;

        _stencilClippingSupport = new StencilClippingSupport();
    }

    //early out if the stencil is already set
    if (_stencilClippingSupport->_stencil == stencil)
        return;

#if CC_ENABLE_GC_FOR_NATIVE_OBJECTS
    auto sEngine = ScriptEngineManager::getInstance()->getScriptEngine();
    if (sEngine)
    {
        if (_stencilClippingSupport->_stencil)
            sEngine->releaseScriptObject(this, _stencilClippingSupport->_stencil);
        if (stencil)
            sEngine->retainScriptObject(this, stencil);
    }
#endif // CC_ENABLE_GC_FOR_NATIVE_OBJECTS

    //cleanup current stencil
    if (_stencilClippingSupport->_stencil != nullptr && _stencilClippingSupport->_stencil->isRunning())
    {
        _stencilClippingSupport->_stencil->onExitTransitionDidStart();
        _stencilClippingSupport->_stencil->onExit();
    }
    CC_SAFE_RELEASE_NULL(_stencilClippingSupport->_stencil);

    //initialise new stencil
    _stencilClippingSupport->_stencil = stencil;
    CC_SAFE_RETAIN(_stencilClippingSupport->_stencil);
    if (_stencilClippingSupport->_stencil != nullptr && this->isRunning())
    {
        _stencilClippingSupport->_stencil->onEnter();
        if (this->_isTransitionFinished)
        {
            _stencilClippingSupport->_stencil->onEnterTransitionDidFinish();
        }
    }

    if (_stencilClippingSupport->_stencil != nullptr)
        _stencilClippingSupport->_originStencilProgram = _stencilClippingSupport->_stencil->getGLProgram();
}

GLfloat FUIContainer::getAlphaThreshold() const
{
    if (_stencilClippingSupport != nullptr)
        return _stencilClippingSupport->_stencilStateManager->getAlphaThreshold();
    else
        return 1;
}

void FUIContainer::setAlphaThreshold(GLfloat alphaThreshold)
{
    if (_stencilClippingSupport == nullptr)
        _stencilClippingSupport = new StencilClippingSupport();

#if CC_CLIPPING_NODE_OPENGLES
    if (alphaThreshold == 1 && alphaThreshold != _stencilClippingSupport->_stencilStateManager->getAlphaThreshold())
    {
        // should reset program used by _stencil
        if (_stencilClippingSupport->_stencil)
            setProgram(_stencilClippingSupport->_stencil, _stencilClippingSupport->_originStencilProgram);
    }
#endif

    _stencilClippingSupport->_stencilStateManager->setAlphaThreshold(alphaThreshold);
}

bool FUIContainer::isInverted() const
{
    if (_stencilClippingSupport != nullptr)
        return _stencilClippingSupport->_stencilStateManager->isInverted();
    else
        return false;
}

void FUIContainer::setInverted(bool inverted)
{
    if (_stencilClippingSupport == nullptr)
        _stencilClippingSupport = new StencilClippingSupport();

    _stencilClippingSupport->_stencilStateManager->setInverted(inverted);
}

void FUIContainer::onEnter()
{
#if CC_ENABLE_SCRIPT_BINDING
    if (_scriptType == kScriptTypeJavascript)
    {
        if (ScriptEngineManager::sendNodeEventToJSExtended(this, kNodeOnEnter))
            return;
    }
#endif

    Node::onEnter();

    if (_stencilClippingSupport != nullptr && _stencilClippingSupport->_stencil != nullptr)
    {
        _stencilClippingSupport->_stencil->onEnter();
    }
    else
    {
        CCLOG("StencilClippingSupport warning: _stencil is nil.");
    }
}

void FUIContainer::onEnterTransitionDidFinish()
{
#if CC_ENABLE_SCRIPT_BINDING
    if (_scriptType == kScriptTypeJavascript)
    {
        if (ScriptEngineManager::sendNodeEventToJSExtended(this, kNodeOnEnterTransitionDidFinish))
            return;
    }
#endif

    Node::onEnterTransitionDidFinish();

    if (_stencilClippingSupport != nullptr && _stencilClippingSupport->_stencil != nullptr)
    {
        _stencilClippingSupport->_stencil->onEnterTransitionDidFinish();
    }
}

void FUIContainer::onExitTransitionDidStart()
{
#if CC_ENABLE_SCRIPT_BINDING
    if (_scriptType == kScriptTypeJavascript)
    {
        if (ScriptEngineManager::sendNodeEventToJSExtended(this, kNodeOnExitTransitionDidStart))
            return;
    }
#endif

    if (_stencilClippingSupport != nullptr && _stencilClippingSupport->_stencil != nullptr)
    {
        _stencilClippingSupport->_stencil->onExitTransitionDidStart();
    }

    Node::onExitTransitionDidStart();
}

void FUIContainer::onExit()
{
#if CC_ENABLE_SCRIPT_BINDING
    if (_scriptType == kScriptTypeJavascript)
    {
        if (ScriptEngineManager::sendNodeEventToJSExtended(this, kNodeOnExit))
            return;
    }
#endif

    if (_stencilClippingSupport != nullptr && _stencilClippingSupport->_stencil != nullptr)
    {
        _stencilClippingSupport->_stencil->onExit();
    }

    Node::onExit();
}

void FUIContainer::setCameraMask(unsigned short mask, bool applyChildren)
{
    Node::setCameraMask(mask, applyChildren);

    if (_stencilClippingSupport != nullptr && _stencilClippingSupport->_stencil != nullptr)
        _stencilClippingSupport->_stencil->setCameraMask(mask, applyChildren);
}

void FUIContainer::onBeforeVisitScissor()
{
    glEnable(GL_SCISSOR_TEST);

    float scaleX = _scaleX;
    float scaleY = _scaleY;
    Node *parent = this->getParent();
    while (parent) {
        scaleX *= parent->getScaleX();
        scaleY *= parent->getScaleY();
        parent = parent->getParent();
    }

    const Point pos = convertToWorldSpace(Point(_rectClippingSupport->_clippingRegion.origin.x, _rectClippingSupport->_clippingRegion.origin.y));
    GLView* glView = Director::getInstance()->getOpenGLView();
    glView->setScissorInPoints(pos.x,
        pos.y,
        _rectClippingSupport->_clippingRegion.size.width * scaleX,
        _rectClippingSupport->_clippingRegion.size.height * scaleY);
}

void FUIContainer::onAfterVisitScissor()
{
    glDisable(GL_SCISSOR_TEST);
}

void FUIContainer::visit(cocos2d::Renderer * renderer, const cocos2d::Mat4 & parentTransform, uint32_t parentFlags)
{
    if (_stencilClippingSupport != nullptr)
    {
        if (!_visible || _children.empty())
            return;

        uint32_t flags = processParentFlags(parentTransform, parentFlags);

        // IMPORTANT:
        // To ease the migration to v3.0, we still support the Mat4 stack,
        // but it is deprecated and your code should not rely on it
        Director* director = Director::getInstance();
        CCASSERT(nullptr != director, "Director is null when setting matrix stack");
        director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
        director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewTransform);

        //Add group command

        _stencilClippingSupport->_groupCommand.init(_globalZOrder);
        renderer->addCommand(&_stencilClippingSupport->_groupCommand);

        renderer->pushGroup(_stencilClippingSupport->_groupCommand.getRenderQueueID());

        _stencilClippingSupport->_beforeVisitCmd.init(_globalZOrder);
        _stencilClippingSupport->_beforeVisitCmd.func = CC_CALLBACK_0(StencilStateManager::onBeforeVisit, _stencilClippingSupport->_stencilStateManager);
        renderer->addCommand(&_stencilClippingSupport->_beforeVisitCmd);

        auto alphaThreshold = this->getAlphaThreshold();
        if (alphaThreshold < 1)
        {
#if CC_CLIPPING_NODE_OPENGLES
            // since glAlphaTest do not exists in OES, use a shader that writes
            // pixel only if greater than an alpha threshold
            GLProgram *program = GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_TEXTURE_ALPHA_TEST_NO_MV);
            GLint alphaValueLocation = glGetUniformLocation(program->getProgram(), GLProgram::UNIFORM_NAME_ALPHA_TEST_VALUE);
            // set our alphaThreshold
            program->use();
            program->setUniformLocationWith1f(alphaValueLocation, alphaThreshold);
            // we need to recursively apply this shader to all the nodes in the stencil node
            // FIXME: we should have a way to apply shader to all nodes without having to do this
            setProgram(_stencilClippingSupport->_stencil, program);
#endif

        }
        _stencilClippingSupport->_stencil->visit(renderer, _modelViewTransform, flags);

        _stencilClippingSupport->_afterDrawStencilCmd.init(_globalZOrder);
        _stencilClippingSupport->_afterDrawStencilCmd.func = CC_CALLBACK_0(StencilStateManager::onAfterDrawStencil, _stencilClippingSupport->_stencilStateManager);
        renderer->addCommand(&_stencilClippingSupport->_afterDrawStencilCmd);

        int i = 0;
        bool visibleByCamera = isVisitableByVisitingCamera();

        if (!_children.empty())
        {
            sortAllChildren();
            // draw children zOrder < 0
            for (auto size = _children.size(); i < size; ++i)
            {
                auto node = _children.at(i);

                if (node && node->getLocalZOrder() < 0)
                    node->visit(renderer, _modelViewTransform, flags);
                else
                    break;
            }
            // self draw
            if (visibleByCamera)
                this->draw(renderer, _modelViewTransform, flags);

            for (auto it = _children.cbegin() + i, itCend = _children.cend(); it != itCend; ++it)
                (*it)->visit(renderer, _modelViewTransform, flags);
        }
        else if (visibleByCamera)
        {
            this->draw(renderer, _modelViewTransform, flags);
        }

        _stencilClippingSupport->_afterVisitCmd.init(_globalZOrder);
        _stencilClippingSupport->_afterVisitCmd.func = CC_CALLBACK_0(StencilStateManager::onAfterVisit, _stencilClippingSupport->_stencilStateManager);
        renderer->addCommand(&_stencilClippingSupport->_afterVisitCmd);

        renderer->popGroup();

        director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    }
    else if (_rectClippingSupport != nullptr && _rectClippingSupport->_clippingEnabled)
    {
        _rectClippingSupport->_beforeVisitCmdScissor.init(_globalZOrder);
        _rectClippingSupport->_beforeVisitCmdScissor.func = CC_CALLBACK_0(FUIContainer::onBeforeVisitScissor, this);
        renderer->addCommand(&_rectClippingSupport->_beforeVisitCmdScissor);

        Node::visit(renderer, parentTransform, parentFlags);

        _rectClippingSupport->_afterVisitCmdScissor.init(_globalZOrder);
        _rectClippingSupport->_afterVisitCmdScissor.func = CC_CALLBACK_0(FUIContainer::onAfterVisitScissor, this);
        renderer->addCommand(&_rectClippingSupport->_afterVisitCmdScissor);
    }
    else
        Node::visit(renderer, parentTransform, parentFlags);
}

NS_FGUI_END