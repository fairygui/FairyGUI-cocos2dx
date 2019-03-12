#include "GearBase.h"
#include "GearDisplay.h"
#include "GComponent.h"
#include "utils/ToolSet.h"
#include "utils/ByteBuffer.h"

NS_FGUI_BEGIN
USING_NS_CC;

bool GearBase::disableAllTweenEffect = false;

GearTweenConfig::GearTweenConfig() :
    tween(true),
    easeType(EaseType::QuadOut),
    duration(0.3f),
    delay(0),
    _tweener(nullptr),
    _displayLockToken(0)
{
}

GearBase::GearBase(GObject * owner) :_owner(owner), _tweenConfig(nullptr)
{
}

GearBase::~GearBase()
{
    if (_tweenConfig && _tweenConfig->_tweener)
        _tweenConfig->_tweener->kill();
    CC_SAFE_DELETE(_tweenConfig);
}

void GearBase::setController(GController * value)
{
    if (value != _controller)
    {
        _controller = value;
        if (_controller != nullptr)
            init();
    }
}

GearTweenConfig * GearBase::getTweenConfig()
{
    if (!_tweenConfig)
        _tweenConfig = new GearTweenConfig();

    return _tweenConfig;
}

void GearBase::init()
{
}

void GearBase::addStatus(const std::string&  pageId, ByteBuffer* buffer)
{
}

void GearBase::apply()
{
}

void GearBase::updateState()
{
}

void GearBase::updateFromRelations(float dx, float dy)
{
}

void GearBase::setup(ByteBuffer* buffer)
{
    _controller = _owner->getParent()->getControllerAt(buffer->ReadShort());
    init();

    GearDisplay* gear = dynamic_cast<GearDisplay*>(this);
    if (gear)
    {
        int cnt = buffer->ReadShort();
        for (int i = 0; i < cnt; i++)
            gear->pages.push_back(buffer->ReadS());
    }
    else
    {
        int cnt = buffer->ReadShort();
        for (int i = 0; i < cnt; i++)
        {
            const std::string& page = buffer->ReadS();
            if (page.empty())
                continue;

            addStatus(page, buffer);
        }

        if (buffer->ReadBool())
            addStatus(STD_STRING_EMPTY, buffer);
    }

    if (buffer->ReadBool())
    {
        _tweenConfig = new GearTweenConfig();
        _tweenConfig->easeType = (EaseType)buffer->ReadByte();
        _tweenConfig->duration = buffer->ReadFloat();
        _tweenConfig->delay = buffer->ReadFloat();
    }
}

NS_FGUI_END