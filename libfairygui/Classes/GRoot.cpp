#include "GRoot.h"

NS_FGUI_BEGIN
USING_NS_CC;

GRoot* GRoot::_inst = nullptr;

GRoot::GRoot()
{
}

GRoot::~GRoot()
{
    delete _inputProcessor;

    Director::getInstance()->getEventDispatcher()->removeEventListener(_windowSizeListener);
}

GRoot* GRoot::getInstance()
{
    if (_inst == nullptr)
    {
        _inst = GRoot::create();
        _inst->retain();
    }

    return _inst;
}

bool GRoot::init()
{
    if (!GComponent::init())
        return false;

    _inputProcessor = new InputProcessor(this);

    _windowSizeListener = Director::getInstance()->getEventDispatcher()->addCustomEventListener(GLViewImpl::EVENT_WINDOW_RESIZED, CC_CALLBACK_0(GRoot::onWindowSizeChanged, this));
    onWindowSizeChanged();

    return true;
}

void GRoot::handlePositionChanged()
{
    _displayObject->setPosition(Vec2(0, this->getHeight()));
}

void GRoot::onWindowSizeChanged()
{
    const cocos2d::Size& rs = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    this->setSize(rs.width, rs.height);
    handlePositionChanged();
    log("onWindowSizeChanged");
}
NS_FGUI_END