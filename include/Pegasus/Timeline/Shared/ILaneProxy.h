/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	ILaneProxy.h
//! \author	Kevin Boulanger
//! \date	07th November 2013
//! \brief	Proxy interface, used by the editor to interact with the timeline lanes

#ifndef PEGASUS_TEXTURE_SHARED_ILANEPROXY_H
#define PEGASUS_TEXTURE_SHARED_ILANEPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Timeline/Shared/IBlockProxy.h"
#include "Pegasus/Timeline/Shared/LaneDefs.h"

namespace Pegasus {
namespace Timeline {


//! Proxy interface, used by the editor to interact with the timeline lanes
class ILaneProxy
{
public:

    //! Destructor
    virtual ~ILaneProxy() {};


    //! Set the name of the lane
    //! \param name New name of the lane, can be empty or nullptr, but no longer than Lane::MAX_NAME_LENGTH
    virtual void SetName(const char * name) = 0;

    //! Get the name of the lane
    //! \return Name of the lane, can be empty or nullptr
    virtual const char * GetName() const = 0;


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
#endif  // PEGASUS_TEXTURE_SHARED_ILANEPROXY_H
