#ifndef __SCROLLPANE_H__
#define __SCROLLPANE_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GComponent.h"
#include "GScrollBar.h"

NS_FGUI_BEGIN

class ScrollPane : public cocos2d::Ref
{
public:
    ScrollPane(GComponent* owner,
        ScrollType scrollType,
        const Margin& scrollBarMargin,
        ScrollBarDisplayType scrollBarDisplay,
        int flags,
        const std::string& vtScrollBarRes,
        const std::string& hzScrollBarRes,
        const std::string& headerRes,
        const std::string& footerRes);
    ~ScrollPane();

    GComponent* getOwner() { return _owner; }
    GComponent* getHeader() { return _header; }
    GComponent* getFooter() { return _footer; }

    bool isBouncebackEffect() { return _bouncebackEffect; }
    void setBouncebackEffect(bool value) { _bouncebackEffect = value; }

    bool isTouchEffect() { return _touchEffect; }
    void setTouchEffect(bool value) { _touchEffect = value; }

    bool isInertiaDisabled() { return _inertiaDisabled; }
    void setInertiaDisabled(bool value) { _inertiaDisabled = value; }

    float getScrollStep() { return _scrollStep; }
    void setScrollStep(float value);

    bool isSnapToItem() { return _snapToItem; }
    void setSnapToItem(bool value) { _snapToItem = value; }

    bool isPageMode() { return _pageMode; }
    void setPageMode(bool value) { _pageMode = value; }

    Controller* getPageController() { return _pageController; }
    void setPageController(Controller* value) { _pageController = value; }

    bool isMouseWheelEnabled() { return _mouseWheelEnabled; }
    void setMouseWheelEnabled(bool value) { _mouseWheelEnabled = value; }

    float getDecelerationRate() { return _decelerationRate; }
    void setDecelerationRate(float value) { _decelerationRate = value; }

    float getPosX() { return _xPos; }
    void setPosX(float value, bool ani = false);
    float getPosY() { return _yPos; }
    void setPosY(float value, bool ani = false);

    float getPercX();
    void setPercX(float value, bool ani = false);
    float getPercY();
    void setPercY(float value, bool ani = false);

    bool isBottomMost();
    bool isRightMost();

    void scrollLeft(float ratio = 1, bool ani = false);
    void scrollRight(float ratio = 1, bool ani = false);
    void scrollUp(float ratio = 1, bool ani = false);
    void scrollDown(float ratio = 1, bool ani = false);
    void scrollTop(bool ani = false);
    void scrollBottom(bool ani = false);
    void scrollToView(GObject* obj, bool ani = false, bool setFirst = false);
    void scrollToView(const cocos2d::Rect& rect, bool ani = false, bool setFirst = false);
    bool isChildInView(GObject* obj);

    int getPageX();
    void setPageX(int value, bool ani = false);
    int getPageY();
    void setPageY(int value, bool ani = false);

    float getScrollingPosX();
    float getScrollingPosY();

    const cocos2d::Size& getContentSize() { return _contentSize; }
    const cocos2d::Size& getViewSize() { return _viewSize; }
    void setViewWidth(float viewWidth);
    void setViewHeight(float viewHeight);

    void lockHeader(int size);
    void lockFooter(int size);

    void cancelDragging();
    static ScrollPane* draggingPane;

private:
    void onOwnerSizeChanged();
    void handleControllerChanged(Controller* c);
    void updatePageController();
    void adjustMaskContainer();
    void setContentSize(float aWidth, float aHeight);
    void changeContentSizeOnScrolling(float deltaWidth, float deltaHeight, float deltaPosX, float deltaPosY);
    void setSize(float aWidth, float aHeight);
    void handleSizeChanged();
    void posChanged(bool ani);
    CALL_LATER_FUNC(ScrollPane, refresh);
    void refresh2();
    void syncScrollBar(bool end = false);
    void showScrollBar(bool show);
    CALL_LATER_FUNC(ScrollPane, onShowScrollBar);
    float getLoopPartSize(float division, int axis);
    bool loopCheckingCurrent();
    void loopCheckingTarget(cocos2d::Vec2& endPos);
    void loopCheckingTarget(cocos2d::Vec2& endPos, int axis);
    void loopCheckingNewPos(float& value, int axis);
    void alignPosition(cocos2d::Vec2& pos, bool inertialScrolling);
    float alignByPage(float pos, int axis, bool inertialScrolling);
    cocos2d::Vec2 updateTargetAndDuration(const cocos2d::Vec2& orignPos);
    float updateTargetAndDuration(float pos, int axis);
    void fixDuration(int axis, float oldChange);
    void killTween();
    void checkRefreshBar();
    void tweenUpdate(float dt);
    float runTween(int axis, float dt);

    void onTouchBegin(EventContext* context);
    void onTouchMove(EventContext* context);
    void onTouchEnd(EventContext* context);
    void onMouseWheel(EventContext* context);
    void onRollOver(EventContext* context);
    void onRollOut(EventContext* context);

    ScrollType _scrollType;
    float _scrollStep;
    float _mouseWheelStep;
    Margin _scrollBarMargin;
    bool _bouncebackEffect;
    bool _touchEffect;
    bool _scrollBarDisplayAuto;
    bool _vScrollNone;
    bool _hScrollNone;
    bool _needRefresh;
    int _refreshBarAxis;

    bool _displayOnLeft;
    bool _snapToItem;
    bool _displayInDemand;
    bool _mouseWheelEnabled;
    bool _pageMode;
    cocos2d::Size _pageSize;
    bool _inertiaDisabled;
    bool _maskDisabled;
    float _decelerationRate;

    float _xPos;
    float _yPos;

    cocos2d::Size _viewSize;
    cocos2d::Size _contentSize;
    cocos2d::Size _overlapSize;
    cocos2d::Vec2 _containerPos;
    cocos2d::Vec2 _beginTouchPos;
    cocos2d::Vec2 _lastTouchPos;
    cocos2d::Vec2 _lastTouchGlobalPos;
    cocos2d::Vec2 _velocity;
    float _velocityScale;
    clock_t _lastMoveTime;
    bool _isMouseMoved;
    bool _isHoldAreaDone;
    int _aniFlag;
    bool _scrollBarVisible;
    int _loop;
    int _headerLockedSize;
    int _footerLockedSize;

    int _tweening;
    cocos2d::Vec2 _tweenStart;
    cocos2d::Vec2 _tweenChange;
    cocos2d::Vec2 _tweenTime;
    cocos2d::Vec2 _tweenDuration;

    GComponent* _owner;
    cocos2d::Node* _maskContainer;
    cocos2d::Node* _container;
    GScrollBar* _hzScrollBar;
    GScrollBar* _vtScrollBar;
    GComponent* _header;
    GComponent* _footer;
    Controller* _pageController;

    static int _gestureFlag;

    friend class GComponent;
};

NS_FGUI_END

#endif // __SCROLLPANE_H__
