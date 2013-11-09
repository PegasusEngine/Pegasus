/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	ITimelineProxy.h
//! \author	Kevin Boulanger
//! \date	07th November 2013
//! \brief	Proxy interface, used by the editor to interact with the timeline

#ifndef PEGASUS_TEXTURE_SHARED_ITIMELINEPROXY_H
#define PEGASUS_TEXTURE_SHARED_ITIMELINEPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Timeline/Shared/ILaneProxy.h"

namespace Pegasus {
namespace Timeline {


//! Proxy interface, used by the editor to interact with the timeline
class ITimelineProxy
{
public:

    //! Destructor
    virtual ~ITimelineProxy() {};


    //! Create a new lane
    //! \return Proxy interface for a timeline lane, nullptr if the number of lanes is Timeline::MAX_NUM_LANES
    virtual ILaneProxy * CreateLane() = 0;
};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TEXTURE_SHARED_ITIMELINEPROXY_H
