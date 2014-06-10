/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	BlockProxy.cpp
//! \author	Kevin Boulanger
//! \date	09th November 2013
//! \brief	Proxy object, used by the editor to interact with the timeline blocks

//! \todo Why do we need this in Rel-Debug? LaneProxy should not even be compiled in REL mode
PEGASUS_AVOID_EMPTY_FILE_WARNING

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Timeline/Proxy/BlockProxy.h"
#include "Pegasus/Timeline/Proxy/LaneProxy.h"
#include "Pegasus/Timeline/Block.h"
#include "Pegasus/Timeline/Lane.h"

namespace Pegasus {
namespace Timeline {


BlockProxy::BlockProxy(Block * block)
:   mBlock(block)
{
    PG_ASSERTSTR(block != nullptr, "Trying to create a timeline block proxy from an invalid timeline block object");
}

//----------------------------------------------------------------------------------------

BlockProxy::~BlockProxy()
{
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

const char * BlockProxy::GetEditorString() const
{
    return mBlock->GetEditorString();
}

//----------------------------------------------------------------------------------------

void BlockProxy::SetColor(unsigned char red, unsigned char green, unsigned char blue)
{
    mBlock->SetColor(red, green, blue);
}

//----------------------------------------------------------------------------------------

void BlockProxy::GetColor(unsigned char & red, unsigned char & green, unsigned char & blue) const
{
    mBlock->GetColor(red, green, blue);
}


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
