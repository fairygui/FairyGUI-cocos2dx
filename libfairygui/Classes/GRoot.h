#ifndef __GROOT_H__
#define __GROOT_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GComponent.h"
#include "event/InputProcessor.h"

NS_FGUI_BEGIN

class GRoot : public GComponent
{
public:
    GRoot();
    ~GRoot();

    CREATE_FUNC(GRoot);

    static GRoot* getInstance();

protected:
    virtual bool init() override;
    void handlePositionChanged() override;

private:
    void onWindowSizeChanged();

    static GRoot* _inst;
    cocos2d::EventListener* _windowSizeListener;
    InputProcessor* _inputProcessor;
};

NS_FGUI_END

#endif // __GROOT_H__
