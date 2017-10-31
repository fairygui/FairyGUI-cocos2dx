#ifndef __GROOT_H__
#define __GROOT_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "event/InputProcessor.h"
#include "GComponent.h"
#include "GGraph.h"
#include "Window.h"

NS_FGUI_BEGIN

class GRoot : public GComponent
{
public:
    GRoot();
    ~GRoot();

    CREATE_FUNC(GRoot);

    static GRoot* getInstance();

    void showWindow(Window* win);
    void hideWindow(Window* win);
    void hideWindowImmediately(Window* win);
    void bringToFront(Window* win);
    void showModalWait();
    void closeModalWait();
    void closeAllExceptModals();
    void closeAllWindows();
    Window* getTopWindow();

    GGraph* getModalLayer();
    bool hasModalWindow();
    bool isModalWaiting();

    InputProcessor* getInputProcessor() const { return _inputProcessor; }
    cocos2d::Vec2 getTouchPosition(int touchId);
    GObject* getTouchTarget();

    void showPopup(GObject* popup);
    void showPopup(GObject* popup, GObject* target, PopupDirection dir);
    void togglePopup(GObject* popup);
    void togglePopup(GObject* popup, GObject* target, PopupDirection dir);
    void hidePopup();
    void hidePopup(GObject* popup);
    bool hasAnyPopup();
    cocos2d::Vec2 getPoupPosition(GObject* popup, GObject* target, PopupDirection dir);

    void showTooltips(const std::string& msg);
    void showTooltipsWin(GObject* tooltipWin);
    void hideTooltips();

protected:
    virtual void handleInit() override;
    virtual void handlePositionChanged() override;

private:
    void onWindowSizeChanged();
    void createModalLayer();
    void adjustModalLayer();
    void closePopup(GObject* target);
    void checkPopups();
    void onTouchEvent(int eventType);

    CALL_LATER_FUNC(GRoot, doShowTooltipsWin);

    static GRoot* _inst;
    cocos2d::EventListener* _windowSizeListener;
    InputProcessor* _inputProcessor;

    GGraph* _modalLayer;
    GObject* _modalWaitPane;
    cocos2d::Vector<GObject*> _popupStack;
    cocos2d::Vector<GObject*> _justClosedPopups;
    GObject* _tooltipWin;
    GObject* _defaultTooltipWin;
};

NS_FGUI_END

#endif
