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
#include "Pegasus/Timeline/TimelineManager.h"

#include "Pegasus/TimelineBlock/Camera/CameraLineBlock.h"

using namespace Pegasus;
using namespace Pegasus::Timeline;

namespace Pegasus {
namespace TimelineBlock {


void RegisterBaseBlocks(Timeline::TimelineManager * timelineManager)
{
    PG_ASSERTSTR(timelineManager != nullptr, "Trying to register base timeline blocks with an undefined timeline");

    // Empty block
    REGISTER_BASE_TIMELINE_BLOCK(Block);

    // Camera blocks
    REGISTER_BASE_TIMELINE_BLOCK(CameraLineBlock);
}


}   // namespace TimelineBlock
}   // namespace Pegasus
