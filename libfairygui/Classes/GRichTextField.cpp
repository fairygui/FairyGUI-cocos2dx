#include "GRichTextField.h"

NS_FGUI_BEGIN
USING_NS_CC;

GRichTextField::GRichTextField()
{
}

GRichTextField::~GRichTextField()
{

}

bool GRichTextField::init()
{
    if (!GTextField::init())
        return false;

    return true;
}

NS_FGUI_END