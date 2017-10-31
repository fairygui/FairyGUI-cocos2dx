#include "GObjectPool.h"
#include "GObject.h"
#include "UIPackage.h"

NS_FGUI_BEGIN
USING_NS_CC;

GObjectPool::GObjectPool()
{
}

GObjectPool::~GObjectPool()
{
    for (auto it = _pool.cbegin(); it != _pool.cend(); ++it)
        delete it->second;
}

GObject * GObjectPool::getObject(const std::string & url)
{
    std::string url2 = UIPackage::normalizeURL(url);
    if (url2.length() == 0)
        return nullptr;

    GObject* ret;
    Vector<GObject*>* arr = _pool[url2];
    if (arr != nullptr && !arr->empty())
    {
        ret = arr->back();
        ret->retain();
        arr->popBack();
        return ret;
    }

    ret = UIPackage::createObjectFromURL(url2);
    return ret;
}

void GObjectPool::returnObject(GObject * obj)
{
    const std::string& url = obj->getResourceURL();
    Vector<GObject*>* arr = _pool[url];
    if (arr == nullptr)
    {
        arr = new Vector<GObject*>();
        _pool[url] = arr;
    }

    arr->pushBack(obj);
}

NS_FGUI_END