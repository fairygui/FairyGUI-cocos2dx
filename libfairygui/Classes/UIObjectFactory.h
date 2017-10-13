#ifndef __UIOBJECTFACTORY_H__
#define __UIOBJECTFACTORY_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "tinyxml2/tinyxml2.h"
#include "GComponent.h"
#include "PackageItem.h"

NS_FGUI_BEGIN

class UIObjectFactory
{
public:
    typedef std::function<GComponent*()> GComponentCreator;
    static void setPackageItemExtension(const std::string& url, GComponentCreator creator);
    static GObject* newObject(PackageItem* pi);
    static GObject* newObject(const std::string& type);

    static void resolvePackageItemExtension(PackageItem* pi);
private:
    static std::unordered_map<std::string, GComponentCreator> packageItemExtensions;
};

NS_FGUI_END

#endif // __UIOBJECTFACTORY_H__
