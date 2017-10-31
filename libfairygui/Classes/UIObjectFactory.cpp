#include "UIObjectFactory.h"
#include "UIPackage.h"
#include "utils/ToolSet.h"
#include "GComponent.h"
#include "GImage.h"
#include "GMovieClip.h"
#include "GTextField.h"
#include "GRichTextField.h"
#include "GTextInput.h"
#include "GLoader.h"
#include "GGraph.h"
#include "GList.h"
#include "GLabel.h"
#include "GButton.h"
#include "GProgressBar.h"
#include "GSlider.h"
#include "GComboBox.h"
#include "GScrollBar.h"
#include "GGroup.h"

NS_FGUI_BEGIN

using namespace std;

unordered_map<string, UIObjectFactory::GComponentCreator> UIObjectFactory::_packageItemExtensions;
UIObjectFactory::GLoaderCreator UIObjectFactory::_loaderCreator;

void UIObjectFactory::setPackageItemExtension(const string& url, GComponentCreator creator)
{
    if (url.size() == 0)
    {
        CCLOG("Invaild url: %s", url.c_str());
        return;
    }
    PackageItem* pi = UIPackage::getItemByURL(url);
    if (pi)
        pi->extensionCreator = creator;

    _packageItemExtensions[url] = creator;
}

GObject * UIObjectFactory::newObject(PackageItem * pi)
{
    GObject *ret = nullptr;
    switch (pi->type)
    {
    case PackageItemType::IMAGE:
        ret = GImage::create();
        break;

    case PackageItemType::MOVIECLIP:
        ret = GMovieClip::create();
        break;

    case PackageItemType::COMPONENT:
    {
        if (pi->extensionCreator != nullptr)
            ret = pi->extensionCreator();
        else
        {
            tinyxml2::XMLDocument* xml = pi->componentData;
            const char *p = xml->RootElement()->Attribute("extention");
            if (p)
            {
                std::string extention = p;
                if (extention == "Button")
                    ret = GButton::create();
                else if (extention == "Label")
                    ret = GLabel::create();
                else if (extention == "ProgressBar")
                    ret = GProgressBar::create();
                else if (extention == "Slider")
                    ret = GSlider::create();
                else if (extention == "ScrollBar")
                    ret = GScrollBar::create();
                else if (extention == "ComboBox")
                    ret = GComboBox::create();
                else
                    ret = GComponent::create();
            }
            else
                ret = GComponent::create();
        }
        break;
    }
    }

    if (ret)
        ret->_packageItem = pi;

    return ret;
}

GObject * UIObjectFactory::newObject(const string & type)
{
    if (type == "image")
        return GImage::create();
    else if (type == "movieclip")
        return GMovieClip::create();
    else if (type == "component")
        return GComponent::create();
    else if (type == "text")
        return GTextField::create();
    else if (type == "richtext")
        return GRichTextField::create();
    else if (type == "inputtext")
        return GTextInput::create();
    else if (type == "group")
        return GGroup::create();
    else if (type == "list")
        return GList::create();
    else if (type == "graph")
        return GGraph::create();
    else if (type == "loader")
        if (_loaderCreator != nullptr)
            return _loaderCreator();
        else
            return GLoader::create();
    else
        return nullptr;
}

void UIObjectFactory::setLoaderExtension(GLoaderCreator creator)
{
    _loaderCreator = creator;
}

void UIObjectFactory::resolvePackageItemExtension(PackageItem * pi)
{
    auto it = _packageItemExtensions.find(UIPackage::URL_PREFIX + pi->owner->getId() + pi->id);
    if (it != _packageItemExtensions.end())
    {
        pi->extensionCreator = it->second;
        return;
    }
    it = _packageItemExtensions.find(UIPackage::URL_PREFIX + pi->owner->getName() + "/" + pi->name);
    if (it != _packageItemExtensions.end())
    {
        pi->extensionCreator = it->second;
        return;
    }
    pi->extensionCreator = nullptr;
}

NS_FGUI_END


