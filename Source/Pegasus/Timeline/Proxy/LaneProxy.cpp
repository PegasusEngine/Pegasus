/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	LaneProxy.cpp
//! \author	Karolyn Boulanger
//! \date	07th November 2013
//! \brief	Proxy object, used by the editor to interact with the timeline lanes

//! \todo Why do we need this in Rel-Debug? LaneProxy should not even be compiled in REL mode
PEGASUS_AVOID_EMPTY_FILE_WARNING

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Timeline/Proxy/LaneProxy.h"
#include "Pegasus/Timeline/Proxy/TimelineProxy.h"
#include "Pegasus/Timeline/Proxy/BlockProxy.h"
#include "Pegasus/Timeline/Lane.h"
#include "Pegasus/Timeline/Timeline.h"

namespace Pegasus {
namespace Timeline {

    
LaneProxy::LaneProxy(Lane * lane)
:   mLane(lane)
{
    PG_ASSERTSTR(lane != nullptr, "Trying to create a timeline lane proxy from an invalid timeline lane object");
}

//----------------------------------------------------------------------------------------

LaneProxy::~LaneProxy()
{
}

//----------------------------------------------------------------------------------------

ITimelineProxy * LaneProxy::GetTimeline() const
{
    Timeline * const timeline = mLane->GetTimeline();
    if (timeline != nullptr)
    {
        return timeline->GetProxy();
    }
    else
    {
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

void LaneProxy::SetName(const char * name)
{
    mLane->SetName(name);
}

//----------------------------------------------------------------------------------------

const char * LaneProxy::GetName() const
{
    return mLane->GetName();
}

//----------------------------------------------------------------------------------------

bool LaneProxy::IsNameDefined() const
{
    return mLane->IsNameDefined();
}

//----------------------------------------------------------------------------------------

void LaneProxy::SetBlockBeat(IBlockProxy * block, Beat beat)
{
    // Convert the abstract proxy to a concrete proxy, relying on the fact that
    // BlockProxy is the only possible derived class from IBlockProxy
    BlockProxy * blockProxy = static_cast<BlockProxy *>(block);

    if (blockProxy != nullptr)
    {
        mLane->SetBlockBeat(blockProxy->GetBlock(), beat);
    }
}

//----------------------------------------------------------------------------------------

void LaneProxy::SetBlockDuration(IBlockProxy * block, Duration duration)
{
    // Convert the abstract proxy to a concrete proxy, relying on the fact that
    // BlockProxy is the only possible derived class from IBlockProxy
    BlockProxy * blockProxy = static_cast<BlockProxy *>(block);

    if (blockProxy != nullptr)
    {
        mLane->SetBlockDuration(blockProxy->GetBlock(), duration);
    }
}

//----------------------------------------------------------------------------------------

bool LaneProxy::IsBlockFitting(IBlockProxy * block, Beat beat, Duration duration) const
{
    // Convert the abstract proxy to a concrete proxy, relying on the fact that
    // BlockProxy is the only possible derived class from IBlockProxy
    BlockProxy * blockProxy = static_cast<BlockProxy *>(block);

    if (blockProxy != nullptr)
    {
        return mLane->IsBlockFitting(blockProxy->GetBlock(), beat, duration);
    }
    else
    {
        return false;
    }
}

//----------------------------------------------------------------------------------------

void LaneProxy::MoveBlockToLane(IBlockProxy * block, ILaneProxy * newLane, Beat beat)
{
    // Convert the abstract block proxy to a concrete proxy, relying on the fact that
    // BlockProxy is the only possible derived class from IBlockProxy
    BlockProxy * blockProxy = static_cast<BlockProxy *>(block);

    // Convert the abstract lane proxy to a concrete proxy, relying on the fact that
    // LaneProxy is the only possible derived class from ILaneProxy
    LaneProxy * newLaneProxy = static_cast<LaneProxy *>(newLane);

    if ((blockProxy != nullptr) && (newLaneProxy != nullptr))
    {
        mLane->MoveBlockToLane(blockProxy->GetBlock(), newLaneProxy->GetLane(), beat);
    }
}

//----------------------------------------------------------------------------------------

unsigned int LaneProxy::GetBlocks(IBlockProxy ** blocks) const
{
    return mLane->GetBlocks(blocks);
}


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
