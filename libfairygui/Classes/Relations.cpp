#include "Relations.h"
#include "GComponent.h"

NS_FGUI_BEGIN
USING_NS_CC;

Relations::Relations(GObject* owner) :
    handling(nullptr)
{
    _owner = owner;
}

Relations::~Relations()
{
    clearAll();
}

void Relations::add(GObject * target, RelationType relationType)
{
    add(target, relationType, false);
}

void Relations::add(GObject * target, RelationType relationType, bool usePercent)
{
    CCASSERT(target, "target is null");

    for (auto it = _items.begin(); it != _items.end(); ++it)
    {
        if ((*it)->getTarget() == target)
        {
            (*it)->add(relationType, usePercent);
            return;
        }
    }
    RelationItem* newItem = new RelationItem(_owner);
    newItem->setTarget(target);
    newItem->add(relationType, usePercent);
    _items.push_back(newItem);
}

void Relations::addItems(GObject * target, const char * sidePairs)
{
    CCASSERT(target, "target is null");
    CCASSERT(sidePairs, "sidePairs is null");

    RelationItem* newItem = new RelationItem(_owner);
    newItem->setTarget(target);

    RelationType tid = RelationType::Left_Left;
    char temp[20];
    const char*p = sidePairs;

    while (p && (*p) != '\0')
    {
        const char* p2 = strchr(p, ',');
        if (p2)
        {
            strncpy(temp, p, p2 - p);
            p = p2 + 1;
        }
        else
        {
            strcpy(temp, p);
            p = NULL;
        }
        int len = (int)strlen(temp);
        bool usePercent = temp[len - 1] == '%';
        int c1 = temp[0];
        int c2;
        const char* p3 = strchr(temp, '-');
        if (p3)
            c2 = *(p3 + 1);
        else
            c2 = c1;

        switch (c1)
        {
        case 119://width
            tid = RelationType::Width;
            break;

        case 104://height
            tid = RelationType::Height;
            break;

        case 109://middle
            tid = RelationType::Middle_Middle;
            break;

        case 99://center
            tid = RelationType::Center_Center;
            break;

        case 108: //left
            if (p3 - temp > 4) //leftext
            {
                if (c2 == 108)
                    tid = RelationType::LeftExt_Left;
                else
                    tid = RelationType::LeftExt_Right;
            }
            else
            {
                switch (c2)
                {
                case 108:
                    tid = RelationType::Left_Left;
                    break;

                case 114:
                    tid = RelationType::Left_Right;
                    break;

                case 99:
                    tid = RelationType::Left_Center;
                    break;

                default:
                    CCLOGERROR("invalid relation type: %s", sidePairs);
                }
            }
            break;

        case 114: //right
            if (p3 - temp > 5) //rightext
            {
                if (c2 == 108)
                    tid = RelationType::RightExt_Left;
                else
                    tid = RelationType::RightExt_Right;
            }
            else
            {
                switch (c2)
                {
                case 108:
                    tid = RelationType::Right_Left;
                    break;

                case 114:
                    tid = RelationType::Right_Right;
                    break;

                case 99:
                    tid = RelationType::Right_Center;
                    break;

                default:
                    CCLOGERROR("invalid relation type: %s", sidePairs);
                }
            }
            break;

        case 116://top
            if (p3 - temp > 3) //topext
            {
                if (c2 == 116)
                    tid = RelationType::TopExt_Top;
                else
                    tid = RelationType::TopExt_Bottom;
            }
            else
            {
                switch (c2)
                {
                case 116:
                    tid = RelationType::Top_Top;
                    break;

                case 98:
                    tid = RelationType::Top_Bottom;
                    break;

                case 109:
                    tid = RelationType::Top_Middle;
                    break;

                default:
                    CCLOGERROR("invalid relation type: %s", sidePairs);
                }
            }
            break;

        case 98://bottom
            if (p3 - temp > 6) //bottomext
            {
                if (c2 == 116)
                    tid = RelationType::BottomExt_Top;
                else
                    tid = RelationType::BottomExt_Bottom;
            }
            else
            {
                switch (c2)
                {
                case 116:
                    tid = RelationType::Bottom_Top;
                    break;

                case 98:
                    tid = RelationType::Bottom_Bottom;
                    break;

                case 109:
                    tid = RelationType::Bottom_Middle;
                    break;

                default:
                    CCLOGERROR("invalid relation type: %s", sidePairs);
                }
            }
            break;

        default:
            CCLOGERROR("invalid relation type: %s", sidePairs);
        }

        newItem->internalAdd(tid, usePercent);
    }

    _items.push_back(newItem);
}

void Relations::remove(GObject * target, RelationType relationType)
{
    for (auto it = _items.begin(); it != _items.end(); )
    {
        if ((*it)->getTarget() == target)
        {
            (*it)->remove(relationType);
            if ((*it)->isEmpty())
            {
                delete (*it);
                it = _items.erase(it);
            }
            else
                it++;
        }
        else
            it++;
    }
}

bool Relations::contains(GObject * target)
{
    for (auto it = _items.begin(); it != _items.end(); ++it)
    {
        if ((*it)->getTarget() == target)
            return true;
    }

    return false;
}

void Relations::clearFor(GObject * target)
{
    for (auto it = _items.begin(); it != _items.end(); )
    {
        if ((*it)->getTarget() == target)
        {
            delete (*it);
            it = _items.erase(it);
        }
        else
            it++;
    }
}

void Relations::clearAll()
{
    for (auto it = _items.begin(); it != _items.end(); ++it)
        delete (*it);
    _items.clear();
}

void Relations::copyFrom(const Relations & source)
{
    clearAll();

    for (auto it = source._items.begin(); it != source._items.end(); ++it)
    {
        RelationItem* item = new RelationItem(_owner);
        item->copyFrom(**it);
        _items.push_back(item);
    }
}

void Relations::onOwnerSizeChanged(float dWidth, float dHeight)
{
    for (auto it = _items.begin(); it != _items.end(); ++it)
        (*it)->applyOnSelfSizeChanged(dWidth, dHeight);
}

bool Relations::isEmpty() const
{
    return _items.size() == 0;
}

void Relations::setup(TXMLElement * xml)
{
    TXMLElement* cxml = xml->FirstChildElement("relation");

    GObject* target;
    while (cxml)
    {
        const char *targetId = cxml->Attribute("target");
        if (_owner->getParent() != nullptr)
        {
            if (targetId && strlen(targetId) > 0)
                target = _owner->getParent()->getChildById(targetId);
            else
                target = _owner->getParent();
        }
        else
        {
            //call from component construction
            target = ((GComponent*)_owner)->getChildById(targetId);
        }
        if (target != nullptr)
        {
            const char* sidePairs = cxml->Attribute("sidePair");
            if (sidePairs)
                addItems(target, sidePairs);
        }

        cxml = cxml->NextSiblingElement("relation");
    }
}

NS_FGUI_END