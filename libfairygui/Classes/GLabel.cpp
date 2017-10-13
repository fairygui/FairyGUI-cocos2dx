#include "GLabel.h"

NS_FGUI_BEGIN
USING_NS_CC;

GLabel::GLabel() :
    _titleObject(nullptr),
    _iconObject(nullptr)
{
}

GLabel::~GLabel()
{

}

void GLabel::setTitle(const std::string & value)
{
    _title = value;
    if (_titleObject != nullptr)
        _titleObject->setText(_title);
    updateGear(6);
}

void GLabel::setIcon(const std::string & value)
{
    _icon = value;
    if (_iconObject != nullptr)
        _iconObject->setIcon(_icon);
    updateGear(7);
}

void GLabel::constructFromXML(tinyxml2::XMLElement * xml)
{
    GComponent::constructFromXML(xml);

    _titleObject = getChild("title");
    _iconObject = getChild("icon");
    if (_titleObject != nullptr)
        _title = _titleObject->getText();
    if (_iconObject != nullptr)
        _icon = _iconObject->getIcon();
}

void GLabel::setup_AfterAdd(tinyxml2::XMLElement * xml)
{
    GComponent::setup_AfterAdd(xml);

    xml = xml->FirstChildElement("Label");
    if (!xml)
        return;

    const char*p = xml->Attribute("title");
    if (p)
        this->setTitle(p);

    p = xml->Attribute("icon");
    if (p)
        this->setIcon(p);
}

bool GLabel::init()
{
    if (!GComponent::init())
        return false;

    return true;
}

NS_FGUI_END