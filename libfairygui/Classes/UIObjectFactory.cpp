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
#include "GTree.h"

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
    if (pi->extensionCreator != nullptr)
        return pi->extensionCreator();
    else
        return newObject(pi->objectType);
}

GObject * UIObjectFactory::newObject(ObjectType type)
{
    switch (type)
    {
    case ObjectType::IMAGE:
        return GImage::create();

    case ObjectType::MOVIECLIP:
        return GMovieClip::create();

    case ObjectType::COMPONENT:
        return GComponent::create();

    case ObjectType::TEXT:
        return GBasicTextField::create();

    case ObjectType::RICHTEXT:
        return GRichTextField::create();

    case ObjectType::INPUTTEXT:
        return GTextInput::create();

    case ObjectType::GROUP:
        return GGroup::create();

    case ObjectType::LIST:
        return GList::create();

    case ObjectType::GRAPH:
        return GGraph::create();

    case ObjectType::LOADER:
        if (_loaderCreator != nullptr)
            return _loaderCreator();
        else
            return GLoader::create();

    case ObjectType::BUTTON:
        return GButton::create();

    case ObjectType::LABEL:
        return GLabel::create();

    case ObjectType::PROGRESSBAR:
        return GProgressBar::create();

    case ObjectType::SLIDER:
        return GSlider::create();

    case ObjectType::SCROLLBAR:
        return GScrollBar::create();

    case ObjectType::COMBOBOX:
        return GComboBox::create();

    case ObjectType::TREE:
        return GTree::create();

    default:
        return nullptr;
    }
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


