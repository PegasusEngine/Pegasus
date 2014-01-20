/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	LaneProxy.h
//! \author	Kevin Boulanger
//! \date	07th November 2013
//! \brief	Proxy object, used by the editor to interact with the timeline lanes

#ifndef PEGASUS_TEXTURE_LANEPROXY_H
#define PEGASUS_TEXTURE_LANEPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Timeline/Shared/ILaneProxy.h"

namespace Pegasus {
    namespace Timeline {
        class Lane;
    }
}

namespace Pegasus {
namespace Timeline {


//! Proxy object, used by the editor to interact with the timeline lanes
class LaneProxy : public ILaneProxy
{
public:

    // Constructor
    //! \param lane Proxied timeline lane object, cannot be nullptr
    LaneProxy(Lane * lane);

    //! Destructor
    virtual ~LaneProxy();

    //! Get the lane associated with the proxy
    //! \return Lane associated with the proxy (!= nullptr)
    inline Lane * GetLane() const { return mLane; }

    //! Get the timeline the lane belongs to
    //! \return Timeline the lane belongs to (!= nullptr)
    virtual ITimelineProxy * GetTimeline() const;


    //! Set the name of the lane
    //! \param name New name of the lane, can be empty or nullptr, but no longer than Lane::MAX_NAME_LENGTH
    virtual void SetName(const char * name);

    //! Get the name of the lane
    //! \return Name of the lane, can be empty or nullptr
    virtual const char * GetName() const;

    //! Test if the name of the lane is defined
    //! \return True if the name is not an empty string
    virtual bool IsNameDefined() const;


    //! Set the position of a block in the lane
    //! \warning The block has to belong to the lane
    //! \note If the block is not found in the lane, the block is not moved,
    //!       to not break the sorted linked list of another lane
    //! \param block Block to move
    //! \param position New position of the block, measured in ticks
    virtual void SetBlockBeat(IBlockProxy * block, Beat beat);

    //! Set the duration of a block in the lane
    //! \warning The block has to belong to the lane
    //! \note If the block is not found in the lane, the block is not resized,
    //!       to not break the sorted linked list of another lane
    //! \param duration New duration of the block, measured in ticks (> 0)
    virtual void SetBlockDuration(IBlockProxy * block, Duration duration);

    //! Test if a block would fit in the lane
    //! \param Block to test for a fit
    //! \param beat New position of the block, measured in ticks
    //! \param duration New duration of the block, measured in ticks (> 0)
    //! \return True if the block would fit in the lane, and if there is enough space to store it
    virtual bool IsBlockFitting(IBlockProxy * block, Beat beat, Duration duration) const;

    //! Move a block from this lane into another lane
    //! \warning The block has to belong to the lane
    //! \note If any error happens, nothing changes to preserve the sorted linked lists
    //! \param block Block to move, has to belong to the lane
    //! \param newLane Lane to move the block into, different from the current lane, != nullptr
    //! \param beat New position of the block, measured in ticks
    virtual void MoveBlockToLane(IBlockProxy * block, ILaneProxy * newLane, Beat beat);


    //! Get the list of blocks of the lane
    //! \param blocks Allocated array of LANE_MAX_NUM_BLOCKS pointers to IBlockProxy,
    //!               contains the resulting list of block proxy pointers.
    //! \note Only the valid blocks have their pointers updated
    //! \return Number of block proxy pointers written to the \a blockList array (<= LANE_MAX_NUM_BLOCKS)
    virtual unsigned int GetBlocks(IBlockProxy ** blocks) const;

    //------------------------------------------------------------------------------------
    
private:

    //! Proxied timeline lane object
    Lane * const mLane;
};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TEXTURE_LANEPROXY_H
