#include "UIConfig.h"

NS_FGUI_BEGIN
USING_NS_CC;

char* UIConfig::defaultFont = "fonts/arial.ttf";
int UIConfig::defaultScrollStep = 25;
float UIConfig::defaultScrollDecelerationRate = 0.967f;
bool UIConfig::defaultScrollTouchEffect = true;
bool UIConfig::defaultScrollBounceEffect = true;
ScrollBarDisplayType UIConfig::defaultScrollBarDisplay = ScrollBarDisplayType::SBD_DEFAULT;
std::string UIConfig::verticalScrollBar = "";
std::string UIConfig::horizontalScrollBar = "";
int UIConfig::touchDragSensitivity = 10;
int UIConfig::touchScrollSensitivity = 20;

NS_FGUI_END