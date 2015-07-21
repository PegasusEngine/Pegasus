/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	ILaneProxy.h
//! \author	Kevin Boulanger
//! \date	07th November 2013
//! \brief	Proxy interface, used by the editor to interact with the timeline lanes

#ifndef PEGASUS_TIMELINE_SHARED_ILANEPROXY_H
#define PEGASUS_TIMELINE_SHARED_ILANEPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Timeline/Shared/LaneDefs.h"
#include "Pegasus/Timeline/Shared/TimelineDefs.h"

namespace Pegasus {
    namespace Timeline {
        class ITimelineProxy;
        class IBlockProxy;
    }
}

namespace Pegasus {
namespace Timeline {


//! Proxy interface, used by the editor to interact with the timeline lanes
class ILaneProxy
{
public:

    //! Destructor
    virtual ~ILaneProxy() {};

    //! Get the timeline the lane belongs to
    //! \return Timeline the lane belongs to (!= nullptr)
    virtual ITimelineProxy* GetTimeline() const = 0;


    //! Set the name of the lane
    //! \param name New name of the lane, can be empty or nullptr, but no longer than Lane::MAX_NAME_LENGTH
    virtual void SetName(const char * name) = 0;

    //! Get the name of the lane
    //! \return Name of the lane, can be empty or nullptr
    virtual const char * GetName() const = 0;

    //! Test if the name of the lane is defined
    //! \return True if the name is not an empty string
    virtual bool IsNameDefined() const = 0;


    //! Set the position of a block in the lane
    //! \warning The block has to belong to the lane
    //! \note If the block is not found in the lane, the block is not moved,
    //!       to not break the sorted linked list of another lane
    //! \param block Block to move, has to belong to the lane
    //! \param beat New position of the block, measured in ticks
    virtual void SetBlockBeat(IBlockProxy * block, Beat beat) = 0;

    //! Set the duration of a block in the lane
    //! \warning The block has to belong to the lane
    //! \note If the block is not found in the lane, the block is not resized,
    //!       to not break the sorted linked list of another lane
    //! \param duration New duration of the block, measured in ticks (> 0)
    virtual void SetBlockDuration(IBlockProxy * block, Duration duration) = 0;

    //! Test if a block would fit in the lane
    //! \param Block to test for a fit
    //! \param beat New position of the block, measured in ticks
    //! \param duration New duration of the block, measured in ticks (> 0)
    //! \return True if the block would fit in the lane, and if there is enough space to store it
    virtual bool IsBlockFitting(IBlockProxy * block, Beat beat, Duration duration) const = 0;

    //! Move a block from this lane into another lane
    //! \warning The block has to belong to the lane
    //! \note If any error happens, nothing changes to preserve the sorted linked lists
    //! \param block Block to move, has to belong to the lane
    //! \param newLane Lane to move the block into, different from the current lane, != nullptr
    //! \param beat New position of the block, measured in ticks
    virtual void MoveBlockToLane(IBlockProxy * block, ILaneProxy * newLane, Beat beat) = 0;


    //! Get the list of blocks of the lane
    //! \param blocks Allocated array of LANE_MAX_NUM_BLOCKS pointers to IBlockProxy,
    //!               contains the resulting list of block proxy pointers.
    //! \note Only the valid blocks have their pointers updated
    //! \return Number of block proxy pointers written to the \a blockList array (<= LANE_MAX_NUM_BLOCKS)
    virtual unsigned int GetBlocks(IBlockProxy ** blocks) const = 0;
};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TIMELINE_SHARED_ILANEPROXY_H
