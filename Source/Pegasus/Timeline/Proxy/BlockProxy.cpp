/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	BlockProxy.cpp
//! \author	Karolyn Boulanger
//! \date	09th November 2013
//! \brief	Proxy object, used by the editor to interact with the timeline blocks

//! \todo Why do we need this in Rel-Debug? LaneProxy should not even be compiled in REL mode
PEGASUS_AVOID_EMPTY_FILE_WARNING

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/PegasusAssetTypes.h"
#include "Pegasus/Timeline/Proxy/BlockProxy.h"
#include "Pegasus/Timeline/Proxy/LaneProxy.h"
#include "Pegasus/Timeline/Block.h"
#include "Pegasus/Timeline/Lane.h"
#include "Pegasus/Timeline/TimelineScript.h"
#include "Pegasus/AssetLib/Shared/IAssetProxy.h"
#include "Pegasus/Core/Shared/ISourceCodeProxy.h"
#include "Pegasus/Math/Color.h"
#include "Pegasus/PropertyGrid/Shared/PropertyEventDefs.h"
#include "Pegasus/PropertyGrid/PropertyGridObject.h"

namespace Pegasus {
namespace Timeline {

BlockPropertyGridObjectDecorator::BlockPropertyGridObjectDecorator(Block* block)
    :
    mBlock(block),
    mDecorated(block->GetPropertyGridProxy())
{
}

BlockPropertyGridObjectDecorator::~BlockPropertyGridObjectDecorator()
{
    PEGASUS_EVENT_DISPATCH(static_cast<Pegasus::PropertyGrid::PropertyGridObjectProxy*>(mDecorated)->GetObject(), Pegasus::PropertyGrid::PropertyGridDestroyed);
}

void BlockPropertyGridObjectDecorator::WriteObjectProperty(unsigned int index, const void * inputBuffer, unsigned int inputBufferSize, bool sendMessage)
{
    mDecorated->WriteObjectProperty(index, inputBuffer, inputBufferSize, sendMessage);

    //notify now the block that an internal property has now been updated.
    mBlock->NotifyInternalObjectPropertyUpdated(index);
}


BlockProxy::BlockProxy(Block * block)
:   mBlock(block),
    mPropertyGridDecorator(block)
{
    PG_ASSERTSTR(block != nullptr, "Trying to create a timeline block proxy from an invalid timeline block object");
}

//----------------------------------------------------------------------------------------

BlockProxy::~BlockProxy()
{
}

//----------------------------------------------------------------------------------------

PropertyGrid::IPropertyGridObjectProxy * BlockProxy::GetPropertyGridProxy()
{
    return &mPropertyGridDecorator;
}

//----------------------------------------------------------------------------------------

const PropertyGrid::IPropertyGridObjectProxy * BlockProxy::GetPropertyGridProxy() const
{
    return &mPropertyGridDecorator;
}

//----------------------------------------------------------------------------------------

Beat BlockProxy::GetBeat() const
{
    return mBlock->GetBeat();
}

//----------------------------------------------------------------------------------------

Duration BlockProxy::GetDuration() const
{
    return mBlock->GetDuration();
}

//----------------------------------------------------------------------------------------

ILaneProxy * BlockProxy::GetLane() const
{
    Lane * const lane = mBlock->GetLane();
    if (lane != nullptr)
    {
        return lane->GetProxy();
    }
    else
    {
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

void BlockProxy::SetColor(unsigned char red, unsigned char green, unsigned char blue)
{
    Math::Color8RGB c(red, green, blue);
    mBlock->SetColor(c);
}

//----------------------------------------------------------------------------------------

void BlockProxy::GetColor(unsigned char & red, unsigned char & green, unsigned char & blue) const
{
    Math::Color8RGB c = mBlock->GetColor();
    red = c.red;
    green = c.green;
    blue = c.blue;
}

//----------------------------------------------------------------------------------------

Core::ISourceCodeProxy* BlockProxy::GetScript() const
{
    TimelineScript* helper = mBlock->GetScript();
    if (helper != nullptr)
    {
        return static_cast<Core::ISourceCodeProxy*>(helper->GetProxy());
    }

    return nullptr;
}

//----------------------------------------------------------------------------------------

unsigned BlockProxy::GetGuid() const
{
    return mBlock->GetGuid();
}

//----------------------------------------------------------------------------------------

const char * BlockProxy::GetInstanceName() const
{
    return mBlock->GetName();
}

//----------------------------------------------------------------------------------------

const char* BlockProxy::GetClassName() const
{
    return mBlock->GetClassName();
}

void BlockProxy::AttachScript(Core::ISourceCodeProxy* code)
{
    PG_ASSERT(code->GetOwnerAsset()->GetTypeDesc()->mTypeGuid == ASSET_TYPE_BLOCKSCRIPT.mTypeGuid);
    if (code->GetOwnerAsset()->GetTypeDesc()->mTypeGuid == ASSET_TYPE_BLOCKSCRIPT.mTypeGuid)
    {
        Pegasus::Timeline::TimelineScriptProxy* scriptProxy = static_cast<Pegasus::Timeline::TimelineScriptProxy*>(code);
        Pegasus::Timeline::TimelineScriptRef timelineScript = static_cast<Pegasus::Timeline::TimelineScript*>(scriptProxy->GetObject());
        mBlock->AttachScript(timelineScript); 
    }
}

void BlockProxy::ClearScript()
{
    mBlock->ShutdownScript();
}

}   // namespace Timeline
}   // namespace Pegasus


#endif  // PEGASUS_ENABLE_PROXIES
