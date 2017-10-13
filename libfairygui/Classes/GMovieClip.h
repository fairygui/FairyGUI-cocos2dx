#ifndef __GMOVIECLIP_H__
#define __GMOVIECLIP_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GObject.h"

NS_FGUI_BEGIN

class GMovieClip : public GObject
{
public:
    GMovieClip();
    ~GMovieClip();

    virtual void constructFromResource() override;

    CREATE_FUNC(GMovieClip);

protected:
    virtual bool init() override;

private:
    cocos2d::Sprite* _content;
    cocos2d::Action* _playAction;
};

NS_FGUI_END

#endif // __GMOVIECLIP_H__
