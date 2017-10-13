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
#include "tinyxml2/tinyxml2.h"

NS_FGUI_BEGIN

unordered_map<string, UIObjectFactory::GComponentCreator> UIObjectFactory::packageItemExtensions;

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

    packageItemExtensions[url] = creator;
}

GObject * UIObjectFactory::newObject(PackageItem * pi)
{
    switch (pi->type)
    {
    case PackageItemType::Image:
        return GImage::create();

    case PackageItemType::MovieClip:
        return GMovieClip::create();

    case PackageItemType::Component:
    {
        if (pi->extensionCreator != nullptr)
            return pi->extensionCreator();

        tinyxml2::XMLDocument* xml = pi->componentData;
        string extention = ToolSet::getAttribute(xml->RootElement(), "extention");
        if (extention.size() > 0)
        {
            if (extention == "Button")
                return GButton::create();
            else if (extention == "Label")
                return GLabel::create();
            else if (extention == "ProgressBar")
                return GProgressBar::create();
            else if (extention == "Slider")
                return GSlider::create();
            else if (extention == "ScrollBar")
                return GScrollBar::create();
            else if (extention == "ComboBox")
                return GComboBox::create();
            else
                return GComponent::create();
        }
        else
            return GComponent::create();
    }
    }
    return nullptr;
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
        //if (loaderCreator != null)
        //	return loaderCreator();
        //else
        return GLoader::create();
    else
        return nullptr;
}

void UIObjectFactory::resolvePackageItemExtension(PackageItem * pi)
{
    //GComponentCreator creator = packageItemExtensions[UIPackage::URL_PREFIX + pi->owner->getId() + pi->id];
    //GComponentCreator creator = packageItemExtensions[UIPackage::URL_PREFIX + pi->owner->getName() + "/" + pi->name];
    //pi.extensionCreator = null;
}

NS_FGUI_END


