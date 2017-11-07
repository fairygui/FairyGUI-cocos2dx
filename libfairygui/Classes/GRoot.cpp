#include "GRoot.h"
#include "UIPackage.h"

NS_FGUI_BEGIN
USING_NS_CC;

GRoot* GRoot::_inst = nullptr;

GRoot::GRoot() :
    _windowSizeListener(nullptr),
    _inputProcessor(nullptr),
    _modalLayer(nullptr),
    _modalWaitPane(nullptr),
    _tooltipWin(nullptr),
    _defaultTooltipWin(nullptr)
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

void GRoot::showWindow(Window * win)
{
    addChild(win);
    adjustModalLayer();
}

void GRoot::hideWindow(Window * win)
{
    win->hide();
}

void GRoot::hideWindowImmediately(Window * win)
{
    if (win->getParent() == this)
        removeChild(win);

    adjustModalLayer();
}

void GRoot::bringToFront(Window * win)
{
    int cnt = numChildren();
    int i;
    if (_modalLayer->getParent() != nullptr && !win->isModal())
        i = getChildIndex(_modalLayer) - 1;
    else
        i = cnt - 1;

    for (; i >= 0; i--)
    {
        GObject* g = getChildAt(i);
        if (g == win)
            return;
        if (dynamic_cast<Window*>(g))
            break;
    }

    if (i >= 0)
        setChildIndex(win, i);
}

void GRoot::showModalWait()
{
    if (!UIConfig::globalModalWaiting.empty())
    {
        if (_modalWaitPane == nullptr)
            _modalWaitPane = UIPackage::createObjectFromURL(UIConfig::globalModalWaiting);

        _modalWaitPane->setSize(getWidth(), getHeight());
        _modalWaitPane->addRelation(this, RelationType::Size);

        addChild(_modalWaitPane);
    }
}

void GRoot::closeModalWait()
{
    if (_modalWaitPane != nullptr && _modalWaitPane->getParent() != nullptr)
        removeChild(_modalWaitPane);
}

void GRoot::closeAllExceptModals()
{
    Vector<GObject*> map(_children);

    for (const auto&child : map)
    {
        if (dynamic_cast<Window*>(child) && !((Window*)child)->isModal())
            hideWindowImmediately((Window*)child);
    }
}

void GRoot::closeAllWindows()
{
    Vector<GObject*> map(_children);

    for (const auto&child : map)
    {
        if (dynamic_cast<Window*>(child))
            hideWindowImmediately((Window*)child);
    }
}

Window * GRoot::getTopWindow()
{
    int cnt = numChildren();
    for (int i = cnt - 1; i >= 0; i--)
    {
        GObject* child = getChildAt(i);
        if (dynamic_cast<Window*>(child))
        {
            return (Window*)child;
        }
    }

    return nullptr;
}

GGraph * GRoot::getModalLayer()
{
    if (_modalLayer == nullptr)
        createModalLayer();

    return _modalLayer;
}

void GRoot::createModalLayer()
{
    _modalLayer = GGraph::create();
    _modalLayer->retain();
    _modalLayer->drawRect(getWidth(), getHeight(), 0, Color4F::WHITE, UIConfig::modalLayerColor);
    _modalLayer->addRelation(this, RelationType::Size);
}

void GRoot::adjustModalLayer()
{
    if (_modalLayer == nullptr)
        createModalLayer();

    int cnt = numChildren();

    if (_modalWaitPane != nullptr && _modalWaitPane->getParent() != nullptr)
        setChildIndex(_modalWaitPane, cnt - 1);

    for (int i = cnt - 1; i >= 0; i--)
    {
        GObject* child = getChildAt(i);
        if (dynamic_cast<Window*>(child) && ((Window*)child)->isModal())
        {
            if (_modalLayer->getParent() == nullptr)
                addChildAt(_modalLayer, i);
            else
                setChildIndexBefore(_modalLayer, i);
            return;
        }
    }

    if (_modalLayer->getParent() != nullptr)
        removeChild(_modalLayer);
}

bool GRoot::hasModalWindow()
{
    return _modalLayer != nullptr && _modalLayer->getParent() != nullptr;
}

bool GRoot::isModalWaiting()
{
    return (_modalWaitPane != nullptr) && _modalWaitPane->onStage();
}

cocos2d::Vec2 GRoot::getTouchPosition(int touchId)
{
    return _inputProcessor->getTouchPosition(touchId);
}

GObject * GRoot::getTouchTarget()
{
    return _inputProcessor->getRecentInput()->getTarget();
}

void GRoot::showPopup(GObject * popup)
{
    showPopup(popup, nullptr, PopupDirection::AUTO);
}

void GRoot::showPopup(GObject * popup, GObject * target, PopupDirection dir)
{
    if (!_popupStack.empty())
    {
        int k = _popupStack.getIndex(popup);
        if (k != -1)
        {
            for (int i = _popupStack.size() - 1; i >= k; i--)
            {
                closePopup(_popupStack.back());
                _popupStack.popBack();
            }
        }
    }
    _popupStack.pushBack(popup);

    addChild(popup);
    adjustModalLayer();

    if (dynamic_cast<Window*>(popup) && target == nullptr && dir == PopupDirection::AUTO)
        return;

    Vec2 pos = getPoupPosition(popup, target, dir);
    popup->setPosition(pos.x, pos.y);
}

void GRoot::togglePopup(GObject * popup)
{
    togglePopup(popup, nullptr, PopupDirection::AUTO);
}

void GRoot::togglePopup(GObject * popup, GObject * target, PopupDirection dir)
{
    if (_justClosedPopups.getIndex(popup) != -1)
        return;

    showPopup(popup, target, dir);
}

void GRoot::hidePopup()
{
    hidePopup(nullptr);
}

void GRoot::hidePopup(GObject * popup)
{
    if (popup != nullptr)
    {
        int k = _popupStack.getIndex(popup);
        if (k != -1)
        {
            for (int i = _popupStack.size() - 1; i >= k; i--)
            {
                closePopup(_popupStack.back());
                _popupStack.popBack();
            }
        }
    }
    else
    {
        for (const auto &obj : _popupStack)
            closePopup(obj);
        _popupStack.clear();
    }
}

void GRoot::closePopup(GObject * target)
{
    if (target->getParent() != nullptr)
    {
        if (dynamic_cast<Window*>(target))
            ((Window*)target)->hide();
        else
            removeChild(target);
    }
}

void GRoot::checkPopups()
{
    _justClosedPopups.clear();
    if (!_popupStack.empty())
    {
        GObject* mc = _inputProcessor->getRecentInput()->getTarget();
        bool handled = false;
        while (mc != this && mc != nullptr)
        {
            int k = _popupStack.getIndex(mc);
            if (k != -1)
            {
                for (int i = _popupStack.size() - 1; i > k; i--)
                {
                    GObject* popup = _popupStack.back();
                    _justClosedPopups.pushBack(popup);
                    closePopup(popup);
                    _popupStack.popBack();
                }
                handled = true;
                break;
            }
            mc = mc->getParent();
        }

        if (!handled)
        {
            for (int i = _popupStack.size() - 1; i >= 0; i--)
            {
                GObject* popup = _popupStack.at(i);
                _justClosedPopups.pushBack(popup);
                closePopup(popup);
            }
            _popupStack.clear();
        }
    }
}

bool GRoot::hasAnyPopup()
{
    return !_popupStack.empty();
}

cocos2d::Vec2 GRoot::getPoupPosition(GObject * popup, GObject * target, PopupDirection dir)
{
    Vec2 pos;
    Vec2 size;
    if (target != nullptr)
    {
        pos = target->localToGlobal(Vec2::ZERO);
        pos = this->globalToLocal(pos);
        size = target->localToGlobal(target->getSize());
        size = this->globalToLocal(size);
        size -= pos;
    }
    else
    {
        pos = globalToLocal(_inputProcessor->getRecentInput()->getPosition());
    }
    float xx, yy;
    xx = pos.x;
    if (xx + popup->getWidth() > getWidth())
        xx = xx + size.x - popup->getWidth();
    yy = pos.y + size.y;
    if ((dir == PopupDirection::AUTO && yy + popup->getHeight() > getHeight())
        || dir == PopupDirection::UP)
    {
        yy = pos.y - popup->getHeight() - 1;
        if (yy < 0)
        {
            yy = 0;
            xx += size.x / 2;
        }
    }

    return Vec2(round(xx), round(yy));
}

void GRoot::showTooltips(const std::string & msg)
{
    if (_defaultTooltipWin == nullptr)
    {
        const std::string& resourceURL = UIConfig::tooltipsWin;
        if (resourceURL.length() > 0)
        {
            CCLOGWARN("FairyGUI: UIConfig.tooltipsWin not defined");
            return;
        }

        _defaultTooltipWin = UIPackage::createObjectFromURL(resourceURL);
        _defaultTooltipWin->setTouchable(false);
    }

    _defaultTooltipWin->setText(msg);
    showTooltipsWin(_defaultTooltipWin);
}

void GRoot::showTooltipsWin(GObject * tooltipWin)
{
    hideTooltips();

    _tooltipWin = tooltipWin;
    CALL_LATER(GRoot, doShowTooltipsWin, 0.1f);
}

void GRoot::doShowTooltipsWin()
{
    if (_tooltipWin == nullptr)
        return;

    Vec2 pt = _inputProcessor->getRecentInput()->getPosition();
    float xx = pt.x + 10;
    float yy = pt.y + 20;

    pt = globalToLocal(Vec2(xx, yy));
    xx = pt.x;
    yy = pt.y;

    if (xx + _tooltipWin->getWidth() > getWidth())
        xx = xx - _tooltipWin->getWidth();
    if (yy + _tooltipWin->getHeight() > getHeight())
    {
        yy = yy - _tooltipWin->getHeight() - 1;
        if (yy < 0)
            yy = 0;
    }

    _tooltipWin->setPosition(round(xx), round(yy));
    addChild(_tooltipWin);
}

void GRoot::hideTooltips()
{
    if (_tooltipWin != nullptr)
    {
        if (_tooltipWin->getParent() != nullptr)
            removeChild(_tooltipWin);
        _tooltipWin = nullptr;
    }
}

void GRoot::onTouchEvent(int eventType)
{
    if (eventType == UIEventType::TouchBegin)
    {
        if (_tooltipWin != nullptr)
            hideTooltips();

        checkPopups();
    }
}

void GRoot::handleInit()
{
    GComponent::handleInit();

    _inputProcessor = new InputProcessor(this);
    _inputProcessor->setCaptureCallback(CC_CALLBACK_1(GRoot::onTouchEvent, this));

#ifdef CC_PLATFORM_PC
    _windowSizeListener = Director::getInstance()->getEventDispatcher()->addCustomEventListener(GLViewImpl::EVENT_WINDOW_RESIZED, CC_CALLBACK_0(GRoot::onWindowSizeChanged, this));
#endif
    onWindowSizeChanged();
}

void GRoot::handlePositionChanged()
{
    _displayObject->setPosition(0, _size.height);
}

void GRoot::onWindowSizeChanged()
{
    const cocos2d::Size& rs = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    setSize(rs.width, rs.height);
}


NS_FGUI_END