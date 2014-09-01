/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineBlockRegistration.cpp
//! \author	Kevin Boulanger
//! \date	31st August 2014
//! \brief	Global registration of the Pegasus-side timeline blocks

#include "Pegasus/TimelineBlock/TimelineBlockRegistration.h"
#include "Pegasus/Timeline/Timeline.h"

#include "Pegasus/TimelineBlock/Camera/CameraLineBlock.h"

namespace Pegasus {
namespace TimelineBlock {


void RegisterBaseBlocks(Timeline::Timeline * timeline)
{
    PG_ASSERTSTR(timeline != nullptr, "Trying to register base timeline blocks with an undefined timeline");

    // Camera blocks
    REGISTER_BASE_TIMELINE_BLOCK(CameraLineBlock);
}


}   // namespace TimelineBlock
}   // namespace Pegasus
