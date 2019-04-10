#include "TranslationHelper.h"
#include "PackageItem.h"
#include "UIPackage.h"
#include "utils/ByteBuffer.h"
#include "tinyxml2/tinyxml2.h"

USING_NS_CC;
NS_FGUI_BEGIN

using namespace std;

std::unordered_map<std::string, std::unordered_map<std::string, std::string>> TranslationHelper::strings;

void TranslationHelper::loadFromXML(const char *xmlString, size_t nBytes)
{
    strings.clear();

    tinyxml2::XMLDocument* xml = new tinyxml2::XMLDocument();
    xml->Parse(xmlString, nBytes);

    tinyxml2::XMLElement* root = xml->RootElement();
    tinyxml2::XMLElement* ele = root->FirstChildElement("string");
    while (ele)
    {
        std::string key = ele->Attribute("name");
        std::string text = ele->GetText();
        size_t i = key.find("-");
        if (i == std::string::npos)
            continue;

        std::string key2 = key.substr(0, i);
        std::string key3 = key.substr(i + 1);
        std::unordered_map<std::string, std::string>& col = strings[key2];
        col[key3] = text;

        ele = ele->NextSiblingElement("string");
    }

    delete xml;
}

void TranslationHelper::translateComponent(PackageItem * item)
{
    if (strings.empty())
        return;

    auto col = strings.find(item->owner->getId() + item->id);
    if (col == strings.end())
        return;

    std::unordered_map<std::string, std::string>& strings = col->second;

    ByteBuffer* buffer = item->rawData;

    buffer->Seek(0, 2);

    int childCount = buffer->ReadShort();
    for (int i = 0; i < childCount; i++)
    {
        int dataLen = buffer->ReadShort();
        int curPos = buffer->position;

        buffer->Seek(curPos, 0);

        ObjectType type = (ObjectType)buffer->ReadByte();
        buffer->Skip(4);
        const string& elementId = buffer->ReadS();

        if (type == ObjectType::COMPONENT)
        {
            if (buffer->Seek(curPos, 6))
                type = (ObjectType)buffer->ReadByte();
        }

        buffer->Seek(curPos, 1);

        auto it = strings.find(elementId + "-tips");
        if (it != strings.end())
            buffer->WriteS(it->second);

        buffer->Seek(curPos, 2);

        int gearCnt = buffer->ReadShort();
        for (int j = 0; j < gearCnt; j++)
        {
            int nextPos = buffer->ReadShort();
            nextPos += buffer->position;

            if (buffer->ReadByte() == 6) //gearText
            {
                buffer->Skip(2);//controller
                int valueCnt = buffer->ReadShort();
                for (int k = 0; k < valueCnt; k++)
                {
                    const string& page = buffer->ReadS();
                    if (!page.empty())
                    {
                        if ((it = strings.find(elementId + "-texts_" + Value(k).asString())) != strings.end())
                            buffer->WriteS(it->second);
                        else
                            buffer->Skip(2);
                    }
                }

                if (buffer->ReadBool() && (it = strings.find(elementId + "-texts_def")) != strings.end())
                    buffer->WriteS(it->second);
            }

            buffer->position = nextPos;
        }

        switch (type)
        {
        case ObjectType::TEXT:
        case ObjectType::RICHTEXT:
        case ObjectType::INPUTTEXT:
        {
            if ((it = strings.find(elementId)) != strings.end())
            {
                buffer->Seek(curPos, 6);
                buffer->WriteS(it->second);
            }
            if ((it = strings.find(elementId + "-prompt")) != strings.end())
            {
                buffer->Seek(curPos, 4);
                buffer->WriteS(it->second);
            }
            break;
        }

        case ObjectType::LIST:
        {
            buffer->Seek(curPos, 8);
            buffer->Skip(2);
            int itemCount = buffer->ReadShort();
            for (int j = 0; j < itemCount; j++)
            {
                int nextPos = buffer->ReadShort();
                nextPos += buffer->position;

                buffer->Skip(2); //url
                if ((it = strings.find(elementId + "-" + Value(j).asString())) != strings.end())
                    buffer->WriteS(it->second);
                else
                    buffer->Skip(2);
                if ((it = strings.find(elementId + "-" + Value(j).asString() + "-0")) != strings.end())
                    buffer->WriteS(it->second);
                buffer->position = nextPos;
            }
            break;
        }

        case ObjectType::LABEL:
        {
            if (buffer->Seek(curPos, 6) && (ObjectType)buffer->ReadByte() == type)
            {
                if ((it = strings.find(elementId)) != strings.end())
                    buffer->WriteS(it->second);
                else
                    buffer->Skip(2);

                buffer->Skip(2);
                if (buffer->ReadBool())
                    buffer->Skip(4);
                buffer->Skip(4);
                if (buffer->ReadBool() && (it = strings.find(elementId + "-prompt")) != strings.end())
                    buffer->WriteS(it->second);
            }
            break;
        }

        case ObjectType::BUTTON:
        {
            if (buffer->Seek(curPos, 6) && (ObjectType)buffer->ReadByte() == type)
            {
                if ((it = strings.find(elementId)) != strings.end())
                    buffer->WriteS(it->second);
                else
                    buffer->Skip(2);
                if ((it = strings.find(elementId + "-0")) != strings.end())
                    buffer->WriteS(it->second);
            }
            break;
        }

        case ObjectType::COMBOBOX:
        {
            if (buffer->Seek(curPos, 6) && (ObjectType)buffer->ReadByte() == type)
            {
                int itemCount = buffer->ReadShort();
                for (int j = 0; j < itemCount; j++)
                {
                    int nextPos = buffer->ReadShort();
                    nextPos += buffer->position;

                    if ((it = strings.find(elementId + "-" + Value(j).asString())) != strings.end())
                        buffer->WriteS(it->second);

                    buffer->position = nextPos;
                }

                if ((it = strings.find(elementId)) != strings.end())
                    buffer->WriteS(it->second);
            }

            break;
        }
        default:
            break;
        }

        buffer->position = curPos + dataLen;
    }
}

NS_FGUI_END
