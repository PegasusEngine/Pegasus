/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	LaneProxy.cpp
//! \author	Kevin Boulanger
//! \date	07th November 2013
//! \brief	Proxy object, used by the editor to interact with the timeline lanes

//! \todo Why do we need this in Rel-Debug? LaneProxy should not even be compiled in REL mode
PEGASUS_AVOID_EMPTY_FILE_WARNING

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Timeline/LaneProxy.h"
#include "Pegasus/Timeline/Lane.h"
#include "Pegasus/Timeline/BlockProxy.h"

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

void LaneProxy::SetBlockPosition(IBlockProxy * block, float position)
{
    // Convert the abstract proxy to a concrete proxy, relying on the fact that
    // BlockProxy is the only possible derived class from IBlockProxy
    BlockProxy * blockProxy = static_cast<BlockProxy *>(block);

    mLane->SetBlockPosition(blockProxy->GetBlock(), position);
}

//----------------------------------------------------------------------------------------

void LaneProxy::SetBlockLength(IBlockProxy * block, float length)
{
    // Convert the abstract proxy to a concrete proxy, relying on the fact that
    // BlockProxy is the only possible derived class from IBlockProxy
    BlockProxy * blockProxy = static_cast<BlockProxy *>(block);

    mLane->SetBlockLength(blockProxy->GetBlock(), length);
}

//----------------------------------------------------------------------------------------

unsigned int LaneProxy::GetBlocks(IBlockProxy ** blocks) const
{
    return mLane->GetBlocks(blocks);
}


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
