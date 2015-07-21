/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineBlockRegistration.h
//! \author	Kevin Boulanger
//! \date	31st August 2014
//! \brief	Global registration of the Pegasus-side timeline blocks

#ifndef PEGASUS_TIMELINEBLOCK_TIMELINEBLOCK_H
#define PEGASUS_TIMELINEBLOCK_TIMELINEBLOCK_H

namespace Pegasus {
    namespace Timeline {
        class TimelineManager;
    }
}

namespace Pegasus {
namespace TimelineBlock {


//! Register the Pegasus-side timeline blocks
//! \warning Required at the initialization of the application, before the app-side blocks get registered
//! \param timeline Timeline of the application
void RegisterBaseBlocks(Timeline::TimelineManager * timelineManager);


}   // namespace TimelineBlock
}   // namespace Pegasus

#endif  // PEGASUS_TIMELINEBLOCK_TIMELINEBLOCK_H
